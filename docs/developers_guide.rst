.. _dev-guide:

*********************************
Developers Guide for PION
*********************************

This guide is for people who want to change or extend PION.
It describes how to contribute changes, how the source code is organised, and how the main parts of the code work.
The classes and functions are documented in more detail in the code documentation made by Doxygen (see `Code documentation`_).

.. include:: dev_coding.inc


.. _ref-dev-source:

Source code layout
==================================

The PION repository contains:

+ ``source/``: the PION source code (see below).
+ ``test/``: the tests run by ``ctest`` and the CI pipeline (see :ref:`tests`), with their parameter files in ``test/ci-inputs/`` and reference snapshots in ``test/data/``.  ``test/problems/`` has parameter files for many other test problems, which are not run automatically.
+ ``analysis/``: tools for analysing snapshots, e.g. ``silocompare``, which the tests use to compare snapshots.
+ ``tools/``: scripts for developers, e.g. ``tools/format.sh``.
+ ``scripts/``: example scripts for building and running PION, e.g. ``scripts/slurm-example.sh``.
+ ``ci/``: the Dockerfile of the image that the CI pipeline runs in.
+ ``doc/``: the Doxygen configuration.

The directories in ``source/`` are:

=====================  ======================================================================
Directory              Contents
=====================  ======================================================================
``main.cpp``,          The ``main()`` functions of ``pion-*`` and ``icgen-*``, and the
``main_icgen.cpp``     simulation parameters (``sim_params.cpp``).
``sim_control/``       Time integration and control of the simulation (see `Control flow`_).
``grid/``              The grid of cells (``uniform_grid.cpp``, ``uniform_grid_pllel.cpp``),
                       the classes that set up the grid on each level, and the stellar wind
                       sources.
``boundaries/``        Boundary conditions: external boundaries (outflow, periodic, ...),
                       internal boundaries (stellar winds), boundaries between MPI
                       sub-domains and between levels of a nested grid, and the motion of
                       stars (``star_motion.cpp``).
``sub_domain/``        Domain decomposition and MPI communication (see :ref:`ref-dev-mpi`).
``spatial_solvers/``,  The equations (hydrodynamics and MHD), and the calculation of fluxes
``equations/``,        across cell faces with Riemann solvers and other flux solvers.
``Riemann_solvers/``
``coord_sys/``         Vector operations (divergence, gradient, ...) for each coordinate
                       system.
``microphysics/``      Heating, cooling, ionisation and recombination (``MPv*`` classes and
                       the interface to NEMO, ``nemo_interface.cpp``).
``raytracing/``        The short-characteristics ray-tracer for point sources and sources
                       at infinity.
``dataIO/``            Reading and writing snapshots (Silo, FITS, text).
``ics/``               The initial-condition generators of ``icgen``.
``defines/``,          Compile-time flags and utility functions.
``tools/``
``contrib/``           Old code that is not compiled.
=====================  ======================================================================


.. _ref-dev-grid:

Computational Grid
==================================

The PION computational grid is a uniform rectilinear grid in 1, 2 or 3 spatial dimensions.
PION can be run in "uniform-grid" mode or "nested-grid" mode, depending on whether static mesh-refinement is needed.
The nested-grid mode sets up a hierarchy of grids with different levels of refinement, each refined grid fully enclosed by the coarser grid one level up in the hierarchy (see :ref:`nested-grid`).
The uniform-grid mode sets up a single grid and there is no mesh refinement.
Grid cells are all the same size for a given refinement level, and they are squares (2D) or cubes (3D) in the space that is modelled.

Terminology used in this guide:
 * the terms `cell` and `zone` are used interchangeably and mean the same thing.
 * `ghost cells` and `boundary cells` have the same meaning.
 * `static mesh-refinement` and `nested grid` are both used for the mesh-refinement strategy used in PION.
 * The six directions in 3D space are denoted :math:`\{\pm\hat{x},\pm\hat{y},\pm\hat{z}\}`


Grid Geometry
--------------------
There are 5 combinations of coordinate system and dimensionality:

1. Cartesian geometry, 1D simulations: this is a line in the :math:`\pm\hat{x}` direction that represents an infinite slab (gradients of all quantities in the two perpendicular directions are zero).
2. Spherical geometry, 1D simulations: a spherically symmetric configuration is modelled, again with gradients in the angular directions set to zero.  In this case geometric source terms are added to the fluid equations because each cell is a spherical shell and its volume increases with radius.
3. Cartesian geometry, 2D simulations: each point on the 2D plane represents an infinite line in the 3rd dimension (gradients perpendicular to the plane are zero), and the divergence of a quantity is linear with distance rather than quadratic.  This is of limited use for modelling physical systems, but is useful for development work and exploring parameter space.
4. Cylindrical geometry, so-called 2.5D simulations: the :math:`(z,R)` plane is simulated assuming rotational symmetry about the axis :math:`R=0`.  Again a geometric source term arises in the radial direction because each cell is a ring (of square cross section) in 3D space and its volume increases with cylindrical radius.  Rotation and toroidal magnetic fields are possible in this configuration, but gradients in the angular direction are zero.
5. Cartesian geometry, 3D simulations: the 3D domain is represented by a block of cubic cells.


