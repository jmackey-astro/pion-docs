.. _python_usage:

Using Python to plot simulation
=========================================

PyPion is a Python library written to read in Silo data files from PION simulations and to plot the data. This library works for 1D, 2D, and 3D data files and for any amount of nested-grid levels. 
This library should be compatible with all versions of Python 3. 

`SILO <https://wci.llnl.gov/simulation/computer-codes/silo>`_ is both a scientific database format and a data I/O library, producing machine-independent data files that can be easily shared between different computing architectures.  It is often built using `HDF5 <https://www.hdfgroup.org/HDF5/>`_ as the low-level I/O driver, and has excellent performance on HPC systems.
The latest release of SILO, `version 4.11 released in September 2021 <https://github.com/markcmiller86/silo-issues/wiki/4.11-Release-Notes-September,-2021>`_, has support for python 3 and can be `downloaded from github <https://github.com/LLNL/Silo/releases/tag/v4.11>`_.
PyPion contains a script to download and install SILO 4.11, which should be used until SILO 4.11 gets packaged in operating system distributions.
Note that e.g. `debian <https://www.debian.org>`_ has a package called `python3-silo <https://packages.debian.org/bullseye/python3-silo>`_ but this was missing some key functionality and will not work until the package is upgraded from SILO 4.10 to 4.11.

The SILO library comes with a python interface, but these routines require an extra layer of customisation to read PION snapshots simply.
This is what PyPion provides -- a set of routines that call functions from the SILO python library to read PION snapshots into numpy arrays and plot them easily and efficiently.


.. _install_python:

Installing PyPion
----------------------------------

We have provided two ways to set up PyPion, using a `docker container <https://www.docker.com/>`_ and using your system python installation.

.. _docker_usage:

Using the PyPion docker image
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A docker image has been created to make it easier to run the python library.  Coming soon...


Using the system python3 on Debian/Ubuntu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To be able to use all the features of this library you will need to have some python modules installed on your system.
And of course you need to have the lastest version of the PyPion repository pulled to your desktop!
This is available here: `https://git.dias.ie/massive-stars-software/pypion/ <https://git.dias.ie/massive-stars-software/pypion/>`_.

Here are instructions for debian/Ubuntu installation:

+ Numpy: :code:`$ sudo apt install python3-numpy`
+ Astropy: :code:`$ sudo apt install python3-astropy`
+ Matplotlib: :code:`$ sudo apt install python3-matplotlib`
+ Tk: :code:`$ sudo apt install python3-tk`

All of these modules can also be installed through pip if you prefer: :code:`$ pip install 'module-name'`

+ Silo:  can be installed together with PyPion as follows:

  .. code-block:: bash
  
    $ git clone https://git.dias.ie/massive-stars-software/pypion ./pypion
    $ cd pypion
    $ git checkout master
    $ cd src/silo
    $ bash install_silo.sh
    $ cd ../..
    $ DIR=`${HOME}/.local/`; echo "${DIR}/silo/lib"
    $ python3 -m pip install astropy matplotlib pypion


Now the Silo library is in ``${HOME}/.local/silo/lib`` and the PyPion library can be imported like any other python package.

Note that this has only been tested on debian 11 / Ubuntu 20 systems, and may not work directly for you on other systems.


Using the system python3 on OS X
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  .. code-block:: bash
  
    $ brew install python numpy git wget gcc cmake
    $ git clone https://git.dias.ie/compastro/pion_python.git ./pypion
    $ cd pypion
    $ git checkout master
    $ cd src/silo
    $ bash install_silo.sh
    $ cd ../..
    $ DIR=`${HOME}/.local/`; echo "${DIR}/silo/lib"
    $ python3 -m pip install astropy matplotlib pypion

Now the Silo library is in ``${HOME}/.local/silo/lib`` and the PyPion library should be installed via pip.


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
  You may need to add the path to the ``Silo.a`` library, for example see below (although you will need to modify the paths):

    .. code-block:: python 

      import sys
      sys.path.insert(0,"/Users/jm/.local/silo/lib")
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


These are the basics you'll need to plot the simulation data; if you need to do other things like adding a colorbar or reflecting the data about the x-axis then see the Plotting_Classes.py script in the `PyPion repository  <https://git.dias.ie/massive-stars-software/pypion/>`_.
Here is an example script that makes the same plot as above, but now using the Plotting_Classes.py routines: :download:`plot_pypion2.py <build_scripts/plot_pypion2.py>`.


.. _issues_python:

Known issues with Python
------------------------------

There are a couple known issues with the python library.

+ If you want to run this library over ssh and don't have an x-server then you will need to add this piece of code after you import matlotlib:

  .. code-block:: python

      matplotlib.use('Agg')
      
      
+ To stop python from printing deprecation warnings to the screen then add the following when importing modules:

  .. code-block:: python

      import warnings
      warnings.filterwarnings("ignore", category=matplotlib.cbook.mplDeprecation)






