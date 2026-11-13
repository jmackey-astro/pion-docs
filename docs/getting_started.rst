.. _getting-started:

******************************
Getting Started
******************************

.. _introduction:

Introduction to PION
-------------------------------

PION is written in object-oriented C++ with the following modules:

+ Systems of equations: equations of inviscid HD and ideal MHD.
+ Coordinate systems: Cartesian coordinates in 1D, 2D, and 3D, Cylindrical coordinates in 2D :math:`(R,z)`, and spherical coordinates in 1D :math:`(r)`.
+ Hydro/MHD solvers: Roe and HLL Riemanns solvers are implemented for HD and MHD, flux-vector-splitting for HD, and HLLD for MHD.
+ Parallel code communication: using the Message Passing Interface (MPI).
+ Computational grid: The grid is a multiply-linked list of finite-volume cells (or zones). Most commonly-used boundary conditions are implemented. When run in parallel each process has a subdomain of the full grid, and inter-process communication is used to share boundary data.  A uniform grid or static mesh-refinement can be selected.
+ Microphysics: chemistry and heating/cooling processes. A number of different classes have been written for different approximations.
+ Raytracing, on serial and parallel grids, from point sources or sources at infinity. This uses the short-characteristics raytracer.
+ Data input and output (I/O), including ASCII, `FITS <https://heasarc.gsfc.nasa.gov/fitsio/fitsio.html>`_, and `Silo <https://wci.llnl.gov/simulation/computer-codes/silo>`_ formats.
+ Stellar wind source terms: constant or evolving stellar wind sources can be placed anywhere on the computational domain.


.. _getting-pion:

Getting the PION source code
----------------------------------

First things first: PION is free software; you can download it and redistribute it and/or modify it under the terms of The BSD 3-Clause License.  Read the :ref:`pion-license` here -- usage of PION implies acceptance of this license.
In particular, this software is provided by the authors "as is", in the hope that it will be useful, but any express or implied warranties are disclaimed, including but not limited to, the implied warranties of merchantability and fitness for a particular purpose.

There are two repositories for PION, the publicly released version and the private development version.  If you want to get involved in active development of new features, then you want to ask for access to the private repository.


Public release of PION
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The source code for PION is hosted on the `DIAS <https://www.dias.ie/>`_ gitlab server: `https://git.dias.ie/massive-stars-software/pion <https://git.dias.ie/massive-stars-software/pion>`_.  You can clone a copy of the source into a new directory called ``pion`` with the following command from within a terminal window:

.. code-block:: bash 

    $ git clone https://git.dias.ie/massive-stars-software/pion.git

Alternatively you can download a zip-file with the source code from `https://git.dias.ie/massive-stars-software/pion/-/archive/master/pion-master.zip <https://git.dias.ie/massive-stars-software/pion/-/archive/master/pion-master.zip>`_ if you prefer.

The PION git repository is also mirrored on `github <https://www.github.com/>`_ at `https://github.com/jmackey-astro/PION <https://github.com/jmackey-astro/PION>`_.


Development version of PION
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The source code for the development version of PION is hosted in a private repository on the `git.nrw <https://gitlab.git.nrw/>`_ gitlab server: `https://gitlab.git.nrw/jmackey1/pion-dev <https://gitlab.git.nrw/jmackey1/pion-dev>`_.  Once you have been given access, you can clone a copy of the source into a new directory called ``pion`` and switch to the ``devel`` branch with the following commands from within a terminal window:

.. code-block:: bash 

    $ git clone https://gitlab.git.nrw/jmackey1/pion-dev.git pion
    $ cd pion
    $ git checkout devel



.. _getting-help:

Getting help
----------------------------------

There are a few ways to get more information about PION and help with compiling, setting up and running your own simulations:

+ Look through the documentation at `https://www.pion.ie/ <https://www.pion.ie/>`_.
+ Read the :ref:`reference papers <reference-papers>`.
+ Subscribe to the PION mailing list and ask questions at `groups.io <https://groups.io/g/pion>`_.
+ Contact `info@pion.ie <mailto:info@pion.ie>`_ with general queries.


.. _system-reqs:

System Requirements for compiling and running PION
------------------------------------------------------

PION runs on Linux and macOS, and on HPC systems.
On Windows it can be compiled and run in the Windows Subsystem for Linux (see :ref:`compile-windows`).
You need:

+ A C++ compiler that supports C++14, e.g. ``g++`` or ``clang++``.
+ `CMake <https://cmake.org/>`_ version 3.10 or newer.
+ For parallel calculations: an MPI library with compiler wrappers (e.g. ``mpicxx`` from `OpenMPI <https://www.open-mpi.org/>`_ or MPICH), and optionally OpenMP for multi-threading within each MPI process.
+ `SUNDIALS <https://computing.llnl.gov/projects/sundials>`_, whose `CVODE <https://computing.llnl.gov/projects/sundials/cvode>`_ solver integrates the microphysics equations.  PION works with SUNDIALS versions 2 to 7.
+ `Boost C++ libraries <https://www.boost.org/>`_ version 1.73 or newer (PION uses the `modified Akima interpolation <https://www.boost.org/doc/libs/master/libs/math/doc/html/math_toolkit/makima.html>`_ from Boost).
+ The logging libraries `spdlog <https://github.com/gabime/spdlog>`_ and `fmt <https://github.com/fmtlib/fmt>`_.  spdlog version 1.13 or newer includes its own copy of fmt and does not need a separate fmt library on Linux; older spdlog versions, and all versions on macOS, need the fmt library as well.
+ `Silo <https://github.com/LLNL/Silo>`_ (version 4.11 or 4.12) for data I/O.  Silo is the recommended file format, and is required for parallel runs on HPC systems.
+ Optional: `CFITSIO <https://heasarc.gsfc.nasa.gov/fitsio/>`_, if you want FITS output (``-DPION_USE_FITS=ON``).  The parallel FITS I/O in PION is not efficient and should not be used on HPC systems.
+ Optional: the GNU ``readline`` library, which is needed for ``Debug`` builds.
+ Optional: the multi-ion non-equilibrium microphysics library `NEMO <https://gitlab.git.nrw/jmackey1/nemo>`_ (``-DPION_NEMO=ON``).  NEMO is installed separately (see its README), usually into ``~/.local``.

On recent Linux distributions and on macOS (Homebrew) all of these, except Silo on macOS, can be installed with the package manager.
On older systems or HPC clusters, the scripts in the ``extra_libraries/`` directory of the PION source code download, compile and install Boost, SUNDIALS, Silo and spdlog (see :ref:`compile-other`).

There are two options for compiling PION: the serial version, which runs on a single core, and the parallel version, which uses MPI (and optionally OpenMP).
For scientific applications you almost certainly want the parallel version, but the serial version is useful for developing new algorithms and debugging.

PION can be run in uniform-grid mode or nested-grid mode (with static mesh-refinement).
These two options have different executables and you can specify at compile-time which executables to compile.


.. _compilation:

Compiling PION
----------------------------------

The instructions below install the required libraries and compile PION on the most common operating systems.
They assume you have access to the development repository (see :ref:`getting-pion`), and that you can use ``sudo`` (Linux) to install packages.

.. _compile-debian:

Debian 12/13 and Ubuntu 22.04/24.04
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All of the required libraries are available as system packages.
These are the packages used to build and test PION in the continuous-integration pipeline (Debian 13), and they also work on newer releases.

.. code-block:: bash

  $ sudo apt update
  $ sudo apt install git g++ make cmake \
      openmpi-bin libopenmpi-dev \
      libsundials-dev libboost-dev libspdlog-dev libfmt-dev \
      libsilo-dev libsilo-bin libhdf5-dev \
      libcfitsio-dev libcfitsio-bin libreadline-dev
  $ git clone https://gitlab.git.nrw/jmackey1/pion-dev.git pion
  $ cd pion/
  $ git checkout devel
  $ cp scripts/compile-debian11.sh ./compile-pion.sh
  $ bash compile-pion.sh

The script ``compile-debian11.sh`` (despite its name, it works on all of the systems above) builds the parallel uniform- and nested-grid executables with MPI, OpenMP and Silo, the analysis tools and the test suite, in the directory ``build/``.
Alternatively, you can run cmake directly from the ``pion`` directory:

