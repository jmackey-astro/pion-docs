
.. _parameter-file:
  
******************************************************************
Description of the parameters in PION parameter files
******************************************************************

PION is controlled mainly by a parameter file that sets what kind of initial conditions to set up, what equations to use, the properties of the computational grid, source terms such as stellar wind, and numerical parameters specifying the integration scheme.
The parameter file is ASCII text and each line should contain a key and its value, separated by whitespace.
String values should be input as normal text, floating-point numbers in format ``1.2345`` or ``1.2345e-67`` (PION defaults to run in double precision).
Unless otherwise stated, physical quantities should be input in CGS units, magnetic field in Gauss.
Comment lines that begin with ``#`` are ignored.
Some parameters are required and some are optional; for optional parameters PION will fall back to default values for any parameters that are not given.
The parameters are described in more detail below.

=====================================================
Initial Conditions Generator
=====================================================

There are a number of different problems that can be set up with PION, with source files located in ``source/ics/``.
The parameter ``ics`` determines which problem is set up, with the following options: 

============================   =======================================  ==========================================
Problem (key value)            Location of source file                  Brief description
============================   =======================================  ==========================================
``1Dto2D``                     ``read_1Dto2D.cpp``                      Read 1D snapshot and map to 2D or 3D grid
``Advection``                  ``basic_tests.cpp``                      Sets up advection of an overdense clump
``BlastWave``                  ``blast_wave.cpp``                       Set up supernova blastwave problems
``DivBPeak``                   ``basic_tests.cpp``                      Sets up a uniform medium with a peak in
                                                                        :math:`\nabla\cdot\mathbf{B}`
``DoubleMachRef``              ``basic_tests.cpp``                      The Double Mach-Reflection test
``FieldLoop``                  ``basic_tests.cpp``                      Advection of a magnetic field loop
``Jet``                        ``jet.cpp``                              Sets up 2D axisymmetric jet or 3D jet
``KelvinHelmholtz``            ``basic_tests.cpp``                      Set up Kelvin-Helmholtz instability test
``LaserAblationAxi``           ``laser_ablation.cpp``                   Set up a HEDP laser ablation test calc
``LiskaWendroffImplosion``     ``basic_tests.cpp``                      The Liska and Wendroff implosion test
``OrszagTang``                 ``basic_tests.cpp``                      The 2D Orszag-Tang Vortex test
``PE_MC_FN`` or ``PE_MC_FM``   ``photoevaporating_multiclumps.cpp``     Set up photoevaporation of dense clouds
``PhotoEvaporatingClump``      ``photoevaporating_clump.cpp``           Sets up photoevaporation of dense cloud
``PhotEvap_RandomClumps``      ``photoevaporating_random_clumps.cpp``   Set up photoevaporation of randomly placed
                                                                        dense clouds
``RadiativeShock``             ``radiative_shock.cpp``                  Sets up 1D or 2D radiative shock test
``ShockCloud``                 ``shock_cloud.cpp``                      Set up shock-cloud interaction problems
``Uniform``                    ``basic_tests.cpp``                      Sets up a uniform medium
============================   =======================================  ==========================================

Each of these types of initial-condition generator has some distinct parameters that should be set in the parameter file (discussed in more detail below).


=====================================================
General Parameters
=====================================================

Integration Scheme
-----------------------------------------------


Some required parameters set the coordinate system, equations to be solved, accuracy of the solution, etc.

=======================  ==================  ================  =================================================================
Parameter                Type                Units             Description
=======================  ==================  ================  =================================================================
``ndim``                 integer             none              Number of spatial dimensions to simulate
``coordinates``          string              none              Geometry. 1D: spherical, 2D: cylindrical/cartesian, 3D: cartesian
``eqn``                  string              none              System of equations to solve: `euler`, `mhd`, `glm-mhd`
``GAMMA``                double              none              adiabatic index of the gas (usually 5/3=1.66666666666667)
``solver``               integer             none              Which flux solver to use (see above for options)
``OrderOfAccSpace``      integer             none              spatial order of accuracy: 1 = piecewise constant; 2 =  piecewise
                                                               linear
