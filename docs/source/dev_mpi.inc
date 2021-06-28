.. _ref-dev-mpi:

Parallel communication
================================

Parallel execution in PION is implemented using the `Message Passing Interface (MPI) <https://en.wikipedia.org/wiki/Message_Passing_Interface>`_ library with a multi-core and multiple domain model.
This is based on domain splitting, where each level of the computational grid is recursively bisected :math:`i` times along the longest dimension until there are :math:`N_P = 2^i` domains, one for each of the :math:`N_P` MPI processes.
For nested-grid simulations the grid has the same shape and size (in terms of numbers of cells) on all levels, and so the same decomposition is performed at each level, and each MPI process has exactly 1 sub-domain (or patch) on each level.

Interprocess communication is in PION is coded such that the implementation and library used is completely encapsulated in a class, so that calls to MPI functions are not scattered throughout the code.
The motivation for this is so that a different communication library could be used in future without significant modification of the PION source code.
There is an abstract base class, ``comms_base``, defined in ``source/comms/comms.h``, which contains the API for interprocess communication.
This contains function declarations for sending and receiving data between processes, for the Silo parallel I/O interface, and various other common tasks such as broadcasting and calculating miminum or maximum across sub-domains.
There are two comms libraries implemented:

+ The class ``comm_files`` in ``source/comms/comm_files.h/.cpp`` is purely for code testing and debugging parallel communication.  It is a very simple communication library that uses disk I/O to send and receive data between processes (and hence it is very slow).  The advantage is that the communicated data can be parsed outside PION if an error occurs, helping with debugging.  This library is, however, poorly maintained, and may no longer work as intended.
+ The class ``comm_mpi`` in ``source/comms/comm_mpi.h/.cpp`` is used in all other cases.  This uses the system MPI library to call various MPI functions, enabling interprocess communication.  It also uses the Silo PMPIO library for parallel I/O.  The rest of this section describes the MPI implementation in more detail.

Whether one uses the file-comms or MPI is a compile-time decision, and a (global) pointer to the communications class called ``COMM`` is initialised with the correct class at run-time.




Domain decomposition
-----------------------------------------------------------

The decomposition of the domain is controlled by the *"Multi-Core, Multiple-Domain (MCMD)"* class in ``decomposition/MCMD_control.cpp``.
The function ``decomposeDomain()`` is called by ``Init()`` from either ``sim_control/sim_control_MPI.cpp`` (uniform grid, called on a single level) or ``sim_control/sim_control_NG_MPI.cpp`` (nested grid, called once for each level).
This is called before each process sets up the grid for its sub-domain, and the function sets the extents and numbers of cells for each sub-domain based on the MPI rank of the process.
It also sets the ranks of the neighbouring MPI processes, if applicable.


Calculating timestep
-----------------------------------------------------------

PION uses an explicit time-integration scheme for the ideal-MHD or Euler equations, which is stable as long as no signal can cross a full grid cell in a single timestep.  The function ``calculate_timestep()`` in ``source/sim_control/calc_timestep.h/.cpp`` provides the single-process implementation, and this is re-implemented as ``sim_control_pllel::calculate_timestep()`` for multi-process simulations in ``source/sim_control/sim_control_MPI.h/.cpp``.
The only difference compared with the single-process version is that the global minimum timestep is calculated across all sub-domains, via calls to the ``COMM`` class pointer.


.. _ref-dev-mpi-boundaries:

Exchange of boundary data on sub-domains
-----------------------------------------------------------

Layers of boundary (or ghost) cells are added to pad each sub-domain, constituting duplicated data that is calculated on the neighbouring sub-domains.
The layers are 4 cells deep for boundaries between sub-domains, and 6 cells deep for the outer boundaries of each level's domain.