.. code-block:: bash

  $ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=mpicxx \
      -DPION_PARALLEL=ON -DPION_OMP=ON -DPION_USE_SILO=ON \
      -DPION_UNIFORM_GRID=ON -DPION_NESTED_GRID=ON \
      -DPION_TOOLS=ON -DPION_BUILD_TESTS=ON
  $ cmake --build build -j 4

To check that the build works, run the test suite (this takes a few minutes):

.. code-block:: bash

  $ ctest --test-dir build

Other Linux distributions (Fedora, Arch, openSUSE, ...) have equivalent packages, usually with similar names.

.. _compile-macos:

macOS
^^^^^^^^^^^^^^^^^^^

PION is compiled on macOS with `Homebrew <https://brew.sh/>`_, which provides all of the libraries except Silo.
Silo is compiled from source with a script in the PION repository.
The instructions below are for Apple Silicon (M1 and newer) Macs, where Homebrew is installed in ``/opt/homebrew``; on Intel Macs Homebrew uses ``/usr/local`` instead, so change the paths in the compile script accordingly.

1. Install Apple's command-line tools, and then Homebrew following `the Homebrew instructions <https://docs.brew.sh/Installation>`_:

   .. code-block:: bash

    $ xcode-select --install

2. Install the libraries, get the PION source code, and compile Silo (it is installed in ``~/.local/silo``):

   .. code-block:: bash

    $ brew update
    $ brew upgrade
    $ brew install git cmake wget open-mpi libomp sundials boost spdlog fmt cfitsio readline
    $ git clone https://gitlab.git.nrw/jmackey1/pion-dev.git pion
    $ cd pion/
    $ git checkout devel
    $ cd extra_libraries
    $ bash install_silo.sh
    $ cd ..

3. Compile PION:

   .. code-block:: bash

    $ cp scripts/compile-osx27-omp.sh ./compile-pion.sh
    $ bash compile-pion.sh

   Apple's ``clang++`` compiler (which the Homebrew ``mpicxx`` wrapper uses) does not include OpenMP, so the script points cmake to the ``libomp`` library from Homebrew.
   If you don't want OpenMP, use ``scripts/compile-osx.sh`` instead.

4. Run the test suite:

   .. code-block:: bash

    $ ctest --test-dir build

MacPorts is not tested any more.

.. _compile-windows:

Windows
^^^^^^^^^^^^^^^^^^^

PION can be compiled on Windows 10 and 11 in the `Windows Subsystem for Linux <https://learn.microsoft.com/en-us/windows/wsl/install>`_ (WSL 2), which runs a Linux distribution alongside Windows.

1. Open PowerShell as an administrator and install WSL with Ubuntu:

   .. code-block:: powershell

    > wsl --install -d Ubuntu-24.04

2. Restart the computer, start Ubuntu from the Start menu, and set up a username and password.
3. Follow the instructions for :ref:`compile-debian` in the Ubuntu terminal.

.. _compile-other:

Older systems and HPC clusters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On older Linux distributions some of the system packages are too old (e.g. Boost older than 1.73) or missing, and on HPC clusters the libraries are usually provided as environment modules (``module avail``, ``module load ...``).
Load or install what is available, and compile the missing libraries with the scripts in ``extra_libraries/``:

+ ``install_boost.sh``: Boost
+ ``install_sundials7.sh``: SUNDIALS 7
+ ``install_silo.sh``: Silo
+ ``install_spdlog.sh``: spdlog

Each script lists its options (installation directory and library version) at the top of the file, and contains settings for some HPC systems that you can use as examples.
Then add the installation directories to ``CMAKE_PREFIX_PATH`` when you run cmake, for example ``-DCMAKE_PREFIX_PATH="$HOME/.local"``.
The ``scripts/`` directory has compile scripts for some HPC systems that can be adapted for others.


.. _compile-scripts-outcomes:

Compilation scripts and resulting executables
-----------------------------------------------

