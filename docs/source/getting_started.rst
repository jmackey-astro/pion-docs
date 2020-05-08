
.. _introduction:

Introduction to PION
===========================

PION is written in object-oriented C++ with the following modules:

+ Systems of equations: equations of inviscid HD and ideal MHD.
+ Coordinate systems: Cartesian coordinates in 1D, 2D, and 3D, Cylindrical coordinates in 2D :math:`(R,z)`, and spherical coordinates in 1D :math:`(r)`.
+ Hydro/MHD solvers: Roe and HLL Riemanns solvers are implemented for HD and MHD, flux-vector-splitting for HD, and HLLD for MHD.
+ Parallel code communication: using the Message Passing Interface (MPI).
+ Computational grid: The grid is a multiply-linked list of finite-volume cells (or zones). Most commonly-used boundary conditions are implemented. When run in parallel each process has a subdomain of the full grid, and inter-process communication is used to share boundary data.  A uniform grid or a static nested grid can be selected
+ Microphysics: chemistry and heating/cooling processes. A number of different classes have been written for different approximations.
+ Raytracing, on serial and parallel grids, from point sources or sources at infinity. This uses the short-characteristics raytracer.
+ Data input and output (I/O), including ASCII, `FITS <https://heasarc.gsfc.nasa.gov/fitsio/fitsio.html>`_, and `Silo <https://wci.llnl.gov/simulation/computer-codes/silo>`_ formats.
+ Stellar wind source terms: constant or evolving stellar wind sources can be placed anywhere on the computational domain.


.. _getting-pion:

Getting the PION source code
================================

How to get the source code.


.. _system-reqs:

System Requirements for compiling and running PION
===================================================

This is what you need to install.


.. _compilation:

Compiling PION
=========================================

This is how to compile PION.


.. _example-sim:

Running an example simulation
=========================================

Instructions for running a simple simulation.



