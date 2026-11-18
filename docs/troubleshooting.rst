.. _troubleshooting:

Troubleshooting
===============

This page lists some common problems when running PION, and what to do about them.
If your problem is not here, see :ref:`getting-help`.

First check that PION itself works on your system by running the tests (see :ref:`tests`): if they pass, the problem is probably in the setup of the simulation.


Problems at startup
-------------------

**The run crashes at once with** ``MPI_ERR_COMM: invalid communicator``

  The number of MPI processes must be a power of 2 (1, 2, 4, 8, ...).
  PION does not check this, and with e.g. 6 processes some of them get no sub-domain, so MPI stops with this error.
  See :ref:`parallel`.

**Nested grid: "NG_centre = ... along axis ... is not allowed"**

  The refinement centre (``NG_centre_XX``, etc.) must be at the centre of the domain or on its lower or upper boundary, along each refined axis.
  See :ref:`nested-grid`.

**Nested grid: "level ... has ... cells along axis ... The number of cells per process must be a whole, even number"**

  With MPI, the number of cells of each level along each axis, divided by the number of processes along that axis, must be a whole, even number.
  Change the number of grid cells or the number of MPI processes.
  Remember that PION chooses the number of processes along each axis from the shape of the domain (see :ref:`parallel`).

**"Don't know what these equations are"**

  The equations are ``eqn euler`` (or ``hd``), ``eqn glm-mhd`` or ``eqn i-mhd`` (see :ref:`parameter-file`).
  ``eqn mhd`` was removed, because it meant GLM-MHD rather than ideal MHD; use ``eqn glm-mhd``.

**"Bad InitIons value"**

  ``InitIons`` can only be ``YES`` (integrate the ion fractions to equilibrium in ``icgen``) or ``LEAVE`` (the default).
  The old option ``NO`` was removed; use ``LEAVE``.

**Many warnings "couldn't find parameter: ... Returning empty string"**

  These are normal: PION looks for every optional parameter, and uses the default value of any that are not in the parameter file.
  Check the list for parameters that you meant to set, in case of a spelling mistake.

**NEMO stops with an error about the parameters**

  + The element mass fractions must add up to exactly 1, and the ion fractions of each element must not add up to more than the element's mass fraction.
  + Photoionisation (``EP_phot_ionisation 1``) needs ray-tracing (``EP_raytracing 1``), and the radiation sources must be point sources.
  + Charge exchange and cosmic-ray ionisation need collisional ionisation (``EP_coll_ionisation 1``).

  See :ref:`ref-nemo`.


Problems during a run
---------------------

**"negative density in leaf grid cell, bugging out."**

  A cell got a negative density in a hydrodynamics update.
  PION first resets the density and writes a warning ("Reset -ve density"), and only stops if this happens in a cell of the domain that is not covered by a finer level.
  This usually means that the timestep is too large for the flow, e.g. at a strong shock, a strong rarefaction, or a sudden change in the initial conditions.
  Try a smaller ``CFL``, check that you use the recommended solver (``solver 9`` for the Euler equations, ``solver 7`` for MHD) and artificial viscosity, and check the initial conditions near the cell (it is printed in the log).

**The run is very slow**

  + Without ``omp-nthreads=N`` on the command line, PION uses **one** OpenMP thread per MPI process, whatever ``OMP_NUM_THREADS`` is set to (see :ref:`command-line`).
  + Check that MPI processes and threads are bound to cores, and that the threads of each process are on one socket (see :ref:`parallel`).
  + Microphysics with many species (e.g. NEMO) can take most of the run time, and the timestep can be limited by the microphysics (``EP_MP_timestep_limit``) rather than by the hydrodynamics.  The log files show the timestep of each step.

**NEMO stops with a CVODE error**

  The integration of the rate equations in a cell failed, and NEMO stops the program itself.
  This can happen if the gas state in a cell is unphysical (e.g. after a hydrodynamics problem in that cell), or if the tolerances are too strict; see the ``EP_nemo_*`` parameters in :ref:`ref-nemo`.


Log files
---------

**The log files are short or empty**

  Information messages are kept in a buffer, and only written to the log file when the buffer is full, when there is an error, or when PION finishes.
  If a run is killed from outside (e.g. it reaches the time limit of a batch job), the last messages are lost.
  Use ``maxwalltime=H`` (MPI builds) so that PION stops by itself a little before the time limit, and writes a final snapshot (see :ref:`command-line`).

**Only one MPI process writes a log**

  In ``Release`` builds only MPI rank 0 writes log messages.
  To see the messages of all processes, e.g. to find which process has a problem, compile PION with ``-DLOG_ALL_PROCS=ON`` and run with ``redirect=PATH``, which writes one file per process (``PATH_<rank>.log``).
  See :ref:`command-line-logs`.


Results that look wrong
-----------------------

**Results depend on the number of MPI processes or threads**

  The results should not depend on the number of MPI processes or OpenMP threads, to round-off error (and, for some microphysics, the tolerance of the integration).
  Bugs in boundary conditions and MPI communication often appear only with some numbers of processes, e.g. where a sub-domain boundary meets a refined level, a wind source, or a radiation source.
  If a result looks suspicious, run the same simulation (or a smaller version of it) with different numbers of processes, e.g. 1, 4 and 16, and with 1 and several threads, and compare the snapshots, e.g. with ``silocompare`` (see :ref:`tests`).
  If the results differ, please report it (see :ref:`getting-help`), with the parameter file and the numbers of processes and threads.