Some flags and settings need to be chosen at compile-time, and these can be specified in a build script.
Example scripts are in the ``scripts/`` subdirectory of the PION source code, and can be modified as needed:

 + Debian / Ubuntu: ``scripts/compile-debian11.sh``
 + macOS with OpenMP: ``scripts/compile-osx27-omp.sh``
 + macOS without OpenMP: ``scripts/compile-osx.sh``
 + `MeluXina <https://docs.lxp.lu/>`_: ``scripts/compile-meluxina-gcc.sh``
 + `Karolina <https://docs.it4i.cz/karolina/introduction/>`_: ``scripts/compile-karolina.it4i.cz-gcc.sh``

These scripts must be copied to the top-level ``pion`` directory before you run them, because they use their own location to find the source code.

Running the build script should create some executable files in the directory ``build/``:

  + ``build/icgen-ug`` : initial-conditions generator for uniform-grid simulations
  + ``build/icgen-ng`` : initial-conditions generator for nested-grid simulations
  + ``build/pion-ug``  : PION executable for uniform-grid simulations
  + ``build/pion-ng``  : PION executable for nested-grid simulations

The executables have the same names for serial (``-DPION_PARALLEL=OFF``) and parallel builds.
With ``-DPION_TOOLS=ON``, analysis programs (e.g. ``silocompare``, ``silo2text``) are also built in ``build/``.

If you do not see these files, then probably the compilation process threw a lot of errors at you, and you can try to resolve these by looking at :ref:`compilation-issues`.

.. _compile-scripts-options:

Compile-time options
--------------------

Enabling/Disabling Features of Pion
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Note that the NEMO multi-ion module is disabled by default.
To use it, install the `NEMO library <https://gitlab.git.nrw/jmackey1/nemo>`_, add its installation directory to ``CMAKE_PREFIX_PATH``, and compile PION with ``-DPION_NEMO=ON``.

====================================  ===================================================================================
 Option                               Effect
====================================  ===================================================================================
``-DCMAKE_BUILD_TYPE=...``            ``Release`` for production runs, ``Debug`` for debugging (needs ``readline``)
``-DCMAKE_CXX_COMPILER=...``          The C++ compiler; use ``mpicxx`` for parallel builds
``-DPION_NESTED_GRID=...``            Set to ``ON`` to build nested grid simulations (default ``ON``)
``-DPION_UNIFORM_GRID=...``           Set to ``ON`` to build uniform grid simulations (default ``OFF``)
``-DPION_PARALLEL=...``               Set to ``ON`` to enable MPI for the Pion build , or ``OFF`` to disable MPI
                                      (default ``ON``)
``-DPION_OMP=...``                    Set to ``ON`` to enable OpenMP multithreading (default ``OFF``).
``-DPION_USE_SILO=...``               Set to ``ON`` to use Silo for handling data I/O (default ``ON``)
``-DPION_USE_FITS=...``               Set to ``ON`` to use Fits for handling data I/O (default ``OFF``)
``-DPION_NEMO=...``                   Set to ``ON`` to build with the multi-ion module NEMO (default ``OFF``)
``-DPION_TOOLS=...``                  Set to ``ON`` to also compile support programs in analysis subdir (default ``OFF``)
``-DPION_SKIP_SOURCE=...``            Set to ``ON`` to skip compilation of the Pion source files (default ``OFF``)
``-DPION_BUILD_DOCUMENTATION=...``    Set to ``ON`` to build Doxygen documentation (default ``OFF``)
``-DPION_BUILD_TESTS=...``            Set to ``ON`` to build tests (default ``OFF``)
``-DON_KAROLINA=...``                 Set to ``ON`` if building on karolina.it4i.cz (default ``OFF``)
``-DPION_INTEL=...``                  Set to ``ON`` only if using Intel compilers (default ``OFF``)
                                      This flag switches PION to include the Intel math header and not math.h
``-DLOG_ALL_PROCS=...``               Set to ``ON`` only for debugging to enable logging from all MPI processes
                                      (default is ``OFF`` ==> only rank 0)
====================================  ===================================================================================

Dependency Helpers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The simplest way to tell cmake where to find self-compiled libraries is ``CMAKE_PREFIX_PATH``, a semicolon-separated list of installation directories.

