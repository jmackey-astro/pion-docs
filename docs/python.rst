.. _python_usage:

Using Python to plot simulation
=========================================

`PyPion <https://github.com/greensh16/PyPion>`_ is a Python library that reads Silo data files from PION simulations and plots the data.
It works for 1D, 2D, and 3D data files and for any number of nested-grid levels, with Python 3.

`Silo <https://github.com/LLNL/Silo>`_ is both a scientific database format and a data I/O library, producing machine-independent data files that can be easily shared between different computing architectures.
The Silo library comes with a python module (``Silo``), but reading PION snapshots with it needs an extra layer of customisation.
This is what PyPion provides: a set of routines that call functions from the Silo python module to read PION snapshots into numpy arrays and plot them easily and efficiently.


.. _install_python:

Installing PyPion
----------------------------------

PyPion is installed from `PyPI <https://pypi.org/project/pypion/>`_ with ``pip``, but it also needs the Silo python module, which is not on PyPI and has to be compiled.
The recommended way is:

1. Create a Python `virtual environment <https://docs.python.org/3/library/venv.html>`_ and install PyPion in it.  Recent Linux distributions and Homebrew do not allow ``pip install`` into the system Python, so a virtual environment is needed.
2. With the virtual environment active, compile Silo and its python module with the script ``extra_libraries/install_silo.sh`` from the PION source code (see :ref:`getting-pion`).  The script builds the python module only if it finds the Python development headers (``Python.h``), and it builds it for the ``python3`` that is first in your ``PATH``, i.e. the one in the virtual environment.
3. Tell the virtual environment where to find the Silo python module.

The python module must be compiled for the same Python version as the virtual environment.
If you upgrade Python, or create the virtual environment with a different Python, then compile Silo again.

Debian and Ubuntu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

  $ sudo apt install build-essential cmake wget python3-dev python3-venv
  $ python3 -m venv ~/pypion-venv
  $ source ~/pypion-venv/bin/activate
  $ python -m pip install --upgrade pip
  $ python -m pip install pypion matplotlib
  $ mkdir -p ~/.cache/silo-build && cd ~/.cache/silo-build
  $ SILO_PREFIX=$HOME/.local/silo bash ~/pion/extra_libraries/install_silo.sh
  $ python -c 'import pathlib, site; p = pathlib.Path(site.getsitepackages()[0]) / "silo_local.pth"; p.write_text(str(pathlib.Path.home() / ".local/silo/lib") + "\n"); print(p)'

Change ``~/pion`` to the location of your copy of the PION source code.
The script downloads and compiles Silo in the current directory (here ``~/.cache/silo-build``, which you can delete afterwards), and installs it in ``~/.local/silo``.
Check that the output of the script contains the line ``*** Silo python module: ON``; if it says ``OFF`` then the Python development headers were not found.

The last command writes a file ``silo_local.pth`` into the virtual environment, so that Python finds ``Silo.so`` in ``~/.local/silo/lib`` without changing ``sys.path`` or ``PYTHONPATH``.
If Python later cannot load ``libsilo.so``, add ``export LD_LIBRARY_PATH="$HOME/.local/silo/lib:$LD_LIBRARY_PATH"`` to your shell startup file.

Debian 12/13 and recent Ubuntu versions also have a system package ``python3-silo``.
It works only with the system Python (or a virtual environment created with ``--system-site-packages``), and it has not been tested with PyPion; older versions of the package (Silo 4.10) do not work.

macOS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Install Apple's command-line tools and `Homebrew <https://brew.sh/>`_ as described in :ref:`compile-macos`, and then:

.. code-block:: bash

  $ brew install python3 cmake wget
  $ python3 -m venv ~/pypion-venv
  $ source ~/pypion-venv/bin/activate
  $ python -m pip install --upgrade pip
  $ python -m pip install pypion matplotlib
  $ mkdir -p ~/.cache/silo-build && cd ~/.cache/silo-build
  $ bash ~/pion/extra_libraries/install_silo.sh
  $ python -c 'import pathlib, site; p = pathlib.Path(site.getsitepackages()[0]) / "silo_local.pth"; p.write_text(str(pathlib.Path.home() / ".local/silo/lib") + "\n"); print(p)'

On macOS the script installs Silo in ``~/.local/silo``, which is the same library that PION uses (see :ref:`compile-macos`), so you only need one copy.
If you already compiled Silo for PION with the Homebrew ``python3``, and ``~/.local/silo/lib/Silo.so`` exists, then you can skip the ``install_silo.sh`` step.

On Apple Silicon Macs, make sure that Homebrew, Python and the terminal are all native (``arm64``), not running under Rosetta: ``uname -m`` and ``python3 -c "import platform; print(platform.machine())"`` should both print ``arm64``.

Check the installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With the virtual environment active, this should print the location of ``Silo.so`` without errors:

.. code-block:: bash

  $ python -c "import Silo; from pypion.ReadData import ReadData; print(Silo.__file__)"

