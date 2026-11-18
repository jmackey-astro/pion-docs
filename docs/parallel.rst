.. _parallel:

Running in parallel
===================

PION runs in parallel with MPI and OpenMP together:

+ **MPI**: the simulation domain is split into sub-domains, one for each MPI process.  Each process updates its own sub-domain, and exchanges boundary data (ghost cells) with its neighbours.
+ **OpenMP**: each MPI process uses several threads to loop over the cells of its sub-domain (hydrodynamics, microphysics and ray-tracing).

Parallel runs need executables compiled with ``-DPION_PARALLEL=ON`` (MPI) and ``-DPION_OMP=ON`` (OpenMP); see :ref:`compilation`.
Silo is the recommended file format for parallel runs, so PION should also be compiled with Silo (``-DPION_USE_SILO=ON``).


Number of MPI processes
-----------------------

**The number of MPI processes must be a power of 2** (1, 2, 4, 8, 16, ...).

PION chooses the decomposition itself: starting from the whole domain, it repeatedly halves the sub-domains along the axis where they are longest.
For example, a 2D domain twice as long in X as in Y, run on 8 processes, is split into 4 sub-domains along X and 2 along Y.
All levels of a nested grid use the same decomposition as level 0 (each level is split into the same number of sub-domains along each axis).

Small sub-domains spend relatively more time exchanging ghost cells, and the overhead of OpenMP threads is larger for small sub-domains, so do not use more processes and threads than the problem size justifies.
Short test runs with different numbers of processes and threads are the best way to choose.

A simulation can be restarted from a snapshot with a different number of MPI processes (still a power of 2), e.g. to move to a larger allocation.


Nested grids
^^^^^^^^^^^^

On nested grids, each sub-domain of a refined level must lie inside the sub-domain of a single process on the next coarser level.
PION checks this at startup, and stops with an error message if it is not true.  Two conditions must be met along each axis that is split between processes:

+ The number of grid cells of each level, divided by the number of processes along the axis, must be a whole, **even** number.
+ The edge of each refined level must lie on a sub-domain boundary of the coarser level.  This depends on the refinement centre (``NG_centre_XX``, ``NG_centre_YY``, ``NG_centre_ZZ``) and on the number of processes along the axis.  Because the decomposition always gives 1 or an even number of processes along each axis, this condition is always met if the refinement centre is at the centre of the domain or on a domain boundary.

If a run stops with one of these errors, change the number of grid cells, the refinement centre, or the number of MPI processes.
Note that the number of processes along each axis comes from the shape of the domain, as described above.


OpenMP threads
--------------

The number of OpenMP threads per MPI process is set on the command line with ``omp-nthreads=N`` (see :ref:`command-line`).
**Without this option PION uses one thread**, whatever ``OMP_NUM_THREADS`` is set to.
If ``N`` is larger than the number of threads available (``OMP_NUM_THREADS``, or else the number of cores), it is reduced with a warning.

Each thread has its own copy of the microphysics solver (including NEMO), so the microphysics of different cells is calculated independently by the threads.


Choosing processes and threads
------------------------------

Some guidelines for HPC systems:

+ Use (MPI processes per node) × (threads per process) = (cores per node).
+ Keep the threads of each process on one socket (CPU) of the node, so that they share the same memory.  For example, on nodes with 2 sockets of 20 cores, use 4 processes per node with 10 threads each (2 processes per socket), or 2 processes per node with 20 threads each.
+ Bind processes and threads to cores (``srun --cpu-bind=cores``, ``OMP_PLACES=cores``, ``OMP_PROC_BIND=close``), so that the operating system does not move them between cores.
+ Remember that the total number of MPI processes must be a power of 2.

On a workstation or laptop, start a parallel run with ``mpirun`` (or ``mpiexec``), for example with 4 processes of 2 threads each:

.. code-block:: bash

  $ mpirun -np 4 ./icgen-ng params.txt omp-nthreads=2
  $ mpirun -np 4 ./pion-ng MyRun_level00_0000.00000000.silo omp-nthreads=2


Batch jobs (Slurm)
------------------

Most HPC systems use the `Slurm <https://slurm.schedmd.com/>`_ batch system.
The PION repository has an example job script, ``scripts/slurm-example.sh``, which runs ``icgen-ng`` and then ``pion-ng`` on 4 nodes with 4 MPI processes per node and 10 OpenMP threads per process.
To use it, change the partition, account, modules and paths to those of your system, and then submit it with ``sbatch slurm-example.sh``.

The important parts of the script are:

.. code-block:: bash

  #SBATCH --nodes=4
  #SBATCH --ntasks=16             # MPI processes: a power of 2
  #SBATCH --ntasks-per-node=4
  #SBATCH --ntasks-per-socket=2
  #SBATCH --cpus-per-task=10      # OpenMP threads per process
  #SBATCH --exclusive
  #SBATCH --mem=0

  export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
  export OMP_PLACES=cores
  export OMP_PROC_BIND=close
  # srun does not inherit --cpus-per-task from sbatch in recent Slurm versions
  export SRUN_CPUS_PER_TASK=${SLURM_CPUS_PER_TASK}

  srun --cpu-bind=cores ${PION_DIR}/pion-ng ${IC_FILE} \
    omp-nthreads=${OMP_NUM_THREADS} \
    outfile=${OUTDIR}/${RUN_NAME} \
    redirect=${OUTDIR}/log_${RUN_NAME} \
    maxwalltime=5.75

Set ``maxwalltime`` (in hours) a little below the time limit of the job, so that PION writes a final snapshot and stops cleanly before the job is killed.
To continue the run in a new job, restart from the last snapshot (see :ref:`command-line-restart`).


Log files and output
--------------------

+ With ``redirect=PATH`` each MPI process writes its log to ``PATH_<rank>.log``.  In ``Release`` builds only rank 0 writes log messages, unless PION was compiled with ``-DLOG_ALL_PROCS=ON``.  See :ref:`command-line-logs`.
+ Parallel Silo snapshots are written by all processes together into a small number of files for each level and snapshot (one file per about 3×10\ :sup:`7` cells, but at most one per process).  PyPion and VisIt read these directly (see :ref:`python_usage`).