=============================  ======================================================================
Option                         Effect
=============================  ======================================================================
``-DCMAKE_PREFIX_PATH=...``    Search these installation directories for all libraries.
``-DSUNDIALS_DIR=...``         Instruct CMake to search the provided path for the Sundials library.
``-DSILO_DIR=...``             Instruct CMake to search the provided path for the Silo library.
``-DFITS_DIR=...``             Instruct CMake to search the provided path for the Fits library.
``-Dspdlog_DIR=...``           Instruct CMake to search the provided path for the Spdlog library.
``-DREADLINE_DIR=...``         Instruct CMake to search the provided path for the Readline library.
``-DBOOST_ROOT=...``           Instruct CMake to search the provided path for the Boost library.
=============================  ======================================================================

Setting installation directories
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

====================================  ====================================================================================
Option                                Effect
====================================  ====================================================================================
``-DCMAKE_INSTALL_PREFIX=...``          Set the root install directory for the compiled libraries and programs.
``-DPION_INSTALL_BINDIR=...``           Set the install directory for Pion executables. Use a relative path to set
                                        the path relative to ``${CMAKE_INSTALL_PREFIX}`` (default ``bin``).
====================================  ====================================================================================





.. _compilation-issues:

Trouble Compiling or Running PION?
----------------------------------

The most common problems in compilation are related to the support libraries.
The best way to figure out what went wrong is to look in at the text on-screen and find the first error message of the compilation (note that warnings are usually not a problem; only "error: ..." messages are critical problems).

If the compilation failed, then be sure to delete the build directory with `rm -rf build/` before you try again; otherwise some settings from the previous compilation effort may be retained.

MPI Error
^^^^^^^^^^^^^^^^^^^

If the MPI library is not found, then the following error can occur:

  .. code-block:: bash

    In file included from /home/jm/code/pion/source/comms/comm_mpi.cpp:44:
    /home/jm/code/pion/source/comms/comm_mpi.h:24:10: fatal error: mpi.h: No such file or directory
       24 | #include <mpi.h>
             |          ^~~~~~~
             compilation terminated.
             make[2]: *** [source/comms/CMakeFiles/comms.dir/build.make:76: source/comms/CMakeFiles/comms.dir/comm_mpi.cpp.o] Error 1
             make[1]: *** [CMakeFiles/Makefile2:528: source/comms/CMakeFiles/comms.dir/all] Error 2
             make[1]: *** Waiting for unfinished jobs....

This usually means that you need to specify the C++ compiler manually by adding the statement `-DCMAKE_CXX_COMPILER=mpicxx` to your cmake command.


SUNDIALS Error
^^^^^^^^^^^^^^^^^^^

An example error related to a SUNDIALS library mismatch is:
        
  .. code-block:: bash

    ../source/microphysics/cvode_integrator.cpp:173:15: error: no matching function for call to 'CVodeCreate' 

This indicates either the wrong library version for SUNDIALS, or that the library is not installed correctly.


Runtime Error: silo
^^^^^^^^^^^^^^^^^^^

If you get this error on running the PION initial-conditions generator:
  .. code-block:: bash

    IC file-type is silo
    IO class initialisation:         error code: silo ...exiting.


    --------------------------------------------------------------------------
    MPI_ABORT was invoked on rank 0 in communicator MPI_COMM_WORLD
    with errorcode 999.

    NOTE: invoking MPI_ABORT causes Open MPI to kill all MPI processes.
    You may or may not see output from other processes, depending on
    exactly when Open MPI kills them.
    --------------------------------------------------------------------------

then it probably means that SILO was not included by cmake, and you need to add the option `-DPION_USE_SILO=ON` to the cmake command.
The same applies to FITS if you choose to write snapshots in FITS format.

Getting Help
^^^^^^^^^^^^^^^^

If you are an experienced programmer and comfortable interpreting compiler error messages, then you may be able to figure out what went wrong.
In this case please do report your findings so they can be added to this documentation to help other new users.
To get help please contact the developers at `info@pion.ie <mailto:info@pion.ie>`_, or post a message on the forum `https://groups.io/g/pion <https://groups.io/g/pion>`_.
Please include all of the screen output from the compilation (machine-readable, not a screenshot).





