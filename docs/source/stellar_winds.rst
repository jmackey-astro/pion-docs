
Stellar Winds
-------------

Stellar winds in PION are implemented in 3 modules: a constant wind, an evolving wind, and a latitude-dependent wind.

Units
=====

The parameter file for a constant wind has a number of parameters that should be set.  They are quoted below with the expected units.  They are stored in these units in the SWP struct of type stellarwind_params.

+---------------------+--------------------------------------------+-------------------------------------+
| Parameter           |   Description                              |   Units/values                      |
+=====================+============================================+=====================================+
| WIND_[i]_pos[n]     | Star's :math:`n\in[x,y,z]`                 | cm                                  |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_radius     | Radius of wind injection region            | cm                                  |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_type       | [Constant, evolving, latitude  dependent]  | [0,1,2]                             |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_mdot       | Mass-loss rate from star                   | :math:`\mathrm{M_{\odot}\,yr}^{-1}` |
|                     | \(\dot{M}\)                                |                                     |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_vinf       | Terminal velocity of wind                  | :math:`\mathrm{km\,s}^{-1}`         |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_vrot       | Equatorial rotation velocity               | :math:`\mathrm{km\,s}^{-1}`         |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_temp       | Effective Temperature of star,             | K                                   |
|                     | :math:`T_\mathrm{eff}`                     |                                     |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_Rstr       | Radius of star                             | cm                                  |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_Bstr       | Strength of surface magnetic field         | Gauss                               |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_TR[n]      | Value of tracer :math:`n` in wind          | Usually :math:`\in [0,1]`           |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_evofile    | Text file with data for evolving wind      | Default is NOFILE                   |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_t_offset   | offset of sim time from evolution time     | Default is 0.0                      |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_t_scalefac | scaling simulation time to evolution time  | Default is 1.0                      |
+---------------------+--------------------------------------------+-------------------------------------+
| WIND_[i]_updatefreq | How often to update the wind               | seconds                             |
+---------------------+--------------------------------------------+-------------------------------------+
|WIND_[i]_enhance_mdot| ad-hoc flag to increase :math:`\dot{M}`    | Default is 0                        |
+---------------------+--------------------------------------------+-------------------------------------+

For constant winds, these data are stored in the global struct SWP, defined in
constants.h, using these units.  In simulation snapshots they also have the
same units.  

The evolving wind file should have all units in CGS.



