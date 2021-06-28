.. _dev-guide:

*********************************
Developers Guide for PION
*********************************

.. include:: dev_coding.inc



.. _ref-dev-grid:

Computational Grid
==================================

The PION computational grid is a uniform rectilinear grid in 1, 2 or 3 spatial dimensions.
PION can be run in "uniform-grid" mode or "nested-grid" mode, depending on whether static mesh-refinement is needed.
The nested-grid mode sets up a hierarchy of grids with different levels of refinement, each refined grid fully enclosed by the coarser grid one level up in the hierarchy.
The uniform-grid mode sets up a single grid and there is no mesh refinement.
Grid cells are all the same diameter for a given refinement level, and they are squares (2D) or cubes (3D) in the space that is modelled.

Terminology used in this guide:
 * the terms `cell` and `zone` are used interchangeably and mean the same thing.
 * `ghost cells` and `boundary cells` have the same meaning.
 * `static mesh-refinement` and `nested-grid` are both used to denote the mesh-refinement strategy used in PION, sometimes interchangeably.
 * The six directions in 3D space are denoted :math:`\{\pm\hat{x},\pm\hat{y},\pm\hat{z}\}`


Grid Geometry
--------------------
There are 5 options for the combinations of coordinate system and dimensionality that have been implemented:

1. Cartesian geometry, 1D simulations: this is a line in the :math:`\pm\hat{x}` direction that represents an infinite slab (gradients of all quantities in the two perpenicular directions are set to zero).
2. Spherical geometry 1D simulations: a spherically symmetric configuration is modelled, again with gradients in the angular directions set to zero.  In this case geometric source terms are added to the fluid equations because each cell is a spherical shell and its volume increases with radius.
3. Cartesian geometry, 2D simulations: each point on the 2D plane represents an infinite line in the 3rd dimension (gradients perpendicular to the plane are zero), and the divergence of a quantity is linear with distance rather than quadratic.  This is of limited use for modelling physical systems, but is useful for development work and exploring parameter space.
4. Cylindrical geometry, so-called 2.5D simulations: the :math:`(R,z)` plane is simulated assuming rotational symmetry about the axis :math:`R=0`.  Again for cylindrical coordinates a geometric source term arises in the radial direction because each cell is a ring (of square cross section) in 3D space and its volume increases with cylindrical radius.  Rotation and toroidal magnetic fields are possible in this configuration, but gradients in the angular directions are obviously zero.
5. Cartesian geometry, 3D simulations: the 3D domain is represented by a block of cubic cells.


Mesh refinement
-------------------------------------

Static mesh-refinement (i.e., a multiply nested-grid structure) is implemented for the case where each refinement level has the same shape and number of cells as the coarser level above it, but the spatial resolution is a factor of 2 higher and so the refined grid covers 1/2 of the domain of the coarser grid in each dimension.
The focus of the nested grid can be at the centre of the domain, the negative boundary or the positive boundary for each dimension.
The number of refinement levels is in principle unlimited, but snapshots can only be written every coarse timestep and this imposes a practical limit: for 10 grid levels there are 512 finest-level timesteps per coarsest-level step, and one probably wants to save a snapshot at least this often.
The coarsest level is denoted `level 0`, and each refined level has a higher level number.


Serial and Parallel grid setup
-------------------------------------

PION can be run in serial mode, in which case there is a single process that calculates everything (i.e., no parallel execution).
In this case, the grid corresponding to the full domain is set up as a single block of data for each refinement level.
PION can also be run in parallel mode, so that :math:`N_p` MPI processes are started and these calculate in parallel.
On each refinement level the full domain is split into :math:`N_p` sub-domains and each MPI process sets up a grid of cells corresponding only to its sub-domain (plus some boundary data for synchronisation).
Each MPI process has exactly 1 sub-domain on each refinement level.


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