for a 3D simulation the :math:`\pm\hat{x}` boundaries have the smallest extent (extending to :math:`[y_\mathrm{min},y_\mathrm{max}]` and :math:`[z_\mathrm{min},z_\mathrm{max}]`) and :math:`\pm\hat{z}` boundaries are the largest, extending to include the edge and corner ghost cells in the :math:`\pm\hat{x}` and :math:`\pm\hat{y}` directions.
The layout of the boundaries is plotted in :numref:`boundaries`, showing how the shape of the `sub-domain plus boundaries` cuboid is the same as that of the `sub-domain` itself, with extra layers of cells on all sides.

  .. _boundaries:
  .. Figure:: img/boundaries.png
    :scale: 40 %
    :alt: Layout of boundary regions for a 3D simulation.
    :align: center
 
    Layout of boundary regions for a 3D simulation.  A cross-section through a plane of constant :math:`z` is shown on the left, with the layout of the :math:`x` and :math:`y` boundaries.  The right-hand plot shows how the :math:`z` boundaries cover both the  :math:`x` and :math:`y` boundary regions, completing the cuboid shape.


There are two boundary exchanges that use the boundaries of the sub-domains:
 + Data are exchanged with neighbouring sub-domains each timestep for data on the same level, to ensure that the solution remains consistent across all sub-domains (and identical to a calculation run on a single process with a single domain).  These boundaries are called "MPI" boundaries in PION, defined in ``boundaries/MCMD_boundaries.h/.cpp``, called by ``TimeUpdateExternalBCs()`` in ``boundaries/assign_update_bcs_MPI.cpp`` at every timestep.
 + Raytracing to calculate photoionization and photoheating rates must be calculated when radiation sources are present.  This involves communication of attenuation factors across sub-domains, with send and receive functions defined in ``boundaries/RT_MPI_boundaries.h/.cpp``, called by ``raytracing/raytracer_SC_pllel.cpp`` every time a raytracing is required.


.. _ref-dev-ng-boundaries:

Exchange of data between levels of the nested grid
-----------------------------------------------------------

