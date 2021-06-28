
.. _parameter-file:
  
******************************************************************
Description of the parameters in PION parameter files
******************************************************************

PION is controlled mainly by a parameter file that sets what kind of initial conditions to set up, what equations to use, the properties of the computational grid, source terms such as stellar wind, and numerical parameters specifying the integration scheme.
There are described here.

=====================================================
Initial Conditions Generator
=====================================================

There are a number of different problems that can be set up with PION, with source files located in ``source/ics/``:

=======================   =======================================  ==========================================
Problem                   Location of source file                  Brief description
=======================   =======================================  ==========================================
1Dto2D                    ``read_1Dto2D.cpp``                      Read 1D snapshot and map to 2D or 3D grid
Advection                 ``basic_tests.cpp``                      Sets up advection of an overdense clump
BlastWave                 ``blast_wave.cpp``                       Set up supernova blastwave problems
DivBPeak                  ``basic_tests.cpp``                      Sets up a uniform medium with a peak in
                                                                   :math:`\nabla\cdot\mathbf{B}`
DoubleMachRef             ``basic_tests.cpp``                      The Double Mach-Reflection test
FieldLoop                 ``basic_tests.cpp``                      Advection of a magnetic field loop
Jet                       ``jet.cpp``                              Sets up 2D axisymmetric jet or 3D jet
KelvinHelmholtz           ``basic_tests.cpp``                      Set up Kelvin-Helmholtz instability test
LaserAblationAxi          ``laser_ablation.cpp``                   Set up a HEDP laser ablation test calc
LiskaWendroffImplosion    ``basic_tests.cpp``                      The Liska and Wendroff implosion test
OrszagTang                ``basic_tests.cpp``                      The 2D Orszag-Tang Vortex test
PE_MC_FN or PE_MC_FM      ``photoevaporating_multiclumps.cpp``     Set up photoevaporation of dense clouds
PhotoEvaporatingClump     ``photoevaporating_clump.cpp``           Sets up photoevaporation of dense cloud
PhotEvap_RandomClumps     ``photoevaporating_random_clumps.cpp``   Set up photoevaporation of randomly placed
                                                                   dense clouds
RadiativeShock            ``radiative_shock.cpp``                  Sets up 1D or 2D radiative shock test
ShockCloud                ``shock_cloud.cpp``                      Set up shock-cloud interaction problems
Uniform                   ``basic_tests.cpp``                      Sets up a uniform medium
=======================   =======================================  ==========================================



=====================================================
Equations
=====================================================

There are three different types of equations that can be set up:

+ **Euler equations**
+ **Ideal MHD equations**
+ **GLM-MHD equations**


=====================================================
Computational Grid
=====================================================

The PION computational grid is a uniform rectilinear grid in 1, 2 or 3 spatial dimensions.
Static mesh-refinement (nested grids) is implemented for the case where each refinement level has the same shape and number of cells as the coarser level above it, but the spatial resolution is a factor of 2 higher and so the grid covers 1/2 of the domain of the coarser grid in each dimension.
The focus of the nested grid can be at the centre of the domain, the negative boundary or the positive boundary for each dimension.
The number of refinement levels is in principle unlimited, but snapshots can only be written every coarse timestep and this imposes a practical limit: for 10 grid levels there are 512 finest-level timesteps per coarsest-level step, and one probably wants to save a snapshot at least this often.
The coarsest level is denoted `level 0`, and each refined level has a higher level number.

The parameters are:

================  ==================  ================  ===========================================================
Parameter         Type                Units             Description
================  ==================  ================  ===========================================================
NGridX            integer             none              Number of grid points in :math:`\hat{x}`-direction
NGridY            integer             none              Number of grid points in :math:`\hat{y}`-direction
NGridZ            integer             none              Number of grid points in :math:`\hat{z}`-direction
Xmin              double              cm                negative boundary in :math:`\hat{x}`-direction (level 0)
Ymin              double              cm                negative boundary in :math:`\hat{y}`-direction (level 0)
Zmin              double              cm                negative boundary in :math:`\hat{z}`-direction (level 0)
Xmax              double              cm                positive boundary in :math:`\hat{x}`-direction (level 0)
Ymax              double              cm                positive boundary in :math:`\hat{y}`-direction (level 0)
Zmax              double              cm                positive boundary in :math:`\hat{z}`-direction (level 0)
================  ==================  ================  ===========================================================


=====================================================
Source terms
=====================================================


Stellar Winds
-------------

Stellar winds in PION are implemented in 3 modules: a constant wind, an evolving wind, and a latitude-dependent wind.
The parameter file for a constant wind has a number of parameters that should be set, quoted below with the expected units.  They are stored in these units in the ``SWP`` struct of type ``stellarwind_params``.

+---------------------+--------------------------------------------+-------------------------------------+
| Parameter           |   Description                              |   Units/values                      |
+=====================+============================================+=====================================+
| WIND_[i]_pos[n]     | Star position :math:`n\in[0,1,2]`          | cm                                  |
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

For constant winds, these data are stored in the global struct ``SWP``, defined in
``pion/source/constants.h``, using these units.  In simulation snapshots they also have the
same units.  
The evolving wind file should have all units in CGS.