The different versions of PION have slightly different control flow, depending on whether PION is run in single-core or parallel mode, and nested-grid or uniform-grid mode.
The time-integration scheme is based on that of `Falle (1991) <https://ui.adsabs.harvard.edu/abs/1991MNRAS.250..581F/abstract>`_ and `Falle et al. (1998) <https://ui.adsabs.harvard.edu/abs/1998MNRAS.297..265F/abstract>`_; the uniform-grid implementation is described in some detail in `Mackey (2012) <https://ui.adsabs.harvard.edu/abs/2012A%26A...539A.147M/abstract>`_, and the nested-grid implementation in `Mackey et al. (2021) <https://ui.adsabs.harvard.edu/abs/2021MNRAS.tmp..790M/abstract>`_ (section 2.4).
You are encouraged to read these papers to gain some understanding of what each step does.
Here the most complicated routine will be described: the parallel implementation of the nested-grid method.


Outer loop
--------------------

The overall control of PION comes from the ``main()`` function defined in the following files in the ``source`` directory of PION:
+ uniform grid, serial mode: ``main.cpp``
+ uniform grid, parallel mode: ``main_MPI.cpp``
+ nested grid, serial mode: ``main_NG.cpp``
+ nested grid, parallel mode: ``main_NG_MPI.cpp``

This code should rarely need to be updated, however, because it just checks that the command-line options are OK, then in turn calls ``Init()``, ``Time_Int()`` and ``Finalise()`` functions from the relevant simulation-control class.
Everything in ``Init()`` and ``Finalise()`` is only called once per simulation run and so efficiency is not crucial here.
The function ``Time_Int()`` is responsible for integrating the initial conditions forward in time to a final state after a (usually large) number of timesteps.

The versions of ``Time_Int()`` can be found here:

+ uniform grid, serial mode: ``sim_control/sim_control.cpp``
+ uniform grid, parallel mode: ``sim_control/sim_control_MPI.cpp``
+ nested grid, serial mode: ``sim_control/sim_control_NG.cpp``
+ nested grid, parallel mode: ``sim_control/sim_control_NG_MPI.cpp``

This routine is responsible for

1. setting the timestep for the next step;
2. saving snapshots periodically according to the chosen criteria;
3. stepping forward in time repeatedly until
4. the execution is stopped when the criterion is reached for ending the simulation.

Each timestep is taken by the function ``advance_time()``.

+ The uniform-grid version of this (serial and parallel) is found in ``sim_control/time_integrator.cpp``, and is split into a 1st-order step and a 2nd-order step.  This refers to the order of accuracy of the integration scheme: the 1st-order scheme is basically `forward Euler integration <https://en.wikipedia.org/wiki/Euler_method>`_ with piecewise-constant data, whereas the 2nd-order scheme involves two sub-steps to achieve better accuracy.  Initially a 1st-order step is taken for half of the requested timestep, to obtain a time-centred intermediate state.  A piecewise-linear interpolation of the data is then applied to this state, and the fluxes across cell boundaries are calculated.  These fluxes are used to step the initial state forward by the full timestep (i.e. the intermediate state is discarded once the fluxes have been calculated).  This step is accurate to 2nd order in space and time (see `Falle et al. (1998) <https://ui.adsabs.harvard.edu/abs/1998MNRAS.297..265F/abstract>`_).
+ The nested-grid version (serial and parallel) is found in ``sim_control/sim_control_NG.cpp``, and calls either ``advance_step_OA1()`` for a 1st-order step or ``advance_step_OA2()`` for a 2nd-order step.

Note that the 1st-order scheme is only used for testing and development purposes, and all scientific applications should use the 2nd-order scheme because it is always more accurate for a given computational expense.


2nd-order timestep for nested-grid, parallel run
----------------------------------------------------

The most complicated (and most useful) timestepping routine in PION is ``advance_step_OA2()`` for the nested-grid, parallel execution, found in ``sim_control/sim_control_NG_MPI.cpp``.
This is a recursive function, first called on the coarsest grid level 0, which then calls itself repeatedly until the finest level is reached.
In this way, calling the function a single time on level 0 will advance all levels one step on level 0 (2 steps on level 1, 4 steps on level 1, :math:`2^n` steps on level :math:`n`).

