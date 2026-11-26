.. _nested-grid:

Nested grids
============

PION can use static mesh-refinement: a set of nested grid levels, each with twice the resolution of the level above it, focused on the region of interest (e.g. around a star).
The grid does not change during the simulation.

Nested-grid simulations use the executables ``icgen-ng`` and ``pion-ng``, which are compiled with ``-DPION_NESTED_GRID=ON`` (see :ref:`compilation`).
These also run simulations with only one level (``grid_nlevels 1``), so they can be used for uniform grids too.


Grid levels
-----------

The coarsest level, level 0, covers the whole simulation domain, given by ``Xmin``, ``Xmax``, etc., with ``NGridX`` × ``NGridY`` × ``NGridZ`` cells.
Each refined level (level 1, 2, ..., ``grid_nlevels`` - 1):

+ has the same number of cells as level 0,
+ has cells half the size of those of the level above it, so level :math:`l` has cell size :math:`\Delta x_l = \Delta x_0 / 2^l`,
+ covers half of the extent of the level above it along each axis,
+ is placed towards the refinement centre ``NG_centre_XX``, ``NG_centre_YY``, ``NG_centre_ZZ``.

The refinement centre must be either at the centre of the domain, or on its lower or upper boundary, along each axis; ``icgen-ng`` and ``pion-ng`` stop with an error otherwise.
Along an axis where the centre is in the middle of the domain, every level is centred on the domain.
Where the centre is on a boundary, every level has one side on that boundary.

Along an axis with periodic boundary conditions, the refinement centre must be at the centre of the domain.
A level that ends at a periodic boundary would need data from the other side of the domain, which is only on a coarser level, and this gives errors along that boundary (in a GLM-MHD test, a large spurious :math:`\psi`).
Because the domain is periodic, you can always shift it so that the region you want to refine is at its centre.
``icgen-ng`` and ``pion-ng`` stop with an error if the refinement centre is on a periodic boundary.

.. versionchanged:: 3.0.0-dev.2
   The check for periodic boundaries; earlier versions ran such simulations.

For example, the 2D cylindrical simulation in :ref:`example-sim` has these grid parameters:

.. code-block:: none

  ndim 2
  coordinates cylindrical
  NGridX  128
  NGridY  128
  Xmin  -4.0e18
  Xmax   1.0e18
  Ymin   0.0
  Ymax   5.0e18
  grid_nlevels 3
  NG_centre_XX 1.0e18
  NG_centre_YY 0.0
  NG_refine_XX 1
  NG_refine_YY 1

The refinement centre is the corner of the domain at :math:`(z,R) = (10^{18}, 0)` cm, so the three levels are:

.. list-table::
   :header-rows: 1

   * - Level
     - :math:`z` range (cm)
     - :math:`R` range (cm)
     - Cell size (cm)
   * - 0
     - :math:`-4.0\times10^{18}` to :math:`1.0\times10^{18}`
     - 0 to :math:`5.0\times10^{18}`
     - :math:`3.9\times10^{16}`
   * - 1
     - :math:`-1.5\times10^{18}` to :math:`1.0\times10^{18}`
     - 0 to :math:`2.5\times10^{18}`
     - :math:`2.0\times10^{16}`
   * - 2
     - :math:`-2.5\times10^{17}` to :math:`1.0\times10^{18}`
     - 0 to :math:`1.25\times10^{18}`
     - :math:`9.8\times10^{15}`

The parameters ``NG_refine_XX``, ``NG_refine_YY`` and ``NG_refine_ZZ`` (default 1) say whether the levels are refined along each axis.
With ``NG_refine_XX 0``, each level instead keeps the full extent of the domain along X, with twice as many cells as the level above it.
This option is not used by any of the PION tests, so check the results carefully if you use it.

All of the grid parameters are listed in :ref:`parameter-file`.


Choosing the number of levels
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each level takes two timesteps for every timestep of the level above it, so in one level-0 timestep, level :math:`l` takes :math:`2^l` timesteps.
With :math:`N` levels the finest level uses about half of the computing time, and a simulation costs about :math:`2^N` times as much as one with level 0 only.
A uniform grid with the resolution of the finest level everywhere would cost about :math:`2^{(N-1)\,\mathrm{ndim}}/2` times as much as the nested grid.

The number of levels is in principle unlimited, but snapshots are only written after a level-0 timestep (see `Output and restarts`_), and a level-0 timestep is :math:`2^{N-1}` finest-level timesteps.
In practice a few to about 8 levels are used.


Time integration
----------------

Each level has its own timestep: level :math:`l` takes two steps of size :math:`\Delta t_{l+1}` on level :math:`l+1` for each of its own steps of size :math:`\Delta t_l = 2\Delta t_{l+1}` (sub-cycling).
At the start of every level-0 step, PION calculates the timestep limit (the CFL condition, and the microphysics limit if there is one) on every level, and sets the timesteps so that no level exceeds its limit.
The first timestep of a simulation is reduced by a factor of about 3.

The timestep counter in the log files and in the snapshot names counts timesteps of the finest level.


Boundaries between levels
-------------------------

Where the edge of a refined level is inside the simulation domain, the level gets its boundary data from the coarser level:

+ **Coarse-to-fine**: the ghost cells around the fine level are filled by interpolating the coarse-level data, with a linear reconstruction that conserves mass, momentum and energy.
  This is done once per coarse-level step; the ghost cells are then updated by the fine level's hydrodynamics and microphysics, like the other cells, for the two fine-level steps.

  .. versionchanged:: 3.0.0-dev.2
     The interpolation conserves mass, momentum and energy exactly.
     Earlier versions corrected only part of the difference between the coarse cell and the fine cells (1/2, 1/4 or 1/8 of it in 1D, 2D or 3D), so nested-grid results differ slightly from those of earlier versions.

+ **Fine-to-coarse**: after the fine level has taken its two steps, every coarse cell that is covered by the fine level is replaced by the volume-weighted average of the fine cells inside it.
+ **Flux correction**: the fluxes across the boundary of a fine level are used to correct the fluxes of the neighbouring coarse cells (Berger & Colella 1989), so that the total mass, momentum and energy are conserved.

Where the edge of a refined level is on the edge of the simulation domain, the level uses the boundary conditions of the domain (``BC_XN``, etc.).
Internal boundaries such as stellar winds can cross level boundaries; each level sets the wind in its own cells.


Radiation on nested grids
-------------------------

Each level is ray-traced separately, at its own resolution.

+ A point source inside the domain of a level is ray-traced on that level from the source position, which is moved to the nearest cell vertex of that level.
+ For a point source outside a refined level (but inside level 0), the column densities at the edge of the refined level are taken from the coarser level, and the rays continue through the refined level from there.
  The coarse and fine levels exchange column densities whenever they are at the same time.
+ Point sources outside level 0, and sources at infinity, cannot be used with nested grids.

Radiation sources and stellar winds can be attached to stars that move (see :ref:`parameter-stars`); a moving source then jumps from cell vertex to cell vertex on each level.


Running in parallel
-------------------

With MPI, each level is divided between the MPI processes in the same way as level 0, and each sub-domain of a refined level must lie inside the sub-domain of a single process on the coarser level.
This puts conditions on the number of grid cells and the number of processes, which are described in :ref:`parallel`.


Output and restarts
-------------------

+ Each snapshot is written as one file per level (or several per level, in large parallel runs): ``<OutputFile>_level00_0000.<step>.silo``, ``<OutputFile>_level01_0000.<step>.silo``, etc.
+ Each file has an extra variable ``NG_Mask``: 1 for cells that are not covered by a finer level, and 0 for cells that are (their values are averages of the finer level's cells).  Use it to avoid counting the same region twice when you add up quantities (e.g. mass or energy) over all levels.
+ Snapshots are only written after a level-0 timestep.  With ``OutputCriterion 0``, make ``OutputFrequency`` a multiple of :math:`2^{N-1}` (the number of finest-level steps per level-0 step); otherwise some snapshots are skipped.
+ To start or restart a simulation, give ``pion-ng`` the level-0 file, e.g. ``pion-ng MyRun_level00_0000.00000000.silo``.  It reads the files of the other levels from the same directory.
+ PyPion reads all levels of a snapshot together (see :ref:`python_usage`), and VisIt can open the files of each level.


.. _nested-grid-change-levels:

Changing the number of levels on a restart
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The command-line option ``nlevels=N`` (see :ref:`command-line`) restarts a simulation with a different number of levels than the snapshot has.
The new number of levels is written to the snapshots, so later restarts keep it.

+ **Fewer levels:** ``pion-ng`` reads only the coarsest :math:`N` levels, and the finer levels in the snapshot are left out.
  The cells of level :math:`N-1` that were covered by the finer levels keep their values, which are averages of the finer levels.
+ **One more level:** with a snapshot of :math:`N` levels and ``nlevels=N+1``, ``pion-ng`` adds a new finest level :math:`N`, with half the cell size of level :math:`N-1`, placed as described in `Grid levels`_.
  Its cells are interpolated from level :math:`N-1` in the same way as the coarse-to-fine boundaries (see `Boundaries between levels`_), so the new level has exactly the mass, momentum and energy of the cells of level :math:`N-1` that it covers.
  Only one level can be added at a time; to add more, restart again from a later snapshot.

For example, to restart a simulation with 2 levels from its snapshot at step 90 with a third level:

.. code-block:: bash

  mpirun -np 4 pion-ng BW2D_level00_0000.00000090.silo nlevels=3 omp-nthreads=2

The log then has the warning ``Adding refined level 2 to the snapshot's 2 levels, interpolated from level 1; timestep counter 90 -> 92``, and the next snapshots have three files, ``BW2D_level00_0000.<step>.silo`` to ``BW2D_level02_0000.<step>.silo``.

Things to know when adding a level:

+ The interpolated data have no structure on scales smaller than the cells of level :math:`N-1`, and the linear interpolation has errors at sharp features such as shocks and contact discontinuities.
  These produce small waves when the simulation starts again, which travel away and decay; the small-scale structure that the new level can resolve develops over time.
+ The timestep counter counts timesteps of the finest level, and must be a multiple of :math:`2^{N}` with :math:`N+1` levels, so that all levels are at the same time.
  ``pion-ng`` rounds it up to the next multiple of :math:`2^{N}`, so the step numbers in the snapshot names can jump by a few steps at the restart.
+ The finest level takes the smallest timesteps, so with one more level a level-0 step needs twice as many finest-level steps, and the simulation costs about twice as much per unit of simulation time (see `Choosing the number of levels`_).
+ With MPI, the new level must meet the same conditions on the numbers of cells and processes as the other levels (see :ref:`parallel`); ``pion-ng`` stops with an error otherwise.

.. versionadded:: 3.0.0-dev.2
   Adding a level on a restart.  Earlier versions could only restart with fewer levels.
