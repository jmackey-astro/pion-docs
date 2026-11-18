.. _tests:

Tests
=====

PION has a set of tests that run short simulations and check the results, either against reference snapshots or by comparing runs with different numbers of MPI processes.
They are run with ``ctest``, and most of them also run in the CI pipeline on GitLab.


Building and running the tests
------------------------------

The tests need a parallel build with Silo and the tools, and the option ``-DPION_BUILD_TESTS=ON`` (see :ref:`compilation`), e.g.:

.. code-block:: bash

  $ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=mpicxx \
      -DPION_PARALLEL=ON -DPION_OMP=ON -DPION_NESTED_GRID=ON -DPION_UNIFORM_GRID=ON \
      -DPION_USE_SILO=ON -DPION_TOOLS=ON -DPION_BUILD_TESTS=ON
  $ cmake --build build -j 8

Then, from the PION directory:

.. code-block:: bash

  $ ctest --test-dir build                       # all tests (a few minutes)
  $ ctest --test-dir build -j 8                  # several tests at the same time
  $ ctest --test-dir build -R nested-mhd-bw      # one test
  $ ctest --test-dir build -R "rhd-1d"           # all tests whose names match
  $ ctest --test-dir build --output-on-failure   # print the output of failed tests
  $ ctest --test-dir build -N                    # list the tests

Each test runs in ``build/test/``, where its snapshots and log files are kept, so you can look at them if a test fails.
Which tests are available depends on the build options: the NEMO tests need ``-DPION_NEMO=ON``, some tests need ``-DPION_OMP=ON``, and the nested-grid and uniform-grid tests need the corresponding executables.

Some longer tests are not run by ``ctest``, but can be run as build targets called ``test-<name>``, e.g.:

.. code-block:: bash

  $ cmake --build build --target test-dte-hybrid

The test scripts are in ``test/`` and can also be run by hand from ``build/test/``.
Many of them take an optional argument with the number of MPI processes (default 4), e.g. ``../../test/windrt2d_ng.sh 8``.


How results are checked
-----------------------

There are two kinds of test:

+ **Reference solution**: the final snapshot is compared with a reference snapshot in ``test/data/`` by ``silocompare``.  Most tests compare every cell: the relative L1 difference of each variable must be below :math:`10^{-10}`, and the relative difference in every cell below :math:`10^{-4}`.  Radiation-hydrodynamics results are not reproducible cell-by-cell on different computers, so those tests compare volume-integrated quantities (mass, momentum, energies and tracer masses) instead.
+ **Parallel consistency**: the simulation is run on 1 MPI process and on N processes (default 4), usually with 2 OpenMP threads each, and the results must be the same.  These tests have no reference solution; they check the MPI communication, which is where bugs that depend on the number of processes appear.

When a change to PION is meant to change the results (e.g. a bug fix), the reference snapshots have to be made again.
The comments at the top of each test script say how.
The reference files of the radiation-hydrodynamics tests were made on the CI system, and the NEMO reference files must be made with the NEMO version that CI uses (``NEMO_REF`` in ``.gitlab-ci.yml``).


List of tests
-------------

Tests run by ``ctest``:

