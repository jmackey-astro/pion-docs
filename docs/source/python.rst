.. _python_usage:

Using Python to plot simulation
=========================================

PyPion is a Python library written to read in Silo data files from PION simulations and to plot the data. This library works for 1D, 2D, and 3D data files and for any amount of nested-grid levels. 
This library should be compatible with all versions of Python 3. 

`SILO <https://wci.llnl.gov/simulation/computer-codes/silo>`_ is both a scientific database format and a data I/O library, producing machine-independent data files that can be easily shared between different computing architectures.  It is often built using `HDF5 <https://www.hdfgroup.org/HDF5/>`_ as the low-level I/O driver, and has excellent performance on HPC systems.
The latest release of SILO, `version 4.11 released in September 2021 <https://github.com/markcmiller86/silo-issues/wiki/4.11-Release-Notes-September,-2021>`_, has support for python 3 and can be `downloaded from github <https://github.com/markcmiller86/silo-issues/releases/tag/v4.11>`_.
PyPion contains a script to download and install SILO 4.11, which should be used until this release gets packaged in operating system distributions.
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


Using the system python installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To be able to use all the features of this library you will need to have some python modules installed on your system.
And of course you need to have the lastest version of the PyPion repository pulled to your desktop! This is available here: `https://git.dias.ie/massive-stars-software/pypion/ <https://git.dias.ie/massive-stars-software/pypion/>`_.

Here are instructions for debian/Ubuntu installation:

+ Numpy: :code:`$ sudo apt install python3-numpy`
+ Astropy: :code:`$ sudo apt install python3-astropy`
+ Matplotlib: :code:`$ sudo apt install python3-matplotlib`
+ Tk: :code:`$ sudo apt install python3-tk`

All of these modules can also be installed through pip if you prefer: :code:`$ pip install 'module-name'`

+ Silo:
    .. code-block:: bash
      
      $ cd pypion/silo
      $ bash install_silo.sh



.. _use_python:

Using PyPion to look at simulation data
-----------------------------------------------

The main scripts in the library are:

+ :code:`argparse_command.py` - Saves the options entered into the command line when the python script is run. 
+ :code:`SiloHeader_data.py` - Which opens the silo file and saves all of the important header variables (eg. sim_time, xmax, xmin, etc.).
+ :code:`ReadData.py` - Opens the directory in the silo (or vtk, or fits) file and saves the requested variable data (eg. density, temp, etc.).
+ :code:`Plotting_Classes.py` - Sets up the plotting function and the figure.

For the following example of how to plot PION data we are using the data created from the simulation in :ref:`example-sim`. You can also download the python script for the following example here [Add link!].

1. Import modules:

  First you will want to import the PyPion library, all you need to import is :code:`ReadData.py` since it inherits all information from :code:`SiloHeader_data.py`.
  You need to import the Silo and PyPion functions/classes manually, by adding the path to these files to your python path, something like this:

    .. code-block:: python 

      import sys
      sys.path.insert(0,"/home/username/code/pypion/silo/lib")
      sys.path.insert(0,"/home/username/code/pypion/Library")
      import Silo
      from ReadData import ReadData


  Next import numpy and astropy to help with the data analysis.

    .. code-block:: python 
      
      import numpy as np
      from astropy import units as u

  And finally you will need to import matplotlib libraries to plot the data. 

    .. code-block:: python

      import matplotlib
      from matplotlib.colorbar import Colorbar
      import matplotlib.pyplot as plt
      import matplotlib as mpl
      from mpl_toolkits.axes_grid1 import make_axes_locatable
      from matplotlib.ticker import MultipleLocator
      import matplotlib.gridspec as gridspec


2. Bring in the data:

  We're going to plot all the levels from 1 timestep here, so create an array with the location and name of these 3 files. Then pass this array into the ReadData class.

    .. code-block:: python 

      arr = ('Wind2D_HD_l3n0128_level00_0000.00009216.silo', 'Wind2D_HD_l3n0128_level01_0000.00009216.silo', 'Wind2D_HD_l3n0128_level02_0000.00009216.silo')
      read_data = ReadData(arr)

  We're also going to be plotting the density parameter here. So lets save the density data into an array called 'data' and also save the size of the grid and the simulation time into their own respective arrays.

    .. code-block:: python
      
      param = 'Density'
      data = read_data.get_2Darray(param)['data']
      lim_max = (read_data.get_2Darray(param)['max_extents'] * u.cm)
      lim_min = (read_data.get_2Darray(param)['min_extents'] * u.cm)
      sim_time = read_data.get_2Darray(param)['sim_time'].to(u.Myr)


  Now that we have all the data we need, we can start to plot it. First let's create an empty figure instance.

    .. code-block:: python

      fig = plt.figure()
      
  To plot the data from all the levels onto the same figure we need to loop over each level's data to plot it to the figure.

    .. code-block:: python

      for i in range(len(data)):

            log_data = np.log10(data[i])

            ax1.set_title('Time = %5.5f Myr' % sim_time.value)

            ax1.set_xlim(lim_min[0][0].value, lim_max[0][0].value)
            ax1.set_ylim(lim_min[0][1].value, lim_max[0][1].value)

            im1 = ax1.imshow(log_data, interpolation='nearest', cmap="viridis",
                            extent=[lim_min[i][0].value, lim_max[i][0].value, lim_min[i][1].value, lim_max[i][1].value],
                            origin='lower', vmax=-22, vmin=-27)
                            
       plt.show()


These are the basics you'll need to plot the simulation data; if you need to do other things like adding a colorbar or reflecting the data about the x-axis then see the Plotting_Classes.py script in the `PyPion repository  <https://git.dias.ie/massive-stars-software/pypion/>`_.


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






