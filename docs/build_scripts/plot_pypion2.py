#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

import numpy as np
from astropy import units as u
import matplotlib.pyplot as plt

# edit this to the appropriate path
import sys
sys.path.insert(0,"/Users/jm/.local/silo/lib")
from pypion import Plotting_Classes as pypl

data_path='/Users/jm/Documents/CODE/pion-dev/test/problems/Wind2D/'
files = ( data_path+'Wind2D_HD_l3n0128_level00_0000.00009216.silo',
          data_path+'Wind2D_HD_l3n0128_level01_0000.00009216.silo',
          data_path+'Wind2D_HD_l3n0128_level02_0000.00009216.silo')

var1 = ["Density", -22, -27, "viridis", 'y', 63]

fig = plt.figure()
mod = pypl.Plotting2d(files)
fig = mod.plot2d_1(var1[0], fig, var1)

plt.show()

quit()


