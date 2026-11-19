.. _analysis-tools:

Analysis tools
==============

Most analysis of PION snapshots is done in Python, with PyPion (see :ref:`python_usage`), or with VisIt.
PION also has a few command-line programs for working with snapshots, in the directory ``analysis/`` of the source code.
This page describes four of them:

+ ``silocompare`` compares two snapshots, e.g. to check that a simulation gives the same result with different numbers of MPI processes.
+ ``silo2fits`` converts Silo snapshots to FITS files, e.g. for post-processing with TORUS.
+ ``silo2text`` converts Silo snapshots to text files, e.g. for plotting 1D results with gnuplot.
+ ``map-2d-to-3d`` maps a 2D axisymmetric snapshot onto a 3D grid, to start a 3D simulation from a 2D one.

They are built in ``build/`` with ``-DPION_TOOLS=ON``, and need a parallel build with Silo (``-DPION_PARALLEL=ON -DPION_USE_SILO=ON``); ``silo2fits`` also needs ``-DPION_USE_FITS=ON``, and ``map-2d-to-3d`` needs ``-DPION_NESTED_GRID=ON`` (see :ref:`compile-scripts-options`).
If PION is built with nested grids, the tools are built for nested grids, and read uniform-grid snapshots as well.

All of them are MPI programs: run them with one process, ``mpirun -np 1``.
On some systems they fail to start without ``mpirun``.


.. _silocompare:

silocompare
-----------

``silocompare`` reads two snapshots of the same grid, which may have been written by different numbers of MPI processes, and calculates the differences between them, variable by variable.
PION's tests use it to compare their results with reference snapshots (see :ref:`tests`).

.. code-block:: bash

  $ mpirun -np 1 ./silocompare <dir1> <file1> <dir2> <file2> <level> <outfile> <optype> [<tolerances>]

``<dir1>`` and ``<dir2>``
  The directories of the two snapshots (``.`` for the current directory).

``<file1>`` and ``<file2>``
  The names of the snapshots, or the beginning of the names: ``silocompare`` compares every ``.silo`` file in ``<dir1>`` whose name starts with ``<file1>`` with the file in the same position (in alphabetical order) of the list for ``<dir2>``.
  To compare a single pair of snapshots, give their full names.

``<level>``
  The grid level of the snapshots (0 for a uniform grid).
  For a nested grid, compare each level separately, with the files of that level (``..._level01_...`` for level 1).

``<outfile>``
  The base name of the output file.

``<optype>``
  What to calculate:

  + **0** or **1**: write a Silo file ``<outfile>.00000.silo`` with the difference of the two snapshots (second minus first) for every variable, except that density and pressure are the relative difference :math:`|p_1-p_0|/(|p_1|+|p_0|)`.  With 0 all differences are absolute values, with 1 they keep their sign.  The largest absolute and relative differences of each variable are printed.
  + **2**: for each variable, the mean absolute difference (L1), the root-mean-square difference (L2), the largest absolute difference, the relative L1 difference :math:`\sum|p_1-p_0|/\sum(|p_1|+|p_0|)`, and the largest relative difference in any cell.  These are printed and appended to the text file ``<outfile>_0.txt``.
  + **3**: the volume integrals of mass, momentum, kinetic, thermal and magnetic energy, and the mass of each tracer, in each snapshot, and their relative differences.  These are printed and appended to ``<outfile>_0.txt``.

``<tolerances>`` (optional)
  For optype 2, the snapshots count as the same if the largest relative L1 difference of all variables is below ``<rel-L1-tol>`` (default :math:`10^{-10}`) and the largest relative difference in any cell is below ``<cell-rel-tol>`` (default :math:`10^{-4}`): give either none or both of these.
  A third number, ``<tracer-abs-tol>``, leaves out tracers whose difference is below this value in every cell.
  For optype 3, the integrals count as the same if their relative differences are below ``<tol>`` (default :math:`10^{-10}`).

With optypes 2 and 3, the last line of the output is ``RESULTS ARE THE SAME ...`` or ``RESULTS DIFFER ...``; the exit code is 0 in both cases.
The variables are numbered in the order in which they are stored: density, pressure, the three velocity components, the three magnetic-field components (MHD), :math:`\psi` (GLM-MHD), and then the tracers.

