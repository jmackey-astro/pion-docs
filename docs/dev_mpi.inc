.. _ref-dev-mpi:

Parallel communication
================================

Parallel execution in PION uses the `Message Passing Interface (MPI) <https://en.wikipedia.org/wiki/Message_Passing_Interface>`_ library, with one sub-domain of each grid level per MPI process, and OpenMP threads within each process (see :ref:`parallel` for how to run in parallel).

All MPI calls are in the class ``Sub_domain`` (``source/sub_domain/``), rather than being scattered through the code.
Each grid level has its own ``Sub_domain`` object (``SimPM.levels[l].sub_domain``), which holds the decomposition of that level and does the communication for it:

+ ``sub_domain_setup.cpp``: the domain decomposition, the MPI communicators, and the lists of processes that hold the coarser and finer levels of a nested grid.
+ ``sub_domain_comms.cpp``: sending and receiving data, global operations such as the minimum or maximum over all processes (``global_operation_double()``), broadcasts, and the round-robin control of parallel Silo I/O.
+ ``sub_domain_access.cpp``: functions that return information about the decomposition.


Domain decomposition
-----------------------------------------------------------

The number of MPI processes must be a power of 2.
``Sub_domain::calculate_process_topology()`` splits the domain into :math:`N_P` sub-domains by repeatedly halving it along the axis where the sub-domains are longest, so the number of sub-domains along each axis is 1 or an even number.
The sub-domains are arranged on an MPI Cartesian communicator (``MPI_Cart_create``), which also gives the ranks of the neighbouring processes.

``decomposeDomain()`` sets the extent and number of cells of the sub-domain of this process.
It is called by ``sim_control_pllel::Init()`` (uniform grid) and by ``setup_grid_NG_MPI`` (nested grid, once for each level).
For nested grids all levels have the same number of cells, so every level uses the decomposition of level 0, and ``decomposeDomain()`` also finds the processes that hold the parent (coarser) and child (finer) grids of this sub-domain.
It checks that each sub-domain of a refined level lies inside the sub-domain of one process on the coarser level, and stops with an error if not (see :ref:`parallel`).


Calculating timestep
-----------------------------------------------------------

PION uses an explicit time-integration scheme for the ideal-MHD or Euler equations, which is stable as long as no signal can cross a full grid cell in a single timestep.
The function ``calculate_timestep()`` in ``source/sim_control/calc_timestep.cpp`` provides the single-process implementation, and this is re-implemented as ``sim_control_pllel::calculate_timestep()`` for multi-process simulations in ``source/sim_control/sim_control_MPI.cpp``.
The only difference compared with the single-process version is that the global minimum timestep is found across all sub-domains, with ``Sub_domain::global_operation_double()``.


.. _ref-dev-mpi-boundaries:

Exchange of boundary data on sub-domains
-----------------------------------------------------------

Layers of boundary (or ghost) cells are added to pad each sub-domain, constituting duplicated data that is calculated on the neighbouring sub-domains.
On a nested grid with 2nd-order accuracy, the layers are 4 cells deep for boundaries between sub-domains, and 6 cells deep for the outer boundaries of each level's domain.

For a 3D simulation the :math:`\pm\hat{x}` boundaries have the smallest extent (extending to :math:`[y_\mathrm{min},y_\mathrm{max}]` and :math:`[z_\mathrm{min},z_\mathrm{max}]`) and :math:`\pm\hat{z}` boundaries are the largest, extending to include the edge and corner ghost cells in the :math:`\pm\hat{x}` and :math:`\pm\hat{y}` directions.
The layout of the boundaries is plotted in :numref:`boundaries`, showing how the shape of the `sub-domain plus boundaries` cuboid is the same as that of the `sub-domain` itself, with extra layers of cells on all sides.

  .. _boundaries:
  .. Figure:: img/boundaries.png
    :scale: 40 %
    :alt: Layout of boundary regions for a 3D simulation.
    :align: center

    Layout of boundary regions for a 3D simulation.  A cross-section through a plane of constant :math:`z` is shown on the left, with the layout of the :math:`x` and :math:`y` boundaries.  The right-hand plot shows how the :math:`z` boundaries cover both the  :math:`x` and :math:`y` boundary regions, completing the cuboid shape.


