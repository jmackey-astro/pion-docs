.. _ref-dev-winds:

Stellar winds
================================

Stellar winds are implemented as an internal boundary condition: in a sphere around each wind source, the gas in the grid cells is reset every timestep to the state of a freely flowing wind, and the hydrodynamics then carries this gas out into the rest of the grid.
The parameters of the wind sources are described in :ref:`parameter-winds`.


Source files and classes
------------------------

+ ``grid/stellar_wind_BC.cpp``: the class ``stellar_wind``, for winds that are constant in time (``WIND_[i]_type 0``), and the derived class ``stellar_wind_evolution``, for winds whose properties are interpolated from a table in a file (type 1).
+ ``grid/stellar_wind_angle.cpp``: the class ``stellar_wind_angle`` (derived from ``stellar_wind_evolution``), for latitude-dependent winds of rotating stars, following Langer et al. (1999) (type 2).
+ ``grid/stellar_wind_latdep.cpp``: the class ``stellar_wind_latdep``, a modified version of this in which the polar wind is not enhanced by rotation (type 3).
+ ``boundaries/stellar_wind_boundaries.cpp``: the class ``stellar_wind_bc``, which connects the wind classes to the boundary-condition machinery (boundary type ``STWIND``), and moves the wind regions with moving stars.
+ ``boundaries/star_motion.cpp``: the motion of the stars that wind sources can be attached to.
+ ``sim_params.cpp`` reads the ``WIND_*`` parameters into the global list ``SWP`` of ``stellarwind_params`` structs, and ``dataio_base.cpp`` writes them to, and reads them from, the snapshot headers.

Each grid (each level of a nested grid, on each MPI process) has its own wind object, ``grid->Wind``, with a list of the sources and, for each source, a list of the grid cells in its wind region (``struct wind_source`` and ``struct wind_cell``).
Only one type of wind class is used in a simulation: ``BC_assign_STWIND()`` creates the most general class that any of the sources needs (type 3 > 2 > 1 > 0).


Setting up the wind region
--------------------------

The wind boundary is requested in the parameter file with ``BC_Ninternal 1`` and ``BC_INTERNAL_000 stellar-wind``, which the grid setup turns into a boundary of type ``STWIND``.
``BC_assign_STWIND()`` is called once for each grid, when the boundaries are set up.
It creates the wind class, adds the sources to it (``add_source()``, ``add_evolving_source()`` or ``add_rotating_source()``, depending on the type), and then adds the grid cells of each source with ``BC_assign_STWIND_add_cells2src()``: every cell of the grid whose centre is within the radius of the wind region is added with ``stellar_wind::add_cell()``.
On a parallel run each process only adds the cells of its own sub-domain.

The radius of the wind region (``current_radius``) is set by ``BC_set_wind_radius()``, starting from ``WIND_[i]_radius``, and is increased if it is too small:

+ it must be at least ``MIN_WIND_RAD`` cells (7, set in ``sim_constants.h``) of the coarsest level that the wind region overlaps, so that the wind is resolved where it leaves the region;
+ it must be at least 2 cells larger than the stellar radius, and for MHD winds larger than the Alfvén radius (``get_min_wind_radius()``).

For each cell, ``add_cell()`` calculates a reference state, which is stored with the cell and copied into it at every update:

+ For a wind at its terminal velocity (``set_wind_cell_reference_state_vinf()``), the density is :math:`\rho = \dot{M}/(4\pi r^2 v_\infty)` (or :math:`\dot{M}/(2\pi r v_\infty)` in 2D Cartesian slab symmetry), the velocity is radial with speed :math:`v_\infty`, and the pressure follows an adiabat from the stellar surface, where the temperature is ``WIND_[i]_Tstar``, with a minimum temperature for the wind.  With MHD, the magnetic field is a split monopole with a toroidal component from the rotation of the star (``WIND_[i]_Bsrf``, ``WIND_[i]_vrot``).
+ For an accelerating wind (``WIND_[i]_acceleration 1``, ``set_wind_cell_reference_state_acc()``), the gas is injected with 4 times the isothermal sound speed at the stellar surface, and the velocity follows a :math:`\beta=1` law, :math:`v(r) = v_0 + (v_\infty - v_0)(1-R_\star/r)`.  Outside the wind region the wind is then accelerated by an approximate radiative force (``BC_set_windacc_radflux()``), which is stored in the extra data of each cell within 100 stellar radii and applied as a source term in the hydrodynamics.  This needs ``EP_wind_acceleration 1``.
+ Cells inside the star (:math:`r < R_\star`) are given fixed values (``set_stellar_interior_values()``).
+ The tracers are set to ``WIND_[i]_TR[n]`` (or, for evolving winds, the element mass fractions from the file), and the star's space velocity is added if it moves.