``OrderOfAccTime``       integer             none              temporal order of accurasy: 1 = 1st order piecewise constant; 
                                                               2 = 2nd order, piecewise linear
``CFL``                  double              none              Courant-Friedrichs-Lewy number (<1 1D, <0.5 2D, <0.33 3D)
``ArtificialViscosity``  integer             none              0=no AV, 1=Falle+(1998) AV, 3=Sanders+(1998) H-correction 
                                                               H-correction only works with some solvers.
``EtaViscosity``         double              none              Viscosity parameter for Falle+(1998) AV, in range [0,1], 
                                                               default value is 0.15.
=======================  ==================  ================  =================================================================

There are three different types of equations that can be set up:

+ **Euler equations**: the inviscid equations of hydrodynamics.
+ **Ideal MHD equations**: magnetohydrodynamics, only usable in 1D because there is no scheme for dealing with DivB errors.
+ **GLM-MHD equations**: magnetohydrodynamics plus the Dedner et al. (2002) divergence cleaning mechanism for mitigating DivB errors.

There are a number of different flux solvers that can be used, with different levels of robustness and accuracy:

Available solvers:

+ **Euler**: 0 = Lax-Friedrichs, 1 = linear, 2 = exact (slow), 3 = linear+exact hybrid, 4 = Roe conserved variables, 6 = Flux-vector splitting, 8 = HLL, 9 = Roe+HLL hybrid)
+ **MHD**: 0 = Lax-Friedrichs, 1 = linear, 4 = Roe conserved variables, 7 = HLLD+HLL hybrid, 8 = HLL

Notes:

+ Recommended solver for the Euler equations is 9, and for the MHD equations is 7.
+ GAMMA should be in the range (1,2)
+ Spatial and temporal order of accuracy may both be set to 1 or 2, but they should be the same as each other.  2nd-order scheme is better.
+ The CFL parameter for an unsplit scheme should be <1/ndim
+ Cartesian coordinate system is available in 1D,2D,3D.
+ Cylindrical coordinates are only available in 2D :math:`(z,R)` with rotational symmetry in :math:`\theta` direction.  The radial coordinate (``y`` axis) should begin at :math:`R=0` and use an axisymmetric boundary condition.
+ Spherical coordinate system is only available in 1D, radial coordinate, with reflection symmetry at :math:`r=0`.


Computational Grid
-----------------------------------------------

The PION computational grid is a uniform rectilinear grid in 1, 2 or 3 spatial dimensions.
Static mesh-refinement (nested grids) is implemented for the case where each refinement level has the same shape and number of cells as the coarser level above it, but the spatial resolution is a factor of 2 higher and so the grid covers 1/2 of the domain of the coarser grid in each dimension.
The focus of the nested grid can be at the centre of the domain, the negative boundary or the positive boundary for each dimension.
The number of refinement levels is in principle unlimited, but snapshots can only be written every coarse timestep and this imposes a practical limit: for 10 grid levels there are 512 finest-level timesteps per coarsest-level step, and one may wish to save a snapshot at least this often.
The coarsest level is denoted ``level 0``, and each refined level has a higher level number.

The parameters are:

========================  ==================  ================  =================================================================
Parameter                 Type                Units             Description
========================  ==================  ================  =================================================================
``NGridX``                integer             none              Number of grid points in :math:`\hat{x}`-direction
``NGridY``                integer             none              Number of grid points in :math:`\hat{y}`-direction
``NGridZ``                integer             none              Number of grid points in :math:`\hat{z}`-direction
``Xmin``                  double              cm                negative boundary in :math:`\hat{x}`-direction (level 0)
``Ymin``                  double              cm                negative boundary in :math:`\hat{y}`-direction (level 0)
``Zmin``                  double              cm                negative boundary in :math:`\hat{z}`-direction (level 0)
``Xmax``                  double              cm                positive boundary in :math:`\hat{x}`-direction (level 0)
``Ymax``                  double              cm                positive boundary in :math:`\hat{y}`-direction (level 0)
``Zmax``                  double              cm                positive boundary in :math:`\hat{z}`-direction (level 0)
``grid_nlevels``          integer             none              Number of grid levels for static mesh-refinement (>=1)
``grid_aspect_ratio_XX``  integer             none              Aspect ratio of grid in each dimension
``grid_aspect_ratio_YY``  integer             none              Aspect ratio of grid in each dimension
``grid_aspect_ratio_ZZ``  integer             none              Aspect ratio of grid in each dimension
``NG_centre_XX``          double              cm                focus of nested grid refinement in X
``NG_centre_YY``          double              cm                focus of nested grid refinement in Y
``NG_centre_ZZ``          double              cm                focus of nested grid refinement in Z
``NG_refine_XX``          integer             none              refine in x-direction? (1==yes)
``NG_refine_YY``          integer             none              refine in y-direction? (1==yes)
``NG_refine_ZZ``          integer             none              refine in z-direction? (1==yes)
========================  ==================  ================  =================================================================



Boundary Conditions
-----------------------------------------------

+---------------------+----------+--------------------------------------------------------------+
| parameter name      |  type    | description                                                  |
+=====================+==========+==============================================================+
| ``BC_XN``           |  string  | boundary condition for negative x-direction                  |
+---------------------+----------+--------------------------------------------------------------+
| ``BC_XP``           |  string  | boundary condition for positive x-direction                  |
+---------------------+----------+--------------------------------------------------------------+
| ``BC_YN``           |  string  | boundary condition for negative y-direction                  |
+---------------------+----------+--------------------------------------------------------------+
| ``BC_YP``           |  string  | boundary condition for positive y-direction                  |
+---------------------+----------+--------------------------------------------------------------+
| ``BC_ZN``           |  string  | boundary condition for negative z-direction                  |
+---------------------+----------+--------------------------------------------------------------+
| ``BC_ZP``           |  string  | boundary condition for positive z-direction                  |
+---------------------+----------+--------------------------------------------------------------+
| ``BC_Ninternal``    |  integer | Number of internal boundary regions (stellar wind)           |
+---------------------+----------+--------------------------------------------------------------+
| ``BC_INTERNAL_000`` |  string  | type of 1st internal boundary region (if ``BC_Ninternal`` >0)|
+---------------------+----------+--------------------------------------------------------------+

The possible values for the boundary conditions are listed in :ref:`ref-boundary-conditions`.


Units
-----------------------------------------------

PION uses CGS units by default.
There are parameters to set up a system of units, but currently they do not do anything.
Internally PION solves the MHD equations in units such that factors of :math:`4\pi` do not appear, but these are rescaled by :math:`\sqrt{4\pi}` on reading from and writing to snapshots, and so the snapshots have magnetic field units of Gauss.

+---------------+--------+------+
| parameter name| type   | value|
+===============+========+======+
| units         | string | cgs  |
+---------------+--------+------+
| rhoval        | double | 1.0  |
+---------------+--------+------+
| lenval        | double | 1.0  |
+---------------+--------+------+
| velval        | double | 1.0  |
+---------------+--------+------+
| magval        | double | 1.0  |
+---------------+--------+------+



=====================================================
Source terms
=====================================================


Radiation sources
--------------------

The parameter ``RT_Nsources`` is an integer setting the number of radiation sources.
Each source ``i``, numbered from 0, has parameters of the form ``RT_[name]_[i]`` that describe the radiation field emitted.