There are two boundary exchanges that use the boundaries of the sub-domains:
 + Data are exchanged with neighbouring sub-domains each timestep for data on the same level, to ensure that the solution remains consistent across all sub-domains (and identical to a calculation run on a single process with a single domain).  These boundaries are called "MPI" boundaries in PION, defined in the class ``sub_domain_bc`` in ``boundaries/sub_domain_boundaries.cpp`` (``BC_assign_BCMPI()`` and ``BC_update_BCMPI()``), and updated by ``TimeUpdateExternalBCs()`` at every timestep.  The exchange uses persistent MPI communications (``Sub_domain::establish_persistent_communications()``), which are set up once and then restarted every time the boundary data are needed.
 + Raytracing to calculate photoionization and photoheating rates must be calculated when radiation sources are present.  This involves communication of attenuation factors across sub-domains, with send and receive functions defined in ``boundaries/RT_MPI_boundaries.cpp``, called by ``raytracing/raytracer_SC_pllel.cpp`` every time a raytracing is required.


.. _ref-dev-ng-boundaries:

Exchange of data between levels of the nested grid
-----------------------------------------------------------

There are three further `boundary` communications needed between levels of a nested grid:

 1. **Restriction**: If a grid has a more refined grid covering part of its sub-domain, then at each timestep the refined grid averages the data from blocks of :math:`2^\mathrm{ndim}` cells and sends this to the coarser grid.  The received data overwrites anything calculated by the coarse grid.  This is called *restriction*, and is referred to in the PION source code as "FINE_TO_COARSE".

    + It is implemented in the classes ``NG_fine_to_coarse_bc`` (serial) and ``NG_MPI_fine_to_coarse_bc`` (parallel) that are defined in the files ``boundaries/NG_fine_to_coarse_boundaries.cpp`` and ``boundaries/NG_MPI_fine_to_coarse_boundaries.cpp``, respectively.
    + The function ``setup_grid_NG_MPI::setup_boundary_structs()`` in ``grid/setup_grid_NG_MPI.cpp`` adds ``FINE_TO_COARSE_RECV`` and/or ``FINE_TO_COARSE_SEND`` boundaries to the list of boundaries if this sub-domain needs to receive averaged data from a finer-level sub-domain or needs to send averaged data to a coarser-level sub-domain.
    + There are functions to assign the boundary data to be sent/received.  These are called only once, in the function ``assign_update_bcs_NG_MPI::assign_boundary_data()`` defined in ``boundaries/assign_update_bcs_NG_MPI.cpp``.
    + There are functions to update the boundary data, i.e. exchange data between levels, possibly on another MPI process.  These are called every timestep by the functions ``Time_Int()`` and ``advance_step_OA1()`` or ``advance_step_OA2()`` in ``sim_control/sim_control_NG_MPI.cpp``.

 2. **Prolongation**: All refined grids are enclosed by coarser grids, and the outer boundary of a refined grid therefore needs to be updated by interpolating the solution from the coarse grid onto the external ghost cells of the refined grid.  This process is known as *prolongation*.  It is trickier and more computationally intensive than restriction, because of the interpolation and the requirement to conserve mass, momentum and energy.  PION calculates slopes in the primitive variables in each direction for each coarse cell, and sends the primitive vector and the slope vectors to the refined grid.  The refined grid then interpolates onto the cell-centres of its cells using these slopes, and applies a correction to ensure conservation is maintained.  In PION source code this process is denoted "COARSE_TO_FINE".

     + It is implemented in the classes ``NG_coarse_to_fine_bc`` (serial) and ``NG_MPI_coarse_to_fine_bc`` (parallel) in the files ``boundaries/NG_coarse_to_fine_boundaries.cpp`` and ``boundaries/NG_MPI_coarse_to_fine_boundaries.cpp``, respectively.
     + The function ``setup_grid_NG_MPI::setup_boundary_structs()`` in ``grid/setup_grid_NG_MPI.cpp`` sets external sub-domain boundaries to be ``COARSE_TO_FINE_RECV`` type if needed.  It also adds ``COARSE_TO_FINE_SEND`` boundaries to the list of boundaries if finer-level sub-domains below this sub-domain have level boundaries.
     + Again there are functions to assign the boundary data to be sent/received, called only once, in the function ``assign_update_bcs_NG_MPI::assign_boundary_data()`` defined in ``boundaries/assign_update_bcs_NG_MPI.cpp``.
     + There are functions to update the boundary data, i.e. exchange data between levels, possibly on another MPI process.  These are called every timestep by the functions ``Time_Int()`` and ``advance_step_OA1()`` or ``advance_step_OA2()`` in ``sim_control/sim_control_NG_MPI.cpp``.
     + The send and receive buffers are sized differently for 1st- and 2nd-order spatial accuracy (``OrderOfAccSpace``), and the sender and receiver must agree on the layout.  The packing loops run in OpenMP threads, so writing past the end of one cell's slot in a buffer corrupts the data of the next cell.
     + The same boundaries also pass column densities from the coarse level to the fine level, for radiation sources outside the fine level (``BC_update_COARSE_TO_FINE_RT()``).

 3. **Flux Conservation**: To maintain conservation, the flux crossing the boundaries of refined grids must be kept consistent with the flux crossing the same surface in the coarser grids.  This process was pioneered by `Berger & Colella (1989) <https://scixplorer.org/abs/1989JCoPh..82...64B/abstract>`_, hence in PION the methods are referred to as "BC89".  The cells on the fine and coarse grids that have a face on the outer boundary of the fine grid are identified and an extra flux vector `F` is initialised only on these cells.  Corner cells may have up to three of these flux vectors, so they are put in a vector of arrays, but most face cells only have one.  Time-averaged fluxes over two fine-grid timesteps are sent to the coarse grid and these are used to correct the coarse-grid fluxes to be consistent with the fine grid (it is assumed that the fine grid is more accurate).

     + This is implemented in the class ``NG_BC89flux`` (serial) and ``NG_MPI_BC89flux`` (parallel), defined in ``boundaries/NG_BC89flux.cpp`` and ``boundaries/NG_MPI_BC89flux.cpp``, respectively.
     + The setup functions make a list of cells that are on a level boundary and need flux correction (both to send to the coarser grid, and to receive from the finer grid).  The function ``setup_flux_vectors()`` in ``boundaries/NG_BC89flux.cpp`` (serial) and ``boundaries/NG_MPI_BC89flux.cpp`` (parallel) is called by ``setup_grid_NG_MPI::setup_grid()`` in ``grid/setup_grid_NG_MPI.cpp``.  The parallel implementation decides whether the grid with the BC89 boundary that we need to exchange data with is on the same MPI process (call the serial function) or on a different process (call the MPI-parallel function).
     + The update functions, ``recv_BC89_fluxes_F2C()`` and ``send_BC89_fluxes_F2C()``, exchange data between levels, possibly on another MPI process.  These are called every timestep by the functions ``Time_Int()`` and ``advance_step_OA1()`` or ``advance_step_OA2()`` in ``sim_control/sim_control_NG_MPI.cpp``.



.. _ref-dev-mpi-io:

Parallel input/output (read from/write to disk)
-----------------------------------------------------------

The only parallel I/O implementation that is optimised for speed uses the Silo library and its PMPIO interface (``dataIO/dataio_silo_MPI.cpp``).
This takes a round-robin approach to reading and writing data: the MPI processes are divided into groups, one for each output file, and only one process per group writes to its file at any one time.
When it is finished, it passes on a `baton` to the next process in its group, which then reads/writes its data, and so on.
The number of files is about one per :math:`3\times10^7` cells of a level, and at most one per process, so most snapshots have one file per level.
Each level of the nested grid is written to its own file(s), and all data on all levels is saved.