Updating the wind every timestep
--------------------------------

``BC_update_STWIND()`` is called by ``TimeUpdateInternalBCs()`` for each level, at the half step and at the full step of each timestep (see `Control Flow`_).
For each source it calls ``set_cell_values()``, which copies the reference state into every cell of the wind region.

For an evolving wind (``stellar_wind_evolution``), ``set_cell_values()`` first checks whether it is time to update the source (every ``WIND_[i]_updatefreq``).
If so, ``update_source()`` interpolates the stellar properties (:math:`\dot{M}`, :math:`v_\infty`, :math:`T_\mathrm{eff}`, :math:`R_\star`, element abundances) from the table at the current time, and recalculates the reference state of every cell.
The table is read from ``WIND_[i]_evofile`` by ``read_evolution_file()``.  It has two header lines, and then one line per time with the columns

.. code-block:: none

  time  mass  luminosity  T_eff  Mdot  v_rot  v_crit  v_inf  X_H  X_He  X_C  X_N  X_O  X_Z  X_D

all in CGS units.  The times are shifted by ``WIND_[i]_t_offset`` and divided by ``WIND_[i]_t_scalefac``, so that a stellar model can be started at any age and run faster than real time.
A source only switches on when the simulation reaches the first time in the table, and when the simulation passes the last time (the end of the star's life), ``set_cell_values()`` returns -1 and PION finishes the simulation.

For rotating stars (types 2 and 3) the density and velocity also depend on the polar angle, through ``WIND_[i]_xi`` and the ratio of the rotation velocity to the critical velocity; ``WIND_[i]_enhance_mdot`` can increase the mass-loss rate of a type-3 wind close to critical rotation.


Moving stars
------------

A wind source can be attached to a star (``WIND_[i]_star``, see :ref:`parameter-stars`), which is moved by the N-body integrator in ``star_motion.cpp``, called from ``TimeUpdateInternalBCs()`` and driven by the timestep of the finest level.
Every MPI process does the same calculation, so all processes agree on the positions of the stars.

When the star has moved by more than 0.1 times the cell size of the finest level, ``BC_update_STWIND()`` removes all cells from the wind source (``remove_cells()``), recalculates the radius of the wind region (which can change if the region now overlaps a different level), and adds the cells around the new position with ``BC_assign_STWIND_add_cells2src()``.
The velocity of the star is added to the wind velocity in each cell.


Winds on nested grids and in parallel
-------------------------------------

+ Each level has its own wind object, and sets the wind in its own cells.  A wind region that crosses the boundary of a refined level is set on both levels; the coarse cells under the fine level are then replaced by averages of the fine cells as usual (see :ref:`ref-dev-ng-boundaries`).
+ Because the radius of the wind region depends on the coarsest level it overlaps, the same source has the same wind radius on every level.
+ ``add_cell()`` flags the wind cells as boundary data (``isbd = true``, ``isdomain = false``), because their state is set by the wind class rather than by the solver.  They are still used in the timestep calculation (``timestep = true``), except for cells inside the star.
+ On a parallel run each process only has the wind cells of its own sub-domain.  A wind region on the boundary between sub-domains, or at the edge of a refined level, is a common place for bugs that depend on the number of processes: the tests ``windrt1d-ng`` and ``windrt2d-ng`` check this (see :ref:`tests`).