There are three further `boundary` communications needed between levels of a nested grid:

 1. **Restriction**: If a grid has a more refined grid covering part of its sub-domain, then at each timestep the refined grid averages the data from blocks of :math:`2^3` cells and sends this to the coarser grid.  The received data overwrites anything calculated by the coarse grid.  This is called *restriction*, and is referred to in the PION source code as "FINE2COARSE".

    + It is implemented in the classes ``NG_fine_to_coarse_bc`` (serial) and ``NG_MPI_fine_to_coarse_bc`` (parallel) that are defined in the files ``boundaries/NG_fine_to_coarse_boundaries.h/.cpp`` and ``boundaries/NG_MPI_fine_to_coarse_boundaries.h/.cpp``, respectively.
    + the function ``setup_grid_NG_MPI::setup_boundary_structs()`` in ``grid/setup_grid_NG_MPI.cpp`` adds  ``FINE_TO_COARSE_RECV`` and/or ``FINE_TO_COARSE_SEND`` boundaries to the list of boundaries if this sub-domain needs to receive smoothed data from a finer-level sub-domain or needs to send smoothed data to a coarser-level sub-domain.
    + There are functions to assign the boundary data to be sent/received.  These are called only once, in the function ``assign_update_bcs_NG_MPI::assign_boundary_data()`` defined in ``boundaries/assign_update_bcs_NG_MPI.cpp``.
    + There are function to update the boundary data, i.e. exchange data between levels, possibly on another MPI process.  These are called every timestep by the functions ``Time_Int()`` and ``advance_step_OA1()`` or ``advance_step_OA2()`` in ``sim_control/sim_control_NG_MPI.cpp``.
   
 2. **Prolongation**: All refined grids are enclosed by coarser grids, and the outer boundary of a refined grid therefore needs to be updated by interpolating the solution from the coarse grid onto the external ghost cells of the refined grid.  This process is known as *prolongation*.  It is trickier and more computationally intensive than restriction, because of the interpolation and the requirement to conserve mass, momentum and energy.  PION calculates slopes in the primitive variables in each direction for each coarse cell, and sends the primitive vector and the 3 slope vectors to the refined grid.  The refined grid then interpolates onto the cell-centres of its cells using these slopes, and applies a correction to ensure conservation is maintained.  In PION source code this process is denoted "COARSE2FINE".
 
     + It is implemented in the classes ``NG_coarse_to_fine_bc`` (serial) and ``NG_MPI_coarse_to_fine_bc`` (parallel) in the files ``boundaries/NG_MPI_coarse_to_fine_boundaries.h`` and ``boundaries/NG_MPI_coarse_to_fine_boundaries.cpp``, respectively.
     + the function ``setup_grid_NG_MPI::setup_boundary_structs()`` in ``grid/setup_grid_NG_MPI.cpp`` sets external sub-domain boundaries to be ``COARSE_TO_FINE_RECV`` type if needed.  It also adds ``COARSE_TO_FINE_SEND`` boundaries to the list of boundaries if finer-level sub-domains below this sub-domain has level boundaries.
     + Again there are functions to assign the boundary data to be sent/received, called only once, in the function ``assign_update_bcs_NG_MPI::assign_boundary_data()`` defined in ``boundaries/assign_update_bcs_NG_MPI.cpp``.
     + There are function to update the boundary data, i.e. exchange data between levels, possibly on another MPI process.  These are called every timestep by the functions ``Time_Int()`` and ``advance_step_OA1()`` or ``advance_step_OA2()`` in ``sim_control/sim_control_NG_MPI.cpp``.

 3. **Flux Conservation**: To maintain conservation, the flux crossing the boundaries of refined grids must be kept consistent with the flux crossing the same surface in the coarser grids.  This process was pioneered by `Berger & Colella (1989) <https://ui.adsabs.harvard.edu/abs/1989JCoPh..82...64B/abstract>`_, hence in PION the methods are referred to as "BC89".  The cells on the fine and coarse grids that have a face on the outer boundary of the fine grid are identified and an extra flux vector `F` is initialised only on these cells.  Corner cells may have up to three of these flux vectors, so they are put in a vector of arrays, but most face cells only have one.  Time-averaged fluxes over two fine-grid timesteps are sent to the coarse grid and these are used to correct the coarse-grid fluxes to be consistent with the fine grid (it is assumed that the fine grid is more accurate).

     + This is implemented in the class ``NG_BC89flux`` (serial) and ``NG_MPI_BC89flux`` (parallel), defined in ``boundaries/NG_BC89flux.cpp`` and ``boundaries/NG_MPI_BC89flux.h/.cpp``, respectively.
     + The setup functions make a list of cells that are on a level boundary and need flux correction (both to send to coarser grid, and to receive from finer grid).  The function ``NG_BC89flux::setup_flux_vectors()`` in ``boundaries/NG_BC89flux.cpp`` (serial) and ``boundaries/NG_MPI_BC89flux.cpp`` (parallel) is called by ``setup_grid_NG_MPI::setup_grid()`` in ``grid/setup_grid_NG_MPI.cpp``.  The parallel implementation decides whether the grid with the BC89 boundary that we need to exchange data with is on the same MPI process (call serial function) or on a different process (call the MPI-parallel function).
     + The update functions, ``recv_BC89_fluxes_F2C()`` and ``send_BC89_fluxes_F2C()``, exchange data between levels, possibly on another MPI process.  These are called every timestep by the functions ``Time_Int()`` and ``advance_step_OA1()`` or ``advance_step_OA2()`` in ``sim_control/sim_control_NG_MPI.cpp``.



.. _ref-dev-mpi-io:

Parallel input/output (read from/write to disk)
-----------------------------------------------------------

The only parallel I/O implementation that is optimised for speed is using the Silo library and its PMPIO interface.
This takes a round-robin approach to reading and writing data, where :math:`N` MPI processes (about 8 or 16) can work simultaneously.
Each MPI process is assigned to one of :math:`N` groups of processes, and only one process per group can read or write at any one time.
When it is finished, it passes  on a `baton` to the next process in its group, which then reads/writes its data, and so on.
The motivation is that most supercomputers support multiple simultaneous connections to the file system for parallel I/O, but the number of these is much smaller than the number of cores on the system.
Each level of the nested grid is written to its own file, and all data on all levels is saved.




