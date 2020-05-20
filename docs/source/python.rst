.. _python_usage:

Using Python to plot simulation
=========================================

PION_Python is a Python library written to read in Silo data files from PION simulations and to plot the data. This library works for 1D, 2D, and 3D data files and for any amount of nested-grid levels. 
For the moment this library only works with Python 2, work is being done to update to Python 3. 


.. _install_python:

Installing Requirements
----------------------------------
To be able to use all the features of this library you will need to have the following python
modules installed on your system. Obviously you don't need all of these if you only need parts of this library.

+ Silo: :code:`$ sudo apt install python-silo`
+ Numpy: :code:`$ sudo apt install python-numpy`
+ Astropy: :code:`$ sudo apt install python-astropy`
+ Matplotlib: :code:`$ sudo apt install python-matplotlib`
+ Tk: :code:`$ sudo apt install python-tk`

All of these modules can also be installed through pip if you prefer: :code:`$ pip install 'python-module'`

And of course you need to have the lastest version of the PION_Python repoitory oulled to your desktop!


.. _use_python:

Using PION_Python to look at simulation data
-----------------------------------------------

The main scripts in the library are:

+ argparse_command.py - Saves the options entered into the command line when the python script is run. 
+ SiloHeader_data.py - Which opens the silo file and saves all of the important header variables (eg. sim_time, xmax, xmin, etc.).
+ ReadData.py - Opens the directory in the silo (or vtk, or fits) file and saves the requested variable data (eg. density, temp, etc.).
+ Plotting_Classes.py - Sets up the plotting function and the figure.

For the following example of how to plot PION data we are using the data created from the simulation in :ref:`example-sim`. You can also download the python script for the following example here [Add link!].

1. Import modules:

First you will want to import the PION)Python library, all you need to import is ReadData.py since it inherits all information from SiloHeader_data.py.

.. code-block:: python 

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

We're also going to be plotting the density parameter here. So lets save the density data into an array called 'data' and also save the size of the grid and the simulation time into their own respective arrays..

.. code-block:: python
      
      param = 'Density'
      data = self.get_3Darray(param)['data']
      lim_max = (self.get_3Darray(param)['max_extents'] * u.cm).to(u.pc)
      lim_min = (self.get_3Darray(param)['min_extents'] * u.cm).to(u.pc)
      sim_time = self.get_3Darray(param)['sim_time'].to(u.Myr)

More to do..      
      

