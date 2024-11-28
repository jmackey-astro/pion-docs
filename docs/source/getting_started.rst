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

The source code for the development version of PION is hosted on the `DIAS <https://www.dias.ie/>`_ gitlab server in a private repository: `https://git.dias.ie/compastro/pion <https://git.dias.ie/compastro/pion>`_.  You can clone a copy of the source into a new directory called ``pion`` with the following command from within a terminal window:

.. code-block:: bash 

    $ git clone https://git.dias.ie/compastro/pion.git



.. _getting-help:

Getting help
----------------------------------

There are a few ways to get more information about PION and help with compiling, setting up and running your own simulations:

+ Look through the documentation at `https://www.pion.ie/ <https://www.pion.ie/>`_.
+ Read the `reference papers <https://www.pion.ie/publications/>`_.
+ Subscribe to the PION mailing list and ask questions at `groups.io <https://groups.io/g/pion>`_.
+ Contact `info@pion.ie <mailto:info@pion.ie>`_ with general queries.


.. _system-reqs:

System Requirements for compiling and running PION
------------------------------------------------------

PION has been compiled and run on a number of linux and UNIX-based operating systems and OS X.
For all Operating Systems you need access to a C++ compiler such as gcc and, for multi-core calculations, an implementation of the MPI wrappers around the compiler, and `cmake <https://cmake.org/>`_.
A few extra libraries are needed to run PION:

+ Microphysics is handled by the `CVODE <https://computing.llnl.gov/projects/sundials/cvode>`_ solver, part of the `SUNDIALS <https://computing.llnl.gov/projects/sundials>`_ suite of solvers.
+ PION works with SUNDIALS version 2, 3, 4, and 5, depending on the operating system and availabe system libraries.
+ Data I/O can use ASCII text files, `FITS <https://heasarc.gsfc.nasa.gov/fitsio/fitsio.html>`_, and `SILO <https://wci.llnl.gov/simulation/computer-codes/silo>`_, which are appropriate for different situations. Parallel execution on HPC systems should use SILO because it is built on the HDF5 library and has good performance on supercomputers. PION uses SILO version 4.11, FITS version 3.X (should work with any).  The implementation of parallel I/O using FITS in PION should not be considered ready for use on HPC systems - it is not efficient.
+ Interpolation routines use the `modified Akima interpolation method <https://www.boost.org/doc/libs/master/libs/math/doc/html/math_toolkit/makima.html>`_ of the `Boost C++ libraries <https://www.boost.org/>`_.
+ Logging uses the libraries `spdlog <https://github.com/gabime/spdlog>`_ and `fmt <https://github.com/fmtlib/fmt>`_.

FITS, SILO, BOOST and CVODE can either use system libraries or self-compiled libraries.
Boost needs to be version 1.73.0 or newer, and some older operating systems do not fulfill this requirement.

There are two options for compiling PION, the serial version which runs on a single core with one thread, and the parallel version which uses MPI to run many processes on many cores.  For scientific applications you almost certainly want the parallel version, but the serial version is very useful for developing new algorithms and debugging.

PION can be run in uniform-grid mode or nested-grid mode (with static mesh-refinement).
These two options have different executables and you can specify at compile-time which executables to compile.

PION uses `cmake <https://cmake.org/>`_ for compilation.


.. _compilation:

Compiling PION
----------------------------------

Here are instructions for how to install the required libraries and compile PION for a number of different operating systems.

Debian 11 (Bullseye) and Ubuntu 21
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
+ Support libraries can be installed via the package manager, e.g. apt or aptitude.
+ An example script is provided to compile PION using CMAKE with various options.
+ You need ``sudo`` privileges or another means to install software on the computer.
+ *This has been tested on debian 11, but not on Ubuntu 21 (feedback welcome!)*

.. code-block:: bash 

  $ sudo apt install libcfitsio-bin libcfitsio-dev libsilo-dev libsilo-bin g++ \
    libsundials-dev openmpi-bin openmpi-common curl libcurl4-openssl-dev \
    libhdf5-dev git cmake libbz2-dev libspdlog-dev libfmt-dev libboost-dev
  $ git clone https://git.dias.ie/compastro/pion.git
  $ cd pion/
  $ git checkout devel
  $ cp scripts/build_debian11.sh ./build_pion.sh
  $ rm -rf build
  $ bash build_pion.sh


Debian 10 (Buster)
^^^^^^^^^^^^^^^^^^^

+ Support libraries can be installed in Debian 10 via the package manager, e.g. apt or aptitude, except for boost (version 1.71 is too old).
+ Then a script is run to install boost in `pion/extra_libraries/boost`, or else you can install boost 1.74 via debian backports.
+ Download (or write) a script to compile PION using Cmake.

Here is a recipe for compiling using the system boost library from debian backports.  First add the debian backports repository to your system following `these instructions <https://backports.debian.org/Instructions/>`_.
Then run the commands below from a terminal window (installing packages requires root access; here it is assumed you can use the ``sudo`` command for this).

