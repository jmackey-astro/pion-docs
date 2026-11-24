.. _command-line:

***********************************
Command-line options
***********************************

PION has two programs for each grid type (see :ref:`compile-scripts-outcomes`):
the initial-conditions generator (``icgen-ug`` or ``icgen-ng``), which reads a parameter file and writes the initial snapshot, and the simulation code (``pion-ug`` or ``pion-ng``), which reads a snapshot and runs the simulation.
This page lists the command-line options of both programs.

All optional arguments have the form ``name=value``, with no spaces around the ``=``.
They can be given in any order after the first argument.


.. _command-line-icgen:

Initial-conditions generator
-----------------------------------

.. code-block:: bash

  $ icgen-ng <parameter-file> [options]

``<parameter-file>`` (required) is the text parameter file (see :ref:`parameter-file`).
The snapshot is written to the file named by the ``OutputFile`` parameter.
For example:

.. code-block:: bash

  $ mpirun -np 4 icgen-ng params.txt omp-nthreads=4 redirect=icgen_log

The options are:

=========================  ==============================================================================
Option                     Effect
=========================  ==============================================================================
``optype=TYPE``            File type of the initial snapshot: ``silo`` or ``fits``.  If not given,
                           ``icgen`` writes ``fits`` if ``OutputFileType`` in the parameter file is
                           ``fits`` or ``both``, and ``silo`` otherwise.  FITS needs a uniform grid and
                           PION compiled with ``-DPION_USE_FITS=ON``; if it is not available and was not
                           requested with ``optype``, ``icgen`` writes Silo instead, with a warning.
``omp-nthreads=N``         Number of OpenMP threads per MPI process (OpenMP builds only).  The default
                           is 1 thread, and N is limited to the number of cores.
``redirect=PATH``          Write the log to the file ``PATH.log`` instead of the screen
                           (``PATH_<rank>.log`` for MPI builds; in ``Release`` builds only MPI rank 0
                           writes a log).
=========================  ==============================================================================

``icgen`` warns about, and otherwise ignores, any other arguments.

.. note::

   Older versions of PION took the file type as an optional second argument, e.g. ``icgen-ng params.txt silo omp-nthreads=4``, and the options only worked after it.
   This still works, with a warning that it is deprecated.


.. _command-line-pion:

Simulation code
-----------------------------------

.. code-block:: bash

  $ pion-ng <snapshot> [options]

``<snapshot>`` (required) is the snapshot to start from: the initial snapshot written by ``icgen``, or any later snapshot to restart a simulation.
For nested grids, give the level-0 file (``..._level00_...``); the other levels are read automatically.
The file type is found from the file name extension (``.silo`` or ``.fits``).

For example, to run on 4 MPI processes with 8 OpenMP threads each, writing the log to files:

.. code-block:: bash

  $ mpirun -np 4 pion-ng Wind2D_HD_l3n0128_level00_0000.00000000.silo omp-nthreads=8 redirect=log/wind2d

PION stops with an error if it does not recognise an option.
Options that change a parameter override the value stored in the snapshot (which came from the parameter file), and the new value is saved in the snapshots that this run writes.

Running and logging
^^^^^^^^^^^^^^^^^^^

=========================  ==============================================================================
Option                     Effect
=========================  ==============================================================================
``omp-nthreads=N``         Number of OpenMP threads per MPI process (OpenMP builds only).  The default
                           is 1 thread, *not* the value of ``OMP_NUM_THREADS``, and N is limited to the
                           number of threads that OpenMP allows (cores, or ``OMP_NUM_THREADS`` if set).
``redirect=PATH``          Write the log to files instead of the screen, see :ref:`command-line-logs`.
``maxwalltime=H``          Stop the simulation after H hours of run time, writing a final snapshot that
                           can be used to restart the run.  Useful for batch jobs with a time limit:
                           set H a bit below the limit.  MPI builds only; serial builds ignore it.
=========================  ==============================================================================

Output and time
^^^^^^^^^^^^^^^

=========================  ==============================================================================
Option                     Effect
=========================  ==============================================================================
``finishtime=T``           Simulation time (in seconds) at which to stop.
``opfreq=N``               Write a snapshot every N timesteps.  Also sets
                           ``op_criterion=0``.
``opfreq_time=T``          Write a snapshot every T seconds of simulation time.  Also sets
                           ``op_criterion=1``.
``op_criterion=N``         0: write snapshots every ``opfreq`` timesteps; 1: every ``opfreq_time``
                           seconds.
``outfile=PATH``           Path and base name of the snapshot files, replacing ``OutputFile`` from the
                           parameter file.  The directory must exist.
``optype=TYPE``            Snapshot format: ``silo`` (or 5), ``text`` (1), ``fits`` (2), ``txtfits``
                           (4), or ``txtsilo`` (6).  Silo and FITS need PION to be compiled with the
                           matching library.