+------------------------+-----------+------------------------------------------------------------------------+
| parameter key          | value type| description                                                            |
+========================+===========+========================================================================+
| ``RT_position_[i]_[n]``| double    | position of source along axis ``n`` (cm).  Value >1e100 (or <-1e100)   |
|                        |           | implies a source at infinity.                                          |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_src_type_[i]``    | integer   | 1=point source (tested), 2=diffuse radiation (untested)                |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_at_infty_[i]``    | integer   | 1: source is at infinity, 0: point source obeying inverse square law   |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_update___[i]``    | integer   | 1: use C2-ray update (Mellema+2006); 2: use Mackey(2012) update;       |
|                        |           | 2 is recommended - 1 may be broken.                                    |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_effect___[i]``    | integer   | 1: UV heating, 2: monochromatic photoionizing field, 3: multifrequency |
|                        |           | photoionization (switch to 5 in next release UPDATE CODES!).           |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Tau_src__[i]``    | integer   | set to 10 so that the opacity in a cell is calculated by the           |
|                        |           | microphysics module.  Other options are being deprecated.              |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Tau_var__[i]``    | integer   | unused if ``RT_Tau_src`` is set to 10.  Other options deprecated.      |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Nbins____[i]``    | integer   | Number of energy bins for radiation field (set to 1 unless using MPv10)|
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_EVO_FILE_[i]``    | string    | "NONE": constant wind, or name of text file containing time evolution  |
|                        |           | information.                                                           |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_strength_[i]``    | double    | luminosity in erg/s for multi-frequency source,                        |
|                        |           | or in photons/s for monochromatic source. For a source at infinity     |
|                        |           | this is the incident flux (erg/cm2/s or photons/cm2/s).                |
|                        |           | Not used for evolving source.                                          |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Rstar____[i]``    | double    | Stellar radius in ``R_sun``.  Not used for evolving source.            |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Tstar____[i]``    | double    | Stellar effective temperature in K. Not used for evolving source.      |
+------------------------+-----------+------------------------------------------------------------------------+



Stellar Winds
-------------

The parameter ``WIND_NSRC`` is an integer setting the number of stellar-wind sources.
These are implemented as a spherical boundary region in which the fluid quantities are set according to the wind parameters for each source.


Stellar winds in PION are implemented in 3 modules: a constant wind, an evolving wind, and a latitude-dependent wind.
The parameter file for a constant wind has a number of parameters that should be set, quoted below with the expected units.  They are stored in these units in the ``SWP`` struct of type ``stellarwind_params``.

+---------------------------+--------------------------------------------+-------------------------------------+
| Parameter                 |   Description                              |   Units/values                      |
+===========================+============================================+=====================================+
| ``WIND_[i]_pos[n]``       | Star position :math:`n\in[0,1,2]`          | cm                                  |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_velocity[n]``  | Star velocity :math:`n\in[0,1,2]`          | cm/s (if moving) default is 0       |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_radius``       | Radius of wind injection region            | cm                                  |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_type``         | [Constant, evolving, latitude  dependent]  | [0,1,2]                             |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_mdot``         | Mass-loss rate from star                   | :math:`\mathrm{M_{\odot}\,yr}^{-1}` |
|                           | \(\dot{M}\)                                |                                     |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_vinf``         | Terminal velocity of wind                  | :math:`\mathrm{km\,s}^{-1}`         |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_vrot``         | Equatorial rotation velocity               | :math:`\mathrm{km\,s}^{-1}`         |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_temp``         | Effective Temperature of star,             | K                                   |
|                           | :math:`T_\mathrm{eff}`                     |                                     |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_Rstr``         | Radius of star                             | cm                                  |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_Bstr``         | Strength of surface magnetic field         | Gauss                               |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_TR[n]``        | Value of tracer :math:`n` in wind          | Usually :math:`\in [0,1]`           |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_evofile``      | Text file with data for evolving wind      | Default is NOFILE                   |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_t_offset``     | offset of sim time from evolution time     | Default is 0.0                      |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_t_scalefac``   | scaling simulation time to evolution time  | Default is 1.0                      |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_updatefreq``   | How often to update the wind               | seconds                             |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_enhance_mdot`` | ad-hoc flag to increase :math:`\dot{M}`    | Default is 0                        |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_moving``       | is star moving? yes==1, no==0              | Default is 0                        |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_acceleration`` | is wind accelerated? yes==1, no==0         | Default is 0 (inject at v_inf)      |
+---------------------------+--------------------------------------------+-------------------------------------+