.. code-block:: bash 

  $ sudo apt install libcfitsio-bin libcfitsio-dev libsilo-dev libsilo-bin g++ \
    libsundials-dev openmpi-bin openmpi-common curl libcurl4-openssl-dev \
    libhdf5-dev git cmake libbz2-dev libspdlog-dev libfmt-dev wget
  $ sudo apt install libboost1.74-all-dev/buster-backports
  $ git clone https://git.dias.ie/compastro/pion.git
  $ cd pion/
  $ git checkout devel
  $ cp scripts/build_debian.sh ./
  $ rm -rf build
  $ bash build_debian.sh

If you have trouble with debian backports you can also install boost from source and compile PION as follows:

.. code-block:: bash 

  $ sudo apt install libcfitsio-bin libcfitsio-dev libsilo-dev libsilo-bin g++ \
    libsundials-dev openmpi-bin openmpi-common curl libcurl4-openssl-dev \
    libhdf5-dev git cmake libbz2-dev libspdlog-dev libfmt-dev
  $ git clone https://git.dias.ie/compastro/pion.git
  $ cd pion
  $ git checkout devel
  $ cd extra_libraries
  $ bash install_boost.sh
  $ cd ..
  $ cp scripts/build_debian.sh ./
  $ rm -rf build
  $ bash build_debian.sh



Ubuntu 20
^^^^^^^^^^^^^^^^^^^

Follow the instructions for debian 10, except that there is no "backports" package repository for Ubuntu that contains boost >1.73 and so you have to follow the instructions for self-compiled boost library:

.. code-block:: bash 

  $ sudo apt install libcfitsio-bin libcfitsio-dev libsilo-dev libsilo-bin g++ \
    libsundials-dev openmpi-bin openmpi-common curl libcurl4-openssl-dev \
    libhdf5-dev git cmake libbz2-dev libspdlog-dev libfmt-dev
  $ git clone https://git.dias.ie/compastro/pion.git
  $ cd pion
  $ git checkout devel
  $ cd extra_libraries
  $ bash install_boost.sh
  $ cd ..
  $ cp scripts/build_debian.sh ./
  $ rm -rf build
  $ bash build_debian.sh


OS X
^^^^^^^^^^^^^^^^^^^

There are two main options on OSX for installing extra open-source software, namely `Homebrew <https://brew.sh/>`_ and `MacPorts <https://www.macports.org/>`_.
PION has been tested on OS X Mojave (10.14.6) with software installed via the MacPorts framework. The MPI compiler used is mpich. It is compiled with statically linked libraries.
As of January 2021, there was no working MPI compiler provided by MacPorts for OSX 11 (Big Sur), and so Homebrew is the recommended option for the latest OSX distributions.
PION has also been compiled using Homebrew on OS X Mojave and Big Sur.
Homebrew has no SILO package, so we have to build it ourselves.

1. Instructions for Homebrew:
   
   * Install Apple's Xcode and command-line tools and then Homebrew following `the Homebrew instructions <https://docs.brew.sh/Installation>`_.
   * Run the following commands:

   .. code-block:: bash 

    $ brew update
    $ brew upgrade
    $ brew install sundials cfitsio open-mpi boost git spdlog fmt curl cmake wget
    $ git clone https://git.dias.ie/compastro/pion.git
    $ cd pion/
    $ git checkout devel
    $ cd extra_libraries
    $ bash install_silo.sh
    $ cd ..
    $ rm -rf build
    $ cp scripts/build-osx.sh ./
    $ bash build-osx.sh 

2. Instructions for MacPorts (not clear if this is working anymore, please send feedback if it works for you!)
  
   * Install Apple's Xcode and command-line tools and then Macports (see macports installation instructions for this step).
   * Run the following commands:

   .. code-block:: bash 

    $ sudo port update
    $ sudo port install mpich-default silo sundials cfitsio boost git spdlog fmt cmake wget
    $ cd extra_libraries
    $ bash install_silo.sh
    $ cd ..
    $ rm -rf build
    $ cp scripts/build-osx.sh ./
    $ bash build-osx.sh 


Debian 9
^^^^^^^^^^

+ Support libraries can be installed in Debian 9 via the package manager, e.g. apt or aptitude, except for boost (the version is too old).
+ Then a script is run to install boost in `pion/extra_libraries/boost`.
+ Finally cmake is used to compile PION.
+ *Note that this configuration has not been tested for some time and may no longer work.*

.. code-block:: bash 

  $ sudo apt install libcfitsio-bin libcfitsio-dev libsilo-dev libsilo-bin python-silo \
    libsundials-dev openmpi-bin openmpi-common curl libcurl4-openssl-dev libhdf5-serial-dev git g++ cmake libbz2-dev libspdlog-dev wget
  $ git clone https://git.dias.ie/compastro/pion.git
  $ cd pion
  $ git checkout devel
  $ cd extra_libraries
  $ bash install_boost.sh
  $ cd ..
  $ cp scripts/build_debian.sh ./
  $ bash build_debian.sh