Activate the virtual environment (``source ~/pypion-venv/bin/activate``) every time you open a new terminal to use PyPion.


.. _use_python:

Using PyPion to look at simulation data
-----------------------------------------------

The main scripts in the library are:

+ :code:`argparse_command.py` - Saves the options entered into the command line when a python script is run. 
+ :code:`SiloHeader_data.py` - Which opens the silo file and reads all of the important header variables (eg. sim_time, xmax, xmin, etc.).
+ :code:`ReadData.py` - Opens the directory in the silo (or vtk, or fits) file and reads the requested variable data (eg. density, temp, etc.).
+ :code:`Plotting_Classes.py` - Sets up a plotting function and the figure.

For the following example of how to plot PION data we are using the data created from the simulation in :ref:`example-sim`.
Make sure you run python3 from the directory containing the data files (or include the path in the filename strings).
You can also download the python script for the following example here: :download:`plot_pypion_ex.py <build_scripts/plot_pypion_ex.py>`.
Make sure to edit the ``base_path`` variable on line 9 appropriately before trying to run this script.

1. Import modules:

  First we import some modules liks numpy and astropy to help with the data analysis:

    .. code-block:: python 
      
      import numpy as np
      from astropy import units as u
      import matplotlib.pyplot as plt

  
  We import the PyPion library, here using ``ReadData`` module which inherits from the ``SiloHeader_data`` module.  One could also import the ``Plotting_Classes`` module for some higher level functions.
  If you installed the Silo python module as described in :ref:`install_python`, Python finds it automatically.
  Otherwise you need to add the directory containing ``Silo.so`` to the python path first, with e.g. ``sys.path.insert(0, "/path/to/silo/lib")``.

    .. code-block:: python

      from pypion.ReadData import ReadData


2. Bring in the data:

  We're going to plot all the levels from 1 timestep here, so create an array with the location and name of these 3 files.
  Then pass this array into the Plotting_Classes, and choose to read the variable ``Density`` from the snapshots.
  Again you'll need to change the ``data_path``.

    .. code-block:: python 

      data_path='/Users/jm/Documents/CODE/pion-dev/test/problems/Wind2D/'
      files = ( data_path+'Wind2D_HD_l3n0128_level00_0000.00009216.silo',
                data_path+'Wind2D_HD_l3n0128_level01_0000.00009216.silo',
                data_path+'Wind2D_HD_l3n0128_level02_0000.00009216.silo')

      read_data = ReadData(files)
      param = 'Density'
      data = read_data.get_2Darray(param)
      density = data['data']
      lim_max = data['max_extents'] * u.cm
      lim_min = data['min_extents'] * u.cm
      sim_time = (data['sim_time']*u.s).to(u.Myr)

3. Plot the data (here on a log scale, using imshow):

  Now that we have all the data we need, we can start to plot it. First let's create an empty figure instance.
  To plot the data from all the levels onto the same figure we need to loop over each level's data to plot it to the figure.

    .. code-block:: python

      fig = plt.figure()
      for i in range(len(density)):
        log_data = np.log10(density[i])
        plt.xlim(lim_min[0][0].value, lim_max[0][0].value)
        plt.ylim(lim_min[0][1].value, lim_max[0][1].value)
        im1 = plt.imshow(log_data, interpolation='nearest', cmap="viridis",
            extent=[lim_min[i][0].value, lim_max[i][0].value,
            lim_min[i][1].value, lim_max[i][1].value],
            origin='lower', vmax=-22, vmin=-27)

      plt.title('Time = %5.5f Myr' % sim_time.value)
      plt.show()


These are the basics you'll need to plot the simulation data; if you need to do other things like adding a colorbar or reflecting the data about the x-axis then see the Plotting_Classes.py script in the `PyPion repository <https://github.com/greensh16/PyPion>`_.
Here is an example script that makes the same plot as above, but now using the Plotting_Classes.py routines: :download:`plot_pypion2.py <build_scripts/plot_pypion2.py>`.


.. _issues_python:

Known issues with Python
------------------------------

There are a couple known issues with the python library.

+ If you run PyPion over ssh without an X server, or in a batch job, use a non-interactive matplotlib backend and save figures with ``plt.savefig()`` instead of ``plt.show()``.  Set the backend before importing ``matplotlib.pyplot``:

  .. code-block:: python

      import matplotlib
      matplotlib.use('Agg')
      import matplotlib.pyplot as plt

+ To stop python from printing matplotlib deprecation warnings to the screen, add the following after importing matplotlib:

  .. code-block:: python

      import warnings
      warnings.filterwarnings("ignore", category=matplotlib.MatplotlibDeprecationWarning)

+ ``ImportError: No module named Silo`` or ``ModuleNotFoundError: No module named 'Silo'`` means that Python cannot find ``Silo.so``: check that the virtual environment is active and that ``silo_local.pth`` points to the directory containing ``Silo.so`` (see :ref:`install_python`).  An error about an undefined symbol or a wrong architecture means that ``Silo.so`` was compiled for a different Python version or architecture, and needs to be compiled again.