``simulation_time=T``      Reset the simulation time to T seconds.
=========================  ==============================================================================

Numerical method
^^^^^^^^^^^^^^^^

=========================  ==============================================================================
Option                     Effect
=========================  ==============================================================================
``cfl=C``                  CFL number, in the range 0 to 1.
``ooa=N``                  Order of accuracy in space and time: 1 or 2.
``solver=N``               Flux solver: 0 = Lax-Friedrichs, 1 = linear Riemann solver, 2 = exact Riemann
                           solver (HD), 3 = hybrid of 1 and 2 (HD), 4 = Roe solver in conserved
                           variables, 5 = Roe solver in primitive variables (HD), 6 = flux-vector
                           splitting (HD), 7 = HLLD (MHD), 8 = HLL, 9 = hybrid hydrodynamics solver.
``AVtype=N``               Artificial viscosity: 0 = none, 1 = Falle, Komissarov & Joarder (1998),
                           3 = H-correction of Sanders et al. (1998), 4 = both 1 and 3.  This also
                           sets the viscosity parameter to 0.1.
``EtaVisc=D``              Viscosity parameter (:math:`\geq0`) for ``AVtype=1``.  Give it after
                           ``AVtype``, which resets it.
``gamma=G``                Adiabatic index of the gas (:math:`>1`).
``coordsys=NAME``          Coordinate system: ``cartesian``, ``cylindrical`` or ``spherical``.  Only for
                           testing: the grid and data in the snapshot are not changed.
``nlevels=N``              Number of nested-grid levels.
=========================  ==============================================================================

Physics
^^^^^^^

.. warning::

   Changing these parameters in the middle of a simulation (e.g. when restarting from a snapshot) can have dramatic consequences, including unphysical behaviour.
   Use them only for testing, and never during a production run.

=========================  ==============================================================================
Option                     Effect
=========================  ==============================================================================
``dynamics=0|1``           Switch the hydrodynamics on (1) or off (0), e.g. to evolve only the
                           microphysics and radiation.
``raytracing=0|1``         Switch ray-tracing of radiation sources on or off.
``chemistry=0|1``          Switch the microphysics (chemistry) module on or off.
``microphysics=NAME``      Microphysics module, as for the ``chem_code`` parameter, e.g. ``MPv3`` or
                           ``NEMO`` (see :ref:`ref-microphysics`).
``cooling=N``              Cooling function: 0 for no cooling, or one of the cases described for
                           ``mp_only_cooling`` in :ref:`ref-microphysics`.
``limit_timestep=N``       How the microphysics limits the timestep (``EP_MP_timestep_limit``, 0 to 5; with NEMO 0, 1 or 5).
``min_timestep=T``         Minimum timestep in seconds.
``min_T=T``                Minimum gas temperature in K.
``max_T=T``                Maximum gas temperature in K.
``tc_strength=F``          Strength of thermal conduction, from 0 (none) to 1 (full).
``EP_update_erg=0|1``      Whether heating and cooling change the gas energy (1, the default) or not (0).
``wind_radius_N=R``        Radius in cm of the wind-injection region of stellar-wind source N (0 to 9),
                           e.g. ``wind_radius_0=1.2e17``.
=========================  ==============================================================================

``noise=F`` is also accepted, but has no effect in ``pion``.
Random noise in the initial conditions is set in the parameter file, for the initial-conditions setups that support it.


.. _command-line-logs:

Log files
-----------------------------------

By default PION writes its log to the screen.
With ``redirect=PATH`` it writes to files instead:

+ Serial builds write ``PATH.log``.
+ MPI builds write one file per MPI process, ``PATH_<rank>.log``.  In ``Release`` builds only MPI rank 0 writes a log (``PATH_0.log``), unless PION was compiled with ``-DLOG_ALL_PROCS=ON``.
+ Builds with NEMO also write the messages of the CVODE solver to ``PATH_<rank>_sundials.log``, for every MPI process.
+ A log file is renamed and a new one started when it reaches 5 MB, and up to 20 old files are kept.
+ With OpenMP, information messages come only from thread 0, but warnings and errors come from all threads.

Information messages are buffered, and only written to the file when the buffer is full, when an error occurs, or when PION finishes normally.
So if a run is stopped from outside (e.g. it reaches the time limit of a batch job), the log may be incomplete; use ``maxwalltime`` to let PION stop cleanly.


.. _command-line-restart:

Restarting a simulation
-----------------------------------

To continue a simulation, run ``pion`` with the last snapshot as the first argument (for nested grids, the level-0 file).
Snapshots contain all the parameters of the simulation, so no parameter file is needed.
Use the options above to change e.g. the finishing time or the output frequency, for example:

.. code-block:: bash

  $ mpirun -np 4 pion-ng Wind2D_HD_l3n0128_level00_0000.00009216.silo finishtime=1.0e13 opfreq_time=1.0e11 maxwalltime=23.5