For example, to check that a simulation gives the same result with 1 and 4 MPI processes (run in directories ``np1/`` and ``np4/``):

.. code-block:: bash

  $ mpirun -np 1 ./silocompare np1 Test_0000.00000100.silo np4 Test_0000.00000100.silo 0 cmp 2 | tail -3

and to look at where two snapshots differ, write the difference snapshot ``diff.00000.silo`` and open it with VisIt or PyPion:

.. code-block:: bash

  $ mpirun -np 1 ./silocompare np1 Test_0000.00000100.silo np4 Test_0000.00000100.silo 0 diff 1


.. _silo2fits:

silo2fits
---------

``silo2fits`` converts Silo snapshots to FITS files, e.g. for analysis with software that reads FITS images.
It is also the way to post-process PION simulations with the radiative-transfer code `TORUS <https://github.com/TimHarries/torus>`_ (Harries et al. 2019), which has a reader for PION's FITS snapshots on uniform and nested grids.
The TORUS reader does not work for simulations with NEMO.
For each snapshot it writes one FITS file, or one for each level of a nested grid.

.. code-block:: bash

  $ mpirun -np 1 ./silo2fits <input-dir> <input-file> <output-dir> <output-file> [<Nskip>]

``<input-dir>`` and ``<input-file>``
  The directory of the snapshots, and the beginning of their names: all ``.silo`` files in ``<input-dir>`` whose names start with ``<input-file>`` are converted.
  For a nested grid give the name of the level-0 files, e.g. ``MyRun_level00``: each level-0 snapshot is converted together with all of its finer levels.

``<output-dir>`` and ``<output-file>``
  The directory for the FITS files, and the beginning of their names.
  The files are called ``<output-file>_0000.<step>.fits``, or ``<output-file>_level<NN>_0000.<step>.fits`` for a nested grid.

``<Nskip>`` (optional)
  Convert only every (Nskip+1)-th snapshot (default 0: convert all of them).

For example:

.. code-block:: bash

  $ mkdir fits
  $ mpirun -np 1 ./silo2fits . MyRun_level00 fits MyRun

Each FITS file has:

+ A primary header with the simulation parameters of the snapshot, including the simulation time ``T_SIM`` (s), the timestep ``T_STEP``, the level ``GRID_LEVEL``, and its extent ``LEVEL_XMIN0`` ... ``LEVEL_XMAX2`` (cm).
+ One image extension for each variable, named ``GasDens``, ``GasPres``, ``GasVX``, ``GasVY``, ``GasVZ``, then ``Bx``, ``By``, ``Bz`` (MHD) and ``psi`` (GLM-MHD), the tracers ``Tr0``, ``Tr1``, ..., and ``Temp`` (temperature) if the simulation has microphysics.  The images have the dimensions of the grid level.

``silo2fits`` also writes a text file ``data_<output-file>.txt`` with the extent and number of cells of level 0 of the grid, and a block for each radiation source, one after the other in the order of the parameter file, with its position, strength (``RT_strength_N``) and effective temperature (``RT_Tstar____N``).
TORUS reads the source positions from this file.
For example, for a simulation with two sources:

.. code-block:: none

  Text data for simulation MyRun_level00

  ## GRID PROPERTIES ##
  Xmin 0.000000e+00  cm
  Xmax 8.000000e+18  cm
  ...
  N_Z 1
  #
  ## RADIATION SOURCE ##
  POS_X 3.000000e+18  cm
  POS_Y 0.000000e+00  cm
  POS_Z 0.000000e+00  cm
  Strength 4.000000e+38 erg/s  blackbody source
  T_star   3.750000e+04 K.   Star's effective T.
  #
  ## RADIATION SOURCE ##
  POS_X 7.000000e+18  cm
  ...

The units given for ``Strength`` are always ``erg/s``, but the value is ``RT_strength_N`` in whatever units that source uses (e.g. photons/s for a monochromatic source; see :ref:`parameter-file`).

