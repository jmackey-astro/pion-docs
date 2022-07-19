#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

import numpy as np
from astropy import units as u
import matplotlib.pyplot as plt

# edit this to the appropriate path
import sys
sys.path.insert(0,"/Users/jm/.local/silo/lib")
from pypion.ReadData import ReadData

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
#plt.savefig("bowshock2D_test.png",bbox_inches="tight")



