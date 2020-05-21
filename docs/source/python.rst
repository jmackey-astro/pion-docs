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

We're also going to be plotting the density parameter here. So lets save the density data into an array called 'data' and also save the size of the grid and the simulation time into their own respective arrays.

.. code-block:: python
      
      param = 'Density'
      data = read_data.get_2Darray(param)['data']
      lim_max = (read_data.get_2Darray(param)['max_extents'] * u.cm)
      lim_min = (read_data.get_2Darray(param)['min_extents'] * u.cm)
      sim_time = read_data.get_2Darray(param)['sim_time'].to(u.Myr)


Now that we have all the data we need, we can start to plot it. First lets create an empty figure instance.

.. code-block:: python

      fig = plt.figure()
      
To plot the data from all the levels onto the same figure we need to loop over each level's data to plot iut to the figure.

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


This is the basics you'll need to plot the simulation data, if you need to do other things like adding a colorbar or reflecting the data about the x-axi then see the Plotting_Classes.py script in the PION_Python [add link] repository.


.. _issues_python:

Known issues with Python
------------------------------

There are a couple known issues with the python library.

+ If you want to run this library over ssh and don't have an x-server then you will need to add this piece of code after you import matlotlib:

.. code-block:: python

      matplotlib.use('Agg')
      
      
+ To stop ppython from printing deprecation warnigns to the screen then add hte following when importing modules:

.. code-block:: python

      import warnings
      warnings.filterwarnings("ignore", category=matplotlib.cbook.mplDeprecation)