Serial and Parallel grid setup
-------------------------------------

PION can be run in serial mode, in which case there is a single process that calculates everything.
In this case, the grid of each level covers its full domain as a single block of data.
PION can also be run in parallel mode, so that :math:`N_p` MPI processes are started and these calculate in parallel.
On each refinement level the full domain is split into :math:`N_p` sub-domains and each MPI process sets up a grid of cells corresponding only to its sub-domain, plus ghost cells for the boundary data from its neighbours.
Each MPI process has exactly 1 sub-domain on each refinement level.

The depth of the ghost-cell layers depends on the spatial order of accuracy.  With 2nd-order accuracy it is 2 cells on a uniform grid; on a nested grid it is 6 cells at the outer edge of each level and 4 cells at the edges between MPI sub-domains.


Location of source files
-------------------------------------

The source files controlling the grid are located in ``source/grid/``, particularly ``uniform_grid.cpp`` (single-domain mode) and ``uniform_grid_pllel.cpp`` (MPI parallel mode with multiple sub-domains).
Classes for setting up grids are as follows:

+ Uniform grid, single level, single domain: ``setup_fixed_grid.cpp``
+ Uniform grid, single level, MPI-parallel, multiple domains: ``setup_fixed_grid_MPI.cpp``
+ Static mesh-refinement, single domain: ``setup_NG_grid.cpp``
+ Static mesh-refinement, MPI-parallel, multiple domains: ``setup_grid_NG_MPI.cpp``


---------------------------------------------------------------------


.. _ref-dev-control:

Control Flow
==================================

The different versions of PION have slightly different control flow, depending on whether PION is run in serial or parallel mode, and in nested-grid or uniform-grid mode.
The time-integration scheme is based on that of `Falle (1991) <https://scixplorer.org/abs/1991MNRAS.250..581F/abstract>`_ and `Falle et al. (1998) <https://scixplorer.org/abs/1998MNRAS.297..265F/abstract>`_; the uniform-grid implementation is described in some detail in `Mackey (2012) <https://scixplorer.org/abs/2012A%26A...539A.147M/abstract>`_, and the nested-grid implementation in `Mackey et al. (2021) <https://scixplorer.org/abs/2021MNRAS.504..983M/abstract>`_ (section 2.4).
You are encouraged to read these papers to gain some understanding of what each step does.
Here the most complicated routine will be described: the parallel implementation of the nested-grid method.


Outer loop
--------------------

All four versions of ``pion`` (``pion-ug``, ``pion-ng``, serial and parallel) have the same ``main()`` function, in ``source/main.cpp``.
It reads the command-line options, and creates the simulation-control class for the version that is being compiled (chosen with the preprocessor flags ``PION_NESTED`` and ``PARALLEL``):

+ uniform grid, serial mode: ``sim_control`` in ``sim_control/sim_control.cpp``
+ uniform grid, parallel mode: ``sim_control_pllel`` in ``sim_control/sim_control_MPI.cpp``
+ nested grid, serial mode: ``sim_control_NG`` in ``sim_control/sim_control_NG.cpp``
+ nested grid, parallel mode: ``sim_control_NG_MPI`` in ``sim_control/sim_control_NG_MPI.cpp``

It then calls the ``Init()``, ``Time_Int()`` and ``Finalise()`` functions of this class.
Everything in ``Init()`` and ``Finalise()`` is only called once per simulation run and so efficiency is not crucial here.
The function ``Time_Int()`` is responsible for integrating the initial conditions forward in time to a final state after a (usually large) number of timesteps.
It is responsible for

1. setting the timestep for the next step;
2. saving snapshots periodically according to the chosen criteria;
3. stepping forward in time repeatedly until
4. the criterion for ending the simulation is reached.

Each timestep is taken by the function ``advance_time()``.

+ The uniform-grid version of this (serial and parallel) is found in ``sim_control/time_integrator.cpp``, and calls ``first_order_update()`` and ``second_order_update()``.  These refer to the order of accuracy of the integration scheme: the 1st-order scheme is basically `forward Euler integration <https://en.wikipedia.org/wiki/Euler_method>`_ with piecewise-constant data, whereas the 2nd-order scheme involves two sub-steps to achieve better accuracy.  Initially a 1st-order step is taken for half of the requested timestep, to obtain a time-centred intermediate state.  A piecewise-linear interpolation of the data is then applied to this state, and the fluxes across cell boundaries are calculated.  These fluxes are used to step the initial state forward by the full timestep (i.e. the intermediate state is discarded once the fluxes have been calculated).  This step is accurate to 2nd order in space and time (see `Falle et al. (1998) <https://scixplorer.org/abs/1998MNRAS.297..265F/abstract>`_).
+ The nested-grid version (serial and parallel) is found in ``sim_control/sim_control_NG.cpp`` and ``sim_control/sim_control_NG_MPI.cpp``, and calls either ``advance_step_OA1()`` for a 1st-order step or ``advance_step_OA2()`` for a 2nd-order step.