For constant winds, these data are stored in the global struct ``SWP``, defined in
``pion/source/constants.h``, using these units.  In simulation snapshots they also have the
same units.
The evolving wind file should have all units in CGS.


Jet inflow
----------------

There is an untested module for injecting protostellar jets along the symmetry axis in 2D cylindrical simulations, or along the positive x-axis in 3D Cartesian simulations.
The parameter ``N_JET`` takes an integer argument for the number of jet sources.
The module worked in 2010 when it was used for testing v.1.0 of PION, but has not been used since then.
If you want to know more about this, especially what extra parameters you need to include, take a look at ``source/ics/jet.cpp`` (and probably modify the source code for your needs).


=============================================================
Types of initial conditions that have been programmed
=============================================================


Uniform
-------------------------------------------------------------

This sets up uniform initial conditions throughout the domain in the standard mode, but it can also be used to set up a spherically symmetric medium with a constant density core and a power-lsw density profile outside the core.
One may add random adiabatic noise to this initial condition by setting the parameter ``noise`` to a value between 0 and 1 (this is the fractional level of the noise with respect to background density).
Parameters:

+----------------------------+--------+----------------------------------------------------------------+
| parameter key              | type   | description                                                    |
+============================+========+================================================================+
| ``UNIFORM_ambRO``          | double | gas density (g/cm3)                                            |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambPG``          | double | gas pressure (dyne/cm2)                                        |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambVX``          | double | x-velocity (cm/s)                                              |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambVY``          | double | y-velocity (cm/s)                                              |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambVZ``          | double | z-velocity (cm/s)                                              |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambBX``          | double | magnetic field BX (Gauss)                                      |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambBY``          | double | magnetic field BY (Gauss)                                      |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambBZ``          | double | magnetic field BZ (Gauss)                                      |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambTR[N]``       | double | initial value of tracer ``N``                                  |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_radial_slope``   | double | If non-zero, this is the power-law slope of density with radius|
|                            |        | (spherically symmetric)                                        |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_radius``    | double | constant-density core radius for case with power-law density   |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_radial_velocity``| double | expansion/contraction velocity w.r.t. origin                   |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_centre_XX`` | double | x position of core centre                                      |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_centre_YY`` | double | y position of core centre                                      |
+----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_centre_ZZ`` | double | z position of core centre                                      |
+----------------------------+--------+----------------------------------------------------------------+




ShockCloud
-------------------------------------------------------------

Sets up a shock-cloud test problem


RadiativeShock
-------------------------------------------------------------

Sets up a radiative shock test, with a flow hitting a reflecting boundary.


PhotEvap_RandomClumps
-------------------------------------------------------------

Sets up a simulation with randomly placed clumps on a uniform ISM


PhotoEvaporatingClump
-------------------------------------------------------------

Sets up a simulation of a single dense clump evaporated by an ionizing radiation field.


PE_MC_FN or PE_MC_FM
-------------------------------------------------------------

Sets up a simulation with a fixed number of random clumps or a fixed total mass in random clumps


BlastWave
-------------------------------------------------------------

Sets up a blastwave test problem


Advection
-------------------------------------------------------------

Sets up an advection test on a periodic domain


DoubleMachRef
-------------------------------------------------------------

Sets up the double Mach reflection test problem.



OrszagTang
-------------------------------------------------------------

Sets up the Orzsag-Tang Vortex test problem



LiskaWendroffImplosion
-------------------------------------------------------------

Sets up the Liska and Wendroff implosion test problem. 


KelvinHelmholtz
-------------------------------------------------------------

Sets up a KH instability test problem.


FieldLoop
-------------------------------------------------------------

Sets up the field-loop advection test.


DivBPeak
-------------------------------------------------------------

Sets up the test of how the code can deal with a peak in divB


Others
-------------------------------------------------------------

Not yet documented.