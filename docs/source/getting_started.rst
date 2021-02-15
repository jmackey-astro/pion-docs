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

The source code for PION is hosted on the `DIAS <https://www.dias.ie/>`_ gitlab server: `https://git.dias.ie/massive-stars-software/pion <https://git.dias.ie/massive-stars-software/pion>`_.  You can clone a copy of the source into a new directory called ``pion`` with the following command:

.. code-block:: bash 

    $ git clone https://git.dias.ie/massive-stars-software/pion.git

Alternatively you can download a zip-file with the source code from `https://git.dias.ie/massive-stars-software/pion/-/archive/master/pion-master.zip <https://git.dias.ie/massive-stars-software/pion/-/archive/master/pion-master.zip>`_ if you prefer.

The PION git repository is also mirrored on `github <https://www.github.com/>`_ at `https://github.com/jmackey-astro/PION <https://github.com/jmackey-astro/PION>`_.


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
For all Operating Systems you need access to a C++ compiler such as gcc and, for multi-core calculations, an implementation of the MPI wrappers around the compiler.
A few extra libraries are needed to run PION:

+ Microphysics is handled by the `CVODE <https://computing.llnl.gov/projects/sundials/cvode>`_ solver, part of the `SUNDIALS <https://computing.llnl.gov/projects/sundials>`_ suite of solvers.
+ PION works with SUNDIALS version 2, 3, 4, and 5, depending on the operating system and availabe system libraries.
+ Data I/O can use ASCII text files, `FITS <https://heasarc.gsfc.nasa.gov/fitsio/fitsio.html>`_, and `SILO <https://wci.llnl.gov/simulation/computer-codes/silo>`_, which are appropriate for different situations. Parallel execution on HPC systems should use SILO because it is built on the HDF5 library and has good performance on supercomputers. SILO uses version 4.10.2, FITS uses version 3.390 but should work with all 3.x versions.
+ Interpolation routines use the `spline functions <https://www.gnu.org/software/gsl/doc/html/interp.html>`_ of the `GNU Scientific Library <https://www.gnu.org/software/gsl/>`_ (GSL).

FITS, SILO and CVODE can either use system libraries or self-compiled libraries. The GSL must be present as a system library.  Here are instructions for how to install the required libraries for a number of different operating systems:


Debian 9
^^^^^^^^^^

All of the required support libraries can be installed in Debian 9 via the package manager, e.g. apt or aptitude, as follows. 

.. code-block:: bash 

  $ sudo apt install libcfitsio-bin libcfitsio-dev libsilo-dev libsilo-bin python-silo \
    libsundials-dev openmpi-bin openmpi-common curl libhdf5-serial-dev git libgsl-dev g++


Debian 10
^^^^^^^^^^^^^^^^^^^

As Debian 9, but a couple of packages have changed name:

.. code-block:: bash 

  $ sudo apt install libcfitsio-bin libcfitsio-dev libsilo-dev libsilo-bin g++ \
  python-silo libsundials-dev openmpi-bin openmpi-common curl libhdf5-dev git libgsl-dev


Ubuntu 18
^^^^^^^^^^^^^^^^^^^

The ``libsilo-dev`` library has a bug and doesn't work, so no need to install here, but otherwise it is as for debian 9.

1. Install system libraries for fits, sundials, gsl:
  
  .. code-block:: bash 

    $ sudo apt install libcfitsio-bin libcfitsio-dev libsundials-dev 
      openmpi-bin openmpi-common curl git libgsl-dev g++
   

2. Install local version of silo:
      
  .. code-block:: bash 

    $ cd pion/extra_libraries
    $ bash ./install_all_libs.sh

  This should detect that the OS is Ubuntu and will only install the SILO library.
      

OS X
^^^^^^^^^^^^^^^^^^^

There are two main options on OSX for installing extra open-source software, `Homebrew <https://brew.sh/>`_ and `MacPorts <https://www.macports.org/>`_. PION has been tested on OS X Mojave (10.14.6) with software installed via the MacPorts framework. The MPI compiler used is mpich. It is compiled with statically linked libraries.
As of January 2021, there is no working MPI compiler provided by MacPorts for OSX 11 (Big Sur), and so Homebrew is the only option.

1. Install the support packages:
  
  * Macports: ``$ sudo port install mpich-default silo gsl sundials cfitsio``
  * Brew: ``$ brew install sundials gsl cfitsio open-mpi``

2. Install locally-compiled libraries (only with Homebrew -- with MacPorts the system packages should work):
      
  .. code-block:: bash 

    $ cd pion/extra_libraries
    $ bash ./install_all_libs.sh

  This should compile and install libraries for SILO.


Windows10
^^^^^^^^^^^^^^^^^^^

To compile PION on Windows10 you need to have the Windows Subsystem for Linux 2 (WSL 2) installed. This is architecture that allows the running of a Linux environment on top of Windows 10 natively (uses a lightweight virtual machine). Once you have WSL 2 installed and a version of Linux (i.e Ubuntu18) set-up, compiling PION is just like you would on a normal linux system (i.e Ubuntu18). Here's how to install WSL 2:
   
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


.. _compilation:

Compiling PION
----------------------------------

There are two options for compiling PION, the serial version which runs on a single core with one thread, and the parallel version which uses MPI to run many processes on many cores.  For scientific applications you almost certainly want the parallel version, but the serial version is very useful for developing new algorithms and debugging.

Once you have installed the required support libraries, you can compile PION with standard options via:

  + Parallel version: :code:`$ cd pion/bin_parallel/; bash compile_code.sh`
  + Serial version: :code:`$ cd pion/bin_serial/; bash compile_code.sh`


This should create some executable files in the directory ``pion/``, for the parallel version these are:

  + ``icgen-ug`` : initial-conditions generator for uniform-grid simulations
  + ``icgen-ng`` : initial-conditions generator for nested-grid simulations
  + ``pion-ug``  : PION executable for uniform-grid simulations
  + ``pion-ng``  : PION executable for nested-grid simulations

For the serial version the letter 's' is appended to these filenames, e.g. ``pion-ngs``.

If you do not see these files, then probably the compilation script threw a lot of errors at you, and you can try to resolve these by looking at :ref:`compilation-issues`.


.. _compilation-issues:

Trouble Compiling?
----------------------------------

The most common problems in compilation are related to the support libraries.
The best way to figure out what went wrong is to look in at the text on-screen and find the first error message of the compilation (note that warnings are usually not a problem; only "error: ..." messages are critical problems).
An example error related to a SUNDIALS library mismatch is:
        
  .. code-block:: bash

    ../source/microphysics/cvode_integrator.cpp:173:15: error: no matching function for call to 'CVodeCreate' 

This indicates either the wrong library version for SUNDIALS, or that the library is not installed correctly.

If you are an experienced programmer and comfortable interpreting compiler error messages, then you should be able to figure out what went wrong.
If not, then your best bet is to contact the developers at `info@pion.ie <mailto:info@pion.ie>`_, or post a message on the forum `https://groups.io/g/pion <https://groups.io/g/pion>`_.
Please include all of the screen output from the compilation (machine-readable, not a screenshot).