+ The first task is to ensure that data on all levels are consistent:
    + Receive any external boundary data from a coarser level, if required: ``BC_update_COARSE_TO_FINE_RECV()``.  This can involve MPI communication if the required data is on another MPI process.
    + Update external boundary conditions, including boundary data between sub-domains on the grid level associated with different MPI processes: ``TimeUpdateExternalBCs()``.  This usually involves MPI communication.
    + Send any boundary data from this level to the outer boundary of the next finer level, if required: ``BC_update_COARSE_TO_FINE_SEND()``.  Also usually involves MPI communication, if the destination grid is on another MPI process.

+ Next take a timestep on the next finer level, if it exists (i.e. call the ``advance_step_OA2()`` function on the next finer level).

+ Then calculate the 1st-order half-step on this level (none of this involves MPI communication):
    + Calculate changes associated with radiative heating and cooling, and chemical kinetics: ``calc_microphysics_dU()``
    + Calculate changes from (magneto-)hydrodynamical fluxes across cell boundaries: ``calc_dynamics_dU()``
    + Calculate changes due to thermal conduction of internal energy (not really working): ``calc_thermal_conduction_dU()``
    + Calculate the half-step intermediate state based on the above changes: ``grid_update_state_vector()``

+ Now update various quantities to the intermediate half-step values:
    + update internal boundaries (i.e. stellar winds): ``TimeUpdateInternalBCs()``
    + Receive data from finer levels to replace calculated data (assuming finer level is more accurate): ``BC_update_FINE_TO_COARSE_RECV()``.  This generally involves MPI communication.
    + Update external boundaries, including boundary data between sub-domains on the grid level associated with different MPI processes: ``TimeUpdateExternalBCs()``.  This generally involves MPI communication.
    + Upate properties of radiation sources (if present) and do raytracing on this grid: ``update_evolving_RT_sources()`` and ``do_ongrid_raytracing()``.  This generally involves MPI communication.

+ Calculate the full-step 2nd-order update, again microphysics, dynamics, (thermal conduction), but this time make some corrections before finalising the update:
    + If not the coarsest level, save fluxes crossing the level boundary ``save_fine_fluxes()``
    + If not the finest level, save fluxes crossing cell boundaries corresponding to the level boundary of the next finer level ``save_coarse_fluxes()``
    + If level has data for outer boundary of finer level, send the data ``BC_update_COARSE_TO_FINE_SEND()``.  This generally involves MPI communication.

+ Calculate a second step on the finer level by recursively calling ``advance_step_OA2()``
+ Receive flux values from level boundary of finer level, and correct fluxes on this level accordingly: ``recv_BC89_fluxes_F2C()``.  This generally involves MPI communication.
+ Update state on this level for the full step: ``grid_update_state_vector()``, and advance the current time.
+ Update internal boundaries (i.e. stellar winds): ``TimeUpdateInternalBCs()``
+ Replace data with value averaged from finer grids: ``BC_update_FINE_TO_COARSE_RECV()``.  This generally involves MPI communication.
+ Update properties of radiation sources (if present) to end of timestep and do raytracing on this grid: ``update_evolving_RT_sources()`` and ``do_ongrid_raytracing()``.  This generally involves MPI communication.
+ Send fluxes crossing level boundary to the next coarser level (for correcting fluxes at that level): ``send_BC89_fluxes_F2C()``.  This generally involves MPI communication.
+ Send level data to coarser level, to replace what was calculated there: ``BC_update_FINE_TO_COARSE_SEND()``.  This generally involves MPI communication.
+ Return so that the next step can take place.

Without the nested grid levels, the steps are much simpler and the following function calls are omitted:
``BC_update_COARSE_TO_FINE_RECV()``, ``BC_update_COARSE_TO_FINE_SEND()``,  ``BC_update_FINE_TO_COARSE_RECV()``, ``save_fine_fluxes()``, ``save_coarse_fluxes()``, ``BC_update_COARSE_TO_FINE_SEND()``, ``recv_BC89_fluxes_F2C()``, ``BC_update_FINE_TO_COARSE_RECV()``, ``send_BC89_fluxes_F2C()``, ``BC_update_FINE_TO_COARSE_SEND()``.
The only functions requiring MPI communication are ``TimeUpdateExternalBCs()`` and ``do_ongrid_raytracing()``.





.. include:: dev_mpi.inc

#.. include:: dev_winds.inc