Ubuntu 18
^^^^^^^^^^^^^^^^^^^

The ``libsilo-dev`` library has a bug and doesn't work on ubuntu 18, but otherwise it is as for debian 9.

1. Install system libraries for fits, sundials.
2. Install local version of silo, spdlog/fmt and boost. (TODO: add instructions/script for spdlog)
3. Finally cmake is used to compile PION.

*Note that this configuration has not been tested for some time and may no longer work.*

  .. code-block:: bash 

    $ sudo apt install libcfitsio-bin libcfitsio-dev libsundials-dev wget
      openmpi-bin openmpi-common curl libcurl4-openssl-dev git g++ cmake libbz2-dev libspdlog-dev libhdf5-dev
    $ git clone https://git.dias.ie/compastro/superpion.git
    $ git clone https://git.dias.ie/compastro/pion.git
    $ echo "here you need to install spdlog and fmt... need to add instructions"
    $ cd superpion
    $ bash ./src/build_packages.sh --only=spdlog --build-dependencies --prefix=../pion/extra_libraries/
    $ cd ../pion
    $ git checkout devel
    $ cd extra_libraries
    $ bash install_boost.sh
    $ bash install_silo_hdf5.sh
    $ cd ..
    $ cp scripts/build_ubuntu18.sh ./
    $ bash build_debian.sh


Windows10
^^^^^^^^^^^^^^^^^^^

These instructions are to compile PION on Windows 10 using the Windows Subsystem for Linux 2 (WSL 2). This allows the running of a Linux environment on top of Windows 10 natively (uses a lightweight virtual machine). Once you have WSL 2 installed and a version of Linux (i.e Ubuntu18) set-up, compiling PION is just like you would on a normal linux system (i.e Ubuntu18). Here's how to install WSL 2:
   
1. Turn on Windows linux subsystem feature: 

  + Use the search bar to search for 'Turn Windows features on or off' and click the top result.
  + Check the Windows Subsystem for Linux option and click the OK button.
  + Restart your computer.
 
2. Install WSL:

  + Run PowerShell as an administrator.
  + Type the following command to enable the Virtual Machine Platform feature and press Enter:

   .. code-block:: bash 

    $ Enable-WindowsOptionalFeature -Online -FeatureName VirtualMachinePlatform
   
  + Restart your computer.
 
3. Update WSL to WSL 2:

  + Download this WSL 2 kernel update `HERE <https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi>`_
  + Double-click the ``wsl_update_x64.msi`` file and apply the update.
  + Run PowerShell as an administrator.
  + Type the following command to make Windows Subsystem for Linux 2 your default architecture for new distros that you install and press Enter:

  .. code-block:: bash 

   $ wsl --set-default-version 2

 
4. Install Linux version:

  + Now head to the Windows Store and pick your version of Linux to install (e.g. Ubuntu18).
  + Once it installs, type the following command into the PowerShell to verify the version of the distro you installed is set to 2.
  + If it says 1 then run the following command:

   .. code-block:: bash 

    $ wsl --set-version linux-name 2
 
    where linux-name is the name of your linux distro (use: 'wsl -l -v' to find its name).
 
5. Done

  + Run your linux distro to set it up and then install PION as if you were properly using Linux: :ref:`system-reqs`.


.. _compile-scripts-outcomes:

Compilation scripts and resulting executables
-----------------------------------------------

Some flags and settings need to be chosen at compile-time, and these can be specified in a build script.
Some example build sripts can be downloaded here and modified as needed.
They are also available in the ``scripts/`` subdirectory of the PION source code.

 + Debian 11 / Ubuntu 21: :download:`build-debian11.sh <build_scripts/build-debian11.sh>`
 + Debian 10 / Ubuntu 20: :download:`build-debian.sh <build_scripts/build-debian.sh>`
 + OS X: :download:`build-osx.sh <build_scripts/build-osx.sh>`
 + `Kay.ichec.ie <https://www.ichec.ie/about/infrastructure/kay>`_ using GCC compiler: :download:`build-kay.ichec.ie-gcc.sh <build_scripts/build-kay.ichec.ie-gcc.sh>`
 + `Kay.ichec.ie <https://www.ichec.ie/about/infrastructure/kay>`_ using Intel compiler: :download:`build-kay.ichec.ie-intel.sh <build_scripts/build-kay.ichec.ie-intel.sh>`

Running the build script should create some executable files in the directory ``build/``, for the parallel version these are:

  + ``build/icgen-ug`` : initial-conditions generator for uniform-grid simulations
  + ``build/icgen-ng`` : initial-conditions generator for nested-grid simulations
  + ``build/pion-ug``  : PION executable for uniform-grid simulations
  + ``build/pion-ng``  : PION executable for nested-grid simulations

For the serial version the letter 's' is appended to these filenames, e.g. ``build/pion-ngs``.

If you do not see these files, then probably the compilation process threw a lot of errors at you, and you can try to resolve these by looking at :ref:`compilation-issues`.


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