Note that the 1st-order scheme is only used for testing and development purposes, and all scientific applications should use the 2nd-order scheme because it is always more accurate for a given computational expense.


2nd-order timestep for nested-grid, parallel run
----------------------------------------------------

The most complicated (and most useful) timestepping routine in PION is ``advance_step_OA2()`` for the nested-grid, parallel execution, found in ``sim_control/sim_control_NG_MPI.cpp``.
This is a recursive function, first called on the coarsest grid level 0, which then calls itself repeatedly until the finest level is reached.
In this way, calling the function a single time on level 0 will advance all levels by one level-0 step (2 steps on level 1, 4 steps on level 2, :math:`2^n` steps on level :math:`n`).

+ The first task is to ensure that data on all levels are consistent:
    + Update the external boundaries: ``TimeUpdateExternalBCs()``.  This includes the boundary data between sub-domains of different MPI processes on this level, and receiving the data for the outer boundary of this level from the coarser level (``COARSE_TO_FINE_RECV`` boundaries).  This usually involves MPI communication.
    + Send boundary data from this level to the outer boundary of the next finer level, if required: ``BC_update_COARSE_TO_FINE_SEND()``.  This also usually involves MPI communication, if the destination grid is on another MPI process.

+ Next take a timestep on the next finer level, if it exists (i.e. call ``advance_step_OA2()`` on the next finer level).

+ Then calculate the 1st-order half-step on this level (none of this involves MPI communication):
    + Calculate changes associated with radiative heating and cooling, and chemical kinetics: ``calc_microphysics_dU()``
    + Calculate changes from (magneto-)hydrodynamical fluxes across cell boundaries: ``calc_dynamics_dU()``
    + Calculate the half-step intermediate state based on the above changes: ``grid_update_state_vector()``

+ Now update various quantities to the intermediate half-step values:
    + Update internal boundaries (i.e. stellar winds, and the positions of moving stars): ``TimeUpdateInternalBCs()``
    + Receive data from finer levels to replace calculated data (assuming the finer level is more accurate): ``BC_update_FINE_TO_COARSE_RECV()``.  This generally involves MPI communication.
    + Update external boundaries, including boundary data between sub-domains: ``TimeUpdateExternalBCs()``.  This generally involves MPI communication.
    + Update properties of radiation sources (if present) and do the ray-tracing on this grid: ``update_evolving_RT_sources()`` and ``do_ongrid_raytracing()``.  This generally involves MPI communication.

+ Calculate the full-step 2nd-order update, again microphysics and dynamics, but this time make some corrections before finalising the update:
    + If not the coarsest level, save fluxes crossing the level boundary: ``save_fine_fluxes()``
    + If not the finest level, save fluxes crossing cell boundaries corresponding to the level boundary of the next finer level: ``save_coarse_fluxes()``
    + If this level has data for the outer boundary of the finer level, send the data: ``BC_update_COARSE_TO_FINE_SEND()``.  This generally involves MPI communication.

+ Calculate a second step on the finer level by recursively calling ``advance_step_OA2()``
+ Receive flux values from the level boundary of the finer level, and correct fluxes on this level accordingly: ``recv_BC89_fluxes_F2C()``.  This generally involves MPI communication.
+ Update the state on this level for the full step: ``grid_update_state_vector()``, and advance the current time.
+ Update internal boundaries (i.e. stellar winds): ``TimeUpdateInternalBCs()``
+ Replace data with values averaged from the finer level: ``BC_update_FINE_TO_COARSE_RECV()``.  This generally involves MPI communication.
+ Update properties of radiation sources (if present) to the end of the timestep and do the ray-tracing on this grid: ``update_evolving_RT_sources()`` and ``do_ongrid_raytracing()``, and exchange column densities with the other levels for sources outside a level: ``RT_sync_levels()``.  This generally involves MPI communication.
+ Send fluxes crossing the level boundary to the next coarser level (for correcting fluxes at that level): ``send_BC89_fluxes_F2C()``.  This generally involves MPI communication.
+ Send level data to the coarser level, to replace what was calculated there: ``BC_update_FINE_TO_COARSE_SEND()``.  This generally involves MPI communication.
+ Return so that the next step can take place.

Without the nested grid levels, the steps are much simpler and the functions for exchanging data and fluxes between levels are not needed.
The only functions requiring MPI communication are then ``TimeUpdateExternalBCs()`` and ``do_ongrid_raytracing()``.





.. include:: dev_mpi.inc