For example, with Python:

.. code-block:: python

  from astropy.io import fits
  f = fits.open("fits/MyRun_level01_0000.00000830.fits")
  t = f[0].header["T_SIM"]
  rho = f["GasDens"].data    # numpy array [z, y, x] (3D) or [y, x] (2D)


.. _silo2text:

silo2text
---------

``silo2text`` converts Silo snapshots to text files with one line per cell, which can be read by gnuplot, Python (``numpy.loadtxt``), etc.
It is most useful for 1D simulations.

.. code-block:: bash

  $ mpirun -np 1 ./silo2text <input-dir> <input-file> <output-file> <N>

``<input-dir>`` and ``<input-file>``
  The directory of the snapshots, and the beginning of their names: the ``.silo`` files whose names start with ``<input-file>`` are converted.

``<output-file>``
  The beginning of the names of the text files, which are called ``<output-file>.<step>.txt``.

``<N>``
  Convert every N-th snapshot (1: convert all of them).

The first lines of each file give the columns and the simulation time and step:

.. code-block:: none

  # format: x,[y,z,],rho,pg,vx,vy,vz,[Bx,By,Bz],[Tr0,Tr1,Tr2,..],T,[Tau0,Tau1,...]
  # time = 6.312e+12  timestep = 21635

followed by one line per cell with the position of the cell centre, the primitive variables and the tracers, in the order in which they are stored (see :ref:`silocompare`).
Despite the header, there is no temperature column; for MHD there is one extra column at the end, the total pressure :math:`p_g + B^2/2`.
In 2D and 3D an empty line separates the rows of cells, as gnuplot expects for ``splot``.

``silo2text`` only works for uniform grids and for level 0 of nested grids; for the finer levels of a nested grid use PyPion.


.. _map-2d-to-3d:

map-2d-to-3d
------------

``map-2d-to-3d`` reads a 2D snapshot in cylindrical coordinates :math:`(z,R)`, and writes initial conditions for a 3D simulation in Cartesian coordinates, by rotating the 2D solution about its symmetry axis.
For example, a 2D simulation of a wind bubble can be continued in 3D to follow instabilities that axisymmetry suppresses.

.. code-block:: bash

  $ mpirun -np 1 ./map-2d-to-3d <2d-snapshot> <3d-parameter-file> [omp-nthreads=N]

``<2d-snapshot>``
  The 2D snapshot; for a nested grid, the level-0 file (the finer levels are read too).

``<3d-parameter-file>``
  A parameter file for the 3D simulation, which sets the 3D grid (uniform or nested), the output file name, the microphysics, etc., as for ``icgen`` (see :ref:`parameter-file`).

It writes the snapshot files ``<OutputFile>_level00_0000.00000000.silo``, ``..._level01_...``, etc., with the name ``OutputFile`` from the 3D parameter file, which can then be run with ``pion-ng`` like the output of ``icgen-ng``.

+ The :math:`z`-axis of the 2D grid becomes the :math:`z`-axis of the 3D grid, and each 3D cell at :math:`(x,y,z)` gets the values of the finest 2D cell at :math:`(z, R=\sqrt{x^2+y^2})`, without interpolation.  The velocity and magnetic field are rotated into Cartesian components.
+ The 3D simulation must use the same equations and the same tracers as the 2D one (``map-2d-to-3d`` stops with an error otherwise).
+ The 3D domain must lie inside the 2D domain in :math:`z`; cells beyond the largest :math:`R` of the 2D grid get the values of the outermost 2D cells.
+ The simulation time of the 3D snapshot is 0.  Stars, wind sources and radiation sources are taken from the 3D parameter file, so set them to match the 2D simulation.


Other tools
-----------

The directory ``analysis/`` also contains ``projection2D``, which calculates projected images (column densities, emission measure, H\ :math:`\alpha`, X-ray and radio emission) of 2D axisymmetric simulations.
It was written for specific projects, is not maintained, and is not described here: PyPion and `yt <https://yt-project.org/>`_ are better tools for most analysis.
Older tools that no longer work are in ``analysis/zz_deprecated/``.