.. list-table::
   :header-rows: 1
   :widths: 22 50 28

   * - Name
     - What it runs
     - Check
   * - ``uniform``
     - Double Mach reflection, 2D Euler, uniform grid
     - Reference
   * - ``pbc-otv``
     - Orszag-Tang vortex, 2D MHD, periodic boundaries
     - Reference
   * - ``3d-uniform``
     - 3D adiabatic blast wave, uniform grid
     - Reference
   * - ``3d-uniform-pbc``
     - 3D off-centre blast wave crossing periodic boundaries, uniform grid
     - Reference
   * - ``rhd-1d-mpv3``, ``rhd-1d-mpv5``, ``rhd-1d-mpv7``
     - 1D spherical expansion of an HII region (D-type), with MPv3, MPv5 or MPv7; 1 process
     - Reference (integrated)
   * - ``rhd-1d-mpv3-2src``, ``rhd-1d-mpv5-2src``, ``rhd-1d-mpv7-2src``
     - As above, with the source split into two sources of half the luminosity at the same position
     - Reference of the one-source test (integrated)
   * - ``hii40-nemo``
     - NEMO: Lexington HII40 benchmark, 1D, no hydrodynamics; 2 processes × 2 threads
     - Reference
   * - ``hii40-nemo-2src``
     - NEMO: as above with two sources of half the luminosity
     - Reference of ``hii40-nemo``
   * - ``orbit-2d-rt``
     - Two stars on a circular orbit, each with a wind and an ionising source (MPv3), 2D uniform grid
     - 1 vs N processes, and star trajectories
   * - ``dte-omp``
     - 2D cylindrical HII region expansion (MPv7), 1 process with up to 4 OpenMP threads
     - Reference (integrated)
   * - ``rhd-2d-2src``
     - 2D cylindrical, two ionising sources at different positions (MPv3)
     - 1 vs N processes
   * - ``nested-mhd-bw``
     - 2D MHD blast wave on 2 levels
     - Reference (each level)
   * - ``3d-nested``, ``3d-nested-pbc``
     - 3D adiabatic blast wave on 3 levels, with and without periodic boundaries
     - Reference (each level)
   * - ``windrt2d-ng``
     - Two wind sources with ray-tracing on 3 levels, 2D cylindrical and Cartesian
     - 1 vs N processes
   * - ``rhd-2d-ng-offsrc``
     - Two levels, two ionising sources, one outside the refined level (MPv3)
     - 1 vs N processes
   * - ``windrt1d-ng``
     - 1D spherical wind and ionising source on 3 levels
     - 1 vs N processes

Tests that are only build targets (``test-<name>``):

.. list-table::
   :header-rows: 1
   :widths: 22 50 28

   * - Name
     - What it runs
     - Check
   * - ``dte-mpi``, ``dte-hybrid``
     - As ``dte-omp``, with 4 processes × 1 thread, or 2 processes × 2 threads
     - Reference (integrated)
   * - ``rhd-2d-nemo-2src``
     - NEMO: 2D cylindrical, two ionising sources (about 2 minutes)
     - 1 vs N processes
   * - ``3d-uniform-large``, ``3d-uniform-pbc-large``, ``3d-nested-large``, ``3d-nested-pbc-large``
     - The 3D blast-wave tests on 16 processes
     - Reference
   * - ``binary-3d-ug``, ``binary-3d-ug-adi``, ``binary-3d-ng``, ``binary-3d-ng-adi``
     - Restart of a 3D binary-star simulation from a snapshot in ``test/data``
     - Reference


.. _tests-ci:

Tests in the CI pipeline
------------------------

The CI pipeline (``.gitlab-ci.yml``) runs in a Docker image with all of PION's dependencies (``ci/pion-ci.Dockerfile``).
It runs on merge requests and after each merge into ``devel``:

+ **Format**: checks that all files are formatted with ``clang-format-19``.
+ **Build**: a serial build (with FITS), a parallel build, and a parallel build with NEMO (uniform grid only).
+ **Test**: ``uniform``, ``pbc-otv``, ``3d-uniform``, ``rhd-1d-mpv3``, ``rhd-1d-mpv5``, ``rhd-1d-mpv7``, ``dte-omp``, ``nested-mhd-bw``, ``windrt2d-ng``, ``hii40-nemo`` and ``hii40-nemo-2src``, using the executables from the parallel builds.
+ **Only on** ``devel``: the slower tests ``3d-uniform-pbc``, ``3d-nested`` and ``3d-nested-pbc``, and the Doxygen code documentation, which is published with GitLab Pages (see :ref:`ref-dev-coding`).

The other ``ctest`` tests are not run in CI, so run them yourself (``ctest --test-dir build``) before opening a merge request.

If a test job fails, its output files are kept as job artifacts for a day.


Adding a test
-------------

1. Put the parameter file in ``test/ci-inputs/``, and write a script in ``test/`` based on one of the existing scripts.  The script runs in ``build/test/``, finds the executables in ``..``, and must exit with 0 if the test passes and 1 if it fails.
2. If the test compares with a reference solution, copy the final snapshot to ``test/data/``.  Keep the test small and short (a few seconds to a minute), because it runs in every CI pipeline.
3. Register the test with ``custom_add_test()`` in ``test/CMakeLists.txt``, inside the ``if`` blocks for the build options it needs.
4. To run it in CI, add a job to ``.gitlab-ci.yml`` that extends ``.test_job`` and sets ``TEST_NAME``.
