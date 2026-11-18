
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

=================================  =======================================  ==================================================
Problem (key value)                Location of source file                  Brief description
=================================  =======================================  ==================================================
``Uniform``                        ``basic_tests.cpp``                      Uniform medium, optionally with a radial profile
``ShockTube``                      ``shock_tube.cpp``                       Riemann problem: built-in tests or user-defined
                                                                            left and right states
``Advection``                      ``basic_tests.cpp``                      Advection of an overdense clump
``AdvectSineWave``                 ``basic_tests.cpp``                      Advection of an overdense clump in a sinusoidal
                                                                            velocity field
``DoubleMachRef``                  ``basic_tests.cpp``                      Double Mach reflection test
``FieldLoop``, ``FieldLoopVz``,    ``basic_tests.cpp``                      Advection of a magnetic field loop
``FieldLoopStatic``
``KelvinHelmholtz``,               ``basic_tests.cpp``                      Kelvin-Helmholtz instability tests
``KelvinHelmholtzStone``
``LiskaWendroffImplosion``         ``basic_tests.cpp``                      Liska & Wendroff implosion test
``OrszagTang``                     ``basic_tests.cpp``                      2D Orszag-Tang vortex
``TestCIE``, ``TestMC``            ``basic_tests.cpp``                      1D static tests of the cooling and microphysics
``BlastWave``, ``BlastWave_File``  ``blast_wave.cpp``                       Blast wave (e.g. supernova) in a uniform medium,
                                                                            or from a 1D profile in a file
``ShockCloud``                     ``shock_cloud.cpp``                      Shock-cloud interaction
``RadiativeShock``,                ``radiative_shock.cpp``                  1D or 2D radiative shock tests
``RadiativeShockOutflow``
``Clump_Spherical``,               ``spherical_clump.cpp``                  Clump at the origin, in 1D spherical or 2D
``Clump_Axisymmetric``                                                      cylindrical coordinates
``PhotoEvaporatingClump`` (and     ``photoevaporating_clump.cpp``           Photoevaporation of a dense clump
variants, see below)
``PhotEvap_RandomClumps``,         ``photoevaporating_random_clumps.cpp``   Photoevaporation of randomly placed clumps
``PERC``, ``PERC2``,
``PhotEvap_RandomClumps2``
``PE_MC_FN``, ``PE_MC_FM``         ``photoevaporating_multiclumps.cpp``     Photoevaporation of many random and/or placed
                                                                            clumps
``1Dto2D``                         ``read_1Dto2D.cpp``                      Read a 1D profile and map it onto a 2D or 3D
                                                                            grid
``Supernova``                      ``overwrite-snapshot.cpp``               Add supernova ejecta to an existing snapshot
``LaserAblationAxi``               ``laser_ablation.cpp``                   HEDP laser-ablation test
``Jet``                            ``jet.cpp``                              2D axisymmetric or 3D jet (untested)
=================================  =======================================  ==================================================

Each of these types of initial-condition generator has some distinct parameters that should be set in the parameter file (discussed in more detail below).
Parameters of the state vector that are not given (e.g. a tracer value) are set to -1.0e99, so make sure that all of them are in the parameter file.
Magnetic field values are given in Gauss, unless stated otherwise.

Two parameters are common to most of the generators:

+ ``noise``: if greater than zero, random noise with this fractional amplitude is added to the density and pressure (adiabatic noise, or isothermal for the ``PhotoEvaporatingClump`` types).  Not used by ``ShockTube``, ``PE_MC_*``, ``1Dto2D`` and ``Supernova``.
+ ``InitIons``: if ``EP_chemistry`` is 1, ``icgen`` normally integrates the ion fractions to equilibrium after setting up the grid (``InitIons YES``, the default).  Use ``InitIons LEAVE`` to keep the tracer values set by the generator.

Some other generators are only compiled with extra compile-time flags (``StarBench_*`` tests, ``ReadBBTurbulence``, ``HD_2D_ShockCloud``), and ``LaserAblation3D`` is not implemented.


=====================================================
General Parameters
=====================================================

Integration Scheme
-----------------------------------------------

These parameters set the coordinate system, the equations to be solved, and the numerical scheme.
Parameters marked *required* must be in the parameter file; for the others the default is used if they are missing.

=======================  ==================  ==================================================================
Parameter                Default             Description
=======================  ==================  ==================================================================
``ndim``                 required            Number of spatial dimensions: 1, 2 or 3.
``coordinates``          ``cartesian``       Coordinate system: ``cartesian`` (or ``cart``), ``cylindrical``
                                             (``cyl``) or ``spherical`` (``sph``).
``eqn``                  required            Equations to solve: ``euler`` (or ``hd``), ``mhd`` (or
                                             ``glm-mhd``), or ``i-mhd``; see below.
``GAMMA``                required            Adiabatic index of the gas (usually 5/3 = 1.66666666666667).
``solver``               ``4``               Flux solver, by number or name; see below.
``OrderOfAccSpace``      required            Spatial order of accuracy: 1 = piecewise constant; 2 = piecewise
                                             linear.
``OrderOfAccTime``       required            Temporal order of accuracy: 1 = first order; 2 = second order.
``CFL``                  required            Courant-Friedrichs-Lewy number (<1 in 1D, <0.5 in 2D, <0.33 in 3D).
``ArtificialViscosity``  required            0 = none; 1 = Falle, Komissarov & Joarder (1998) viscosity;
                                             3 = H-correction of Sanders et al. (1998), which only works with
                                             some solvers; 4 = both 1 and 3 (not well tested).
``EtaViscosity``         required for 1, 4   Viscosity parameter for ``ArtificialViscosity`` 1 and 4, in the
                                             range [0,1]; 0.15 is a typical value.
``glm_chyp_multiplier``  ``1.0``             GLM-MHD only: multiplier for the speed of the divergence-cleaning
                                             wave.
``glm_par_limiter``      ``0.3``             GLM-MHD only: parameter for the damping of the divergence-cleaning
                                             wave.
=======================  ==================  ==================================================================

There are three types of equations:

+ **Euler equations** (``euler``): the inviscid equations of hydrodynamics.
+ **GLM-MHD equations** (``mhd`` or ``glm-mhd``): ideal magnetohydrodynamics plus the Dedner et al. (2002) divergence-cleaning method for controlling divergence errors in the magnetic field.  Use this for MHD in 2D and 3D.
+ **Ideal MHD equations** (``i-mhd``): magnetohydrodynamics with no method for controlling divergence errors, so only usable in 1D.

The flux solver can be given by number or by name:

======  ====================  ==========  ==============================================================
Number  Name                  Equations   Description
======  ====================  ==========  ==============================================================
0       ``LF``                HD, MHD     Lax-Friedrichs (very diffusive)
1       ``linear``            HD, MHD     linear Riemann solver (not very robust)
2       ``exact``             HD          exact Riemann solver (slow)
3       ``hybrid``            HD          hybrid of the linear and exact Riemann solvers
4       ``roe``               HD, MHD     Roe solver in conserved variables
5       ``roepv``             HD          Roe solver in primitive variables (not recommended)
6       ``fvs``               HD          flux-vector splitting
7       ``hlld``              MHD         HLLD solver, switching to HLL where needed for stability
8       ``hll``               HD, MHD     HLL solver (diffusive but robust)
9       ``rcv-hll``           HD          Roe solver in conserved variables, switching to HLL where
                                          there are large density contrasts
======  ====================  ==========  ==============================================================

Notes:

+ The recommended solver for the Euler equations is 9, and for the MHD equations is 7.
+ GAMMA should be in the range (1,2).
+ Spatial and temporal order of accuracy may both be set to 1 or 2, but they should be the same as each other.  The 2nd-order scheme is better.
+ The CFL parameter for an unsplit scheme should be <1/ndim.
+ Cartesian coordinates are available in 1D, 2D and 3D, with all equations.
+ Cylindrical coordinates are only available in 2D :math:`(z,R)` with rotational symmetry in the :math:`\theta` direction.  The radial coordinate (``y`` axis) should begin at :math:`R=0` and use an axisymmetric boundary condition.
+ Spherical coordinates are only available in 1D (radial coordinate), with reflection symmetry at :math:`r=0`, and only for the Euler equations.


Computational Grid
-----------------------------------------------

The PION computational grid is a uniform rectilinear grid in 1, 2 or 3 spatial dimensions.
Grid cells must be cubes (squares in 2D): the range divided by the number of cells must be the same along each axis.
Static mesh-refinement (nested grids) is implemented for the case where each refinement level has the same shape and number of cells as the coarser level above it, but the spatial resolution is a factor of 2 higher and so the grid covers 1/2 of the domain of the coarser grid in each dimension.
The focus of the nested grid can be at the centre of the domain, the negative boundary or the positive boundary for each dimension.
The number of refinement levels is in principle unlimited, but snapshots can only be written every coarse timestep and this imposes a practical limit: for 10 grid levels there are 512 finest-level timesteps per coarsest-level step, and one may wish to save a snapshot at least this often.
The coarsest level is denoted ``level 0``, and each refined level has a higher level number.
For runs with MPI, see also the restrictions on nested grids in :ref:`parallel`.

The parameters are:

========================  ==================  ================  =================================================================
Parameter                 Type                Default           Description
========================  ==================  ================  =================================================================
``NGridX``                integer             required          Number of grid cells in :math:`\hat{x}`-direction
``NGridY``                integer             required (2D/3D)  Number of grid cells in :math:`\hat{y}`-direction
``NGridZ``                integer             required (3D)     Number of grid cells in :math:`\hat{z}`-direction
``Xmin``                  double              required          Negative boundary in :math:`\hat{x}`-direction (level 0), in cm
``Ymin``                  double              required (2D/3D)  Negative boundary in :math:`\hat{y}`-direction (level 0), in cm
``Zmin``                  double              required (3D)     Negative boundary in :math:`\hat{z}`-direction (level 0), in cm
``Xmax``                  double              required          Positive boundary in :math:`\hat{x}`-direction (level 0), in cm
``Ymax``                  double              required (2D/3D)  Positive boundary in :math:`\hat{y}`-direction (level 0), in cm
``Zmax``                  double              required (3D)     Positive boundary in :math:`\hat{z}`-direction (level 0), in cm
``grid_nlevels``          integer             1                 Number of grid levels for static mesh-refinement (>=1)
``NG_centre_XX``          double              0.0               Focus of nested-grid refinement in X (cm)
``NG_centre_YY``          double              0.0               Focus of nested-grid refinement in Y (cm)
``NG_centre_ZZ``          double              0.0               Focus of nested-grid refinement in Z (cm)
``NG_refine_XX``          integer             1                 Refine in X direction? (1 = yes)
``NG_refine_YY``          integer             1                 Refine in Y direction? (1 = yes)
``NG_refine_ZZ``          integer             1                 Refine in Z direction? (1 = yes)
========================  ==================  ================  =================================================================

The parameters ``grid_aspect_ratio_XX``, ``grid_aspect_ratio_YY`` and ``grid_aspect_ratio_ZZ`` are read and stored in snapshots, but they are not used.


.. _parameter-output:

Output and time control
-----------------------------------------------

====================  ==================  ================  =================================================================
Parameter             Type                Default           Description
====================  ==================  ================  =================================================================
``OutputPath``        string              required          Directory (or other prefix) put in front of ``OutputFile`` for the
                                                            snapshots written by ``pion``, e.g. ``./`` or ``/scratch/run1/``.
                                                            Include the trailing ``/``.
``OutputFile``        string              required          Base name of the snapshot files.  ``icgen`` writes the
                                                            initial conditions to this name (without ``OutputPath``).
``OutputFileType``    string              required          ``silo`` (recommended), ``fits`` (uniform grids only), ``text``,
                                                            or ``both`` (FITS and text).
``OutputFrequency``   integer             required          Write a snapshot every N timesteps (of level 0), if
                                                            ``OutputCriterion`` is 0.
``OutputCriterion``   integer             0                 0: write snapshots every ``OutputFrequency`` timesteps; 1: every
                                                            ``OPfreqTime`` seconds of simulation time.
``OPfreqTime``        double              0                 Time between snapshots (s), if ``OutputCriterion`` is 1.
``StartTime``         double              required          Simulation time of the initial conditions (s).
``FinishTime``        double              -1                Simulation time (s) at which to stop.  If negative, the run
                                                            continues until it is stopped by other means.
====================  ==================  ================  =================================================================

Snapshot file names have the form ``<OutputFile>_<step>.silo`` on uniform grids, and ``<OutputFile>_level<NN>_<step>.silo`` (one file per level) on nested grids, where ``<step>`` is the timestep number, written as ``0000.00000000``.
Parallel runs may write more than one file per level for large grids.
Every snapshot can be used to restart the simulation (see :ref:`command-line-restart`); there are no separate checkpoint files.
Most of these parameters can be changed when restarting a simulation with the command-line options of ``pion`` (see :ref:`command-line`), e.g. ``opfreq_time=``, ``finishtime=`` or ``outfile=``.


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


.. _parameter-tracers:

Tracers and microphysics
-----------------------------------------------

Passive tracers are extra variables that are advected with the gas.
They are used for the ion fractions and element abundances of the microphysics modules, and to follow e.g. the material from a stellar wind.

=================  ==================  ================  =================================================================
Parameter          Type                Default           Description
=================  ==================  ================  =================================================================
``ntracer``        integer             0                 Number of passive tracers.
``Tracer000``      string              required          Name of each tracer: ``Tracer000``, ``Tracer001``, ... up to
                                       (if ``ntracer``   ``ntracer`` - 1 (three digits).
                                       > 0)
``chem_code``      string              none              Microphysics module, e.g. ``MPv3`` or ``NEMO``; see
                                                         :ref:`ref-microphysics`.  Only used if ``EP_chemistry`` is 1.
``InitIons``       string              ``YES``           ``icgen`` only.  With ``EP_chemistry 1``, ``icgen`` normally sets
                                                         the ion fractions to their equilibrium values (``YES``); set
                                                         ``InitIons LEAVE`` to keep the values set by the initial
                                                         conditions.
=================  ==================  ================  =================================================================

The tracer names that each microphysics module expects are described in :ref:`ref-microphysics` (for NEMO, see :ref:`ref-nemo`).
The initial values of the tracers are set by the initial-conditions parameters, e.g. ``UNIFORM_ambTR[n]``, and the values in stellar winds by ``WIND_[i]_TR[n]``.


.. _parameter-extra-physics:

Extra physics
-----------------------------------------------

Parameters starting with ``EP_`` switch on extra physics.
All are optional.
Some of them can be overridden on the command line when (re)starting ``pion``, but see the warning in :ref:`command-line`.

==========================  ===========  ======================================================================
Parameter                   Default      Description
==========================  ===========  ======================================================================
``EP_dynamics``             1            Hydrodynamics on (1) or off (0, e.g. to evolve only the
                                         microphysics and radiation).
``EP_raytracing``           0            Ray-tracing of the radiation sources (1) or not (0).
``EP_chemistry``            0            Use the microphysics module set by ``chem_code`` (1) or not (0).
``EP_cooling``              0            Radiative cooling: 0 for none; with ``EP_chemistry 0`` this
                                         selects a cooling function of ``mp_only_cooling``, and for the
                                         microphysics modules it switches cooling on.  See
                                         :ref:`ref-microphysics`.
``EP_update_erg``           1            Heating and cooling change the gas energy (1), or not (0).
``EP_MP_timestep_limit``    1            How the microphysics limits the timestep: 0 = no limit;
                                         1 = cooling time; 2 = cooling and recombination times;
                                         3 = cooling, recombination and ionisation times (not
                                         working); 4 = recombination time; 5 = also with implicit
                                         ray-tracing (``RT_update 1``).  MPv3, MPv5, MPv6, MPv7 and
                                         MPv8 always use 1; NEMO uses its own criterion if the value
                                         is not 0.
``EP_Min_Temperature``      0            Minimum gas temperature (K).
``EP_Max_Temperature``      1.0e100      Maximum gas temperature (K).
``EP_Hydrogen_MassFrac``    0.7154       Hydrogen mass fraction X, used by some microphysics modules
                                         (default from Asplund et al. 2009).
``EP_Helium_MassFrac``      0.2703       Helium mass fraction Y, used by some microphysics modules.
``EP_Metal_MassFrac``       0.0142       Metal mass fraction Z, used by some microphysics modules.
``EP_coll_ionisation``      0            NEMO: collisional ionisation.
``EP_phot_ionisation``      0            NEMO: photoionisation (needs ``EP_raytracing 1``).
``EP_rad_recombination``    0            NEMO: radiative recombination.
``EP_charge_exchange``      0            NEMO: charge exchange (needs ``EP_coll_ionisation 1``).
``EP_cr_ionisation``        0            NEMO: cosmic-ray ionisation (needs ``EP_coll_ionisation 1``).
``EP_onthespot_radiation``  0            NEMO: on-the-spot approximation for the diffuse radiation.
``EP_nemo_reltol``          1.0e-6       NEMO: relative tolerance of the CVODE integration.
``EP_nemo_abstol``          1.0e-12      NEMO: absolute tolerance for the mass fractions.
``EP_nemo_minerg``          1.0e-20      NEMO: absolute tolerance for the internal energy.
``EP_sat_thermal_cond``     0            Saturated thermal conduction on (1) or off (0).
``EP_tc_strength``          1.0          Strength of thermal conduction, from 0 (none) to 1 (full).
``EP_compton_cool``         0            Compton cooling of the gas by the radiation of the stars with
                                         stellar winds (1) or not (0).
``EP_wind_acceleration``    0            Allow accelerating winds (``WIND_[i]_acceleration``): 1 = yes.
``EP_shocked_tracer``       0            Index of a tracer that is set to 1 in gas that has passed through a
                                         shock (0 = not used).  Needs ``EP_dust_tracer``.
``EP_dust_tracer``          0            Index of a tracer that is set to 1 in shocked gas that is cool and
                                         dense enough to form dust (0 = not used).  Needs
                                         ``EP_shocked_tracer``.
==========================  ===========  ======================================================================

The NEMO parameters are described in more detail in :ref:`ref-nemo`.


Units
-----------------------------------------------

PION uses CGS units, with magnetic field in Gauss.
Internally PION solves the MHD equations in units such that factors of :math:`4\pi` do not appear, but these are rescaled by :math:`\sqrt{4\pi}` on reading from and writing to snapshots, and so the snapshots have magnetic field units of Gauss.
The parameters ``units``, ``rhoval``, ``lenval``, ``velval`` and ``magval`` are read and stored in snapshots, but they have no effect; they can be left out.



=====================================================
Source terms
=====================================================


Radiation sources
--------------------

The parameter ``RT_Nsources`` is an integer setting the number of radiation sources (default 0).
Each source ``i``, numbered from 0, has parameters of the form ``RT_[name]_[i]`` that describe the radiation field emitted.
Ray-tracing must also be switched on with ``EP_raytracing 1``.

+------------------------+-----------+------------------------------------------------------------------------+
| parameter key          | value type| description                                                            |
+========================+===========+========================================================================+
| ``RT_position_[i]_[n]``| double    | Position of source along axis ``n`` (cm).  Value >1e100 (or <-1e100)   |
|                        |           | implies a source at infinity.  Required.                               |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_star_____[i]``    | integer   | Index of the star (see :ref:`parameter-stars`) that the source is      |
|                        |           | attached to; the source then takes its position from the star.         |
|                        |           | Default -1 (not attached).                                             |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_src_type_[i]``    | integer   | 1 = point source (tested), 2 = diffuse radiation (untested).  Required.|
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_at_infty_[i]``    | integer   | 1: source is at infinity, 0: point source obeying inverse square law.  |
|                        |           | Required.                                                              |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_update___[i]``    | integer   | 1: C2-ray update (Mellema et al. 2006), where the time integration     |
|                        |           | happens during the ray-trace; 2: Mackey (2012) update, where only the  |
|                        |           | column densities are calculated during the ray-trace.  2 is            |
|                        |           | recommended.  Required.                                                |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_effect___[i]``    | integer   | 1: UV heating, 4: monochromatic photoionizing field, 5: multifrequency |
|                        |           | photoionization; for NEMO, 7-10 select the stellar spectrum            |
|                        |           | (see :ref:`ref-nemo`).  Required.                                      |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Tau_var__[i]``    | integer   | Set to 0 (only used for a special diagnostic source type).  Required.  |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Nbins____[i]``    | integer   | Number of energy bins for the radiation field: 1, or 15 for NEMO.      |
|                        |           | Required.                                                              |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_EVO_FILE_[i]``    | string    | ``NOFILE`` for a source that does not change, or the name of a text    |
|                        |           | file with the time evolution of the source.  Required.                 |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_strength_[i]``    | double    | Luminosity in erg/s for a multi-frequency source,                      |
|                        |           | or in photons/s for a monochromatic source. For a source at infinity   |
|                        |           | this is the incident flux (erg/cm2/s or photons/cm2/s).                |
|                        |           | Not used for an evolving source.  Required.                            |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Rstar____[i]``    | double    | Stellar radius in ``R_sun``.  Not used for evolving source.            |
+------------------------+-----------+------------------------------------------------------------------------+
| ``RT_Tstar____[i]``    | double    | Stellar effective temperature in K. Not used for evolving source.      |
+------------------------+-----------+------------------------------------------------------------------------+

The microphysics module calculates the opacity of the gas for the radiation; the old parameter ``RT_Tau_src__[i]`` is no longer used.
For more than one ionising source, use MPv3, MPv5, MPv6, MPv7 or NEMO (see :ref:`ref-microphysics`).


.. _parameter-stars:

Stars
-------------

Stars are point masses that stellar-wind sources and radiation sources can be attached to (with ``WIND_[i]_star`` and ``RT_star_____[i]``).
Attached sources take their position from the star, so if the star moves, the sources move with it.
Moving stars are moved by an N-body integration of their mutual gravity.

+---------------------------+--------------------------------------------+-------------------------------------+
| Parameter                 |   Description                              |   Units/values                      |
+===========================+============================================+=====================================+
| ``STAR_Nstars``           | Number of stars                            | Default 0                           |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``STAR_[i]_pos[n]``       | Position of star ``i`` along axis ``n``    | cm (required)                       |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``STAR_[i]_velocity[n]``  | Velocity of star ``i`` along axis ``n``    | cm/s (default 0)                    |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``STAR_[i]_mass``         | Mass of the star                           | :math:`\mathrm{M_{\odot}}`          |
+---------------------------+--------------------------------------------+-------------------------------------+
| ``STAR_[i]_moving``       | Does the star move? (1 = yes, 0 = no)      | Default 0                           |
+---------------------------+--------------------------------------------+-------------------------------------+

Parameter files without ``STAR_`` parameters, but with a moving wind source (``WIND_[i]_moving_star 1``), still work: PION creates a star for each moving wind source.


Stellar Winds
-------------

The parameter ``WIND_NSRC`` is an integer setting the number of stellar-wind sources.
These are implemented as a spherical boundary region in which the fluid quantities are set according to the wind parameters for each source.

There are four types of stellar wind (``WIND_[i]_type``):

+ 0: spherically symmetric wind, constant in time;
+ 1: spherically symmetric wind that evolves in time, read from a file (``WIND_[i]_evofile``);
+ 2: latitude-dependent wind of a rotating star (Langer et al. 1999 model), evolving in time;
+ 3: latitude-dependent wind of a rotating star (modified version of the Langer et al. 1999 model), evolving in time.

The parameters, with the units they are given in, are:

+------------------------------+--------------------------------------------+-------------------------------------+
| Parameter                    |   Description                              |   Units/values                      |
+==============================+============================================+=====================================+
| ``WIND_[i]_pos[n]``          | Source position, :math:`n\in[0,1,2]`       | cm (required)                       |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_velocity[n]``     | Source velocity, :math:`n\in[0,1,2]`       | cm/s (default 0)                    |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_radius``          | Radius of wind injection region            | cm (required)                       |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_type``            | Type of wind (see above)                   | 0, 1, 2 or 3 (required)             |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_mdot``            | Mass-loss rate from star                   | :math:`\mathrm{M_{\odot}\,yr}^{-1}` |
|                              | :math:`\dot{M}`                            | (required)                          |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_vinf``            | Terminal velocity of wind                  | :math:`\mathrm{km\,s}^{-1}`         |
|                              |                                            | (required)                          |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_vrot``            | Equatorial rotation velocity               | :math:`\mathrm{km\,s}^{-1}`         |
|                              |                                            | (required)                          |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_temp``            | Effective temperature of the star; also    | K (required)                        |
|                              | the temperature of the injected wind       |                                     |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_Rstr``            | Radius of star                             | cm (required)                       |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_Bsrf``            | Strength of surface magnetic field         | Gauss (required)                    |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_TR[n]``           | Value of tracer :math:`n` in wind          | Usually :math:`\in [0,1]`           |
|                              |                                            | (required for each tracer)          |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_evofile``         | Text file with data for evolving wind      | Default is NOFILE                   |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_t_offset``        | Offset of simulation time from the time in | years                               |
|                              | the evolution file                         |                                     |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_t_scalefac``      | Factor to scale simulation time to the     | Default is 1.0                      |
|                              | time in the evolution file                 |                                     |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_updatefreq``      | How often to update an evolving wind       | years                               |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_enhance_mdot``    | Ad-hoc flag to increase :math:`\dot{M}`    | Default is 0                        |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_xi``              | Exponent of the latitude dependence of the | Default is -0.43                    |
|                              | wind (types 2 and 3)                       |                                     |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_star``            | Index of the star (see                     | Default is -1 (not attached)        |
|                              | :ref:`parameter-stars`) that the source is |                                     |
|                              | attached to                                |                                     |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_moving_star``     | Does the source move? 1 = yes, 0 = no.     | Default is 0                        |
|                              | Set from the star if the source is         |                                     |
|                              | attached to one.                           |                                     |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_mass``            | Mass of the star (only used if PION makes  | :math:`\mathrm{M_{\odot}}`          |
|                              | a star for a moving wind source)           |                                     |
+------------------------------+--------------------------------------------+-------------------------------------+
| ``WIND_[i]_acceleration``    | Is the wind accelerated? yes = 1, no = 0   | Default is 0 (inject at v_inf);     |
|                              |                                            | needs ``EP_wind_acceleration 1``    |
+------------------------------+--------------------------------------------+-------------------------------------+

The evolving wind file should have all units in CGS.


Jet inflow
----------------

.. warning::

   The jet module has not been used since 2010 and is untested.

There is a module for injecting protostellar jets along the symmetry axis in 2D cylindrical simulations, or along the positive x-axis in 3D Cartesian simulations, set up with ``ics Jet``.
The parameter ``N_JET`` takes an integer argument for the number of jet sources.
The module worked in 2010 when it was used for testing v.1.0 of PION, but has not been used since then.
If you want to know more about this, especially what extra parameters you need to include, take a look at ``source/ics/jet.cpp`` (and probably modify the source code for your needs).


=============================================================
Types of initial conditions that have been programmed
=============================================================


Uniform
-------------------------------------------------------------

This sets up uniform initial conditions throughout the domain in the standard mode, but it can also be used to set up a spherically symmetric medium with a constant-density core and a power-law density profile outside the core.
Parameters:

+-----------------------------+--------+----------------------------------------------------------------+
| parameter key               | type   | description                                                    |
+=============================+========+================================================================+
| ``UNIFORM_ambRO``           | double | gas density (g/cm3)                                            |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambPG``           | double | gas pressure (dyne/cm2)                                        |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambVX``           | double | x-velocity (cm/s)                                              |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambVY``           | double | y-velocity (cm/s)                                              |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambVZ``           | double | z-velocity (cm/s)                                              |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambBX``           | double | magnetic field BX (Gauss), MHD only                            |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambBY``           | double | magnetic field BY (Gauss), MHD only                            |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambBZ``           | double | magnetic field BZ (Gauss), MHD only                            |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_ambTR[N]``        | double | initial value of tracer ``N``                                  |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_radial_slope``    | double | If non-zero (and ``UNIFORM_core_radius`` is given), the        |
|                             |        | power-law slope of density and pressure with radius            |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_radius``     | double | radius of the constant-density core (cm)                       |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_smooth_core``     | integer| 1 (default): density and pressure are divided by               |
|                             |        | :math:`1+(r_c/r)^{s}`, a smooth core (use a negative slope     |
|                             |        | :math:`s`); 0: multiplied by :math:`(r/r_c)^{s}` outside the   |
|                             |        | core                                                           |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_radial_velocity`` | double | expansion/contraction velocity w.r.t. the core centre (cm/s),  |
|                             |        | where the radial profile is applied                            |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_centre_XX``  | double | x position of core centre (cm, default 0)                      |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_centre_YY``  | double | y position of core centre (cm, default 0)                      |
+-----------------------------+--------+----------------------------------------------------------------+
| ``UNIFORM_core_centre_ZZ``  | double | z position of core centre (cm, default 0)                      |
+-----------------------------+--------+----------------------------------------------------------------+


ShockTube
-------------------------------------------------------------

Sets up a Riemann problem in 1D or 2D (Cartesian or cylindrical coordinates), with a planar discontinuity.

+ ``STnumber``: number of a built-in test, or 0 to use the states given in the parameter file.  The built-in tests are: 1-5, the tests in Toro's book on Riemann solvers; 6, a near-stationary contact discontinuity; 7-13, MHD tests of Falle et al. (1998), including the Brio & Wu test (7) and an Alfvén wave (8); 15-26, the MHD tests 1a to 5b of Ryu & Jones (1995).  The built-in tests switch off ray-tracing, cooling and chemistry.
+ ``STshockpos``: position of the discontinuity along x (cm), for ``STnumber 0``.
+ ``STangleXY``: angle of the discontinuity in the x-y plane (integer, degrees, 2D only).
+ For ``STnumber 0``, the state on the left (negative x) side is given by ``STpostvecRO``, ``STpostvecPG``, ``STpostvecVX``, ``STpostvecVY``, ``STpostvecVZ`` (and ``STpostvecBX``, ``STpostvecBY``, ``STpostvecBZ`` for MHD), and the state on the right side by ``STprevec...``.  Magnetic field values are in code units (magnetic pressure :math:`B^2/2`), not Gauss.
+ Tracers: ``STprevecTR[n]`` and ``STpostvecTR[n]``.


ShockCloud
-------------------------------------------------------------

Sets up a planar shock (at 1/8 of the x range) that will hit a spherical cloud, in 2D or 3D Cartesian or 2D cylindrical coordinates.

+ ``SCcloudradius``: radius of the cloud, as a fraction of the y range.
+ ``SCdratio``: density of the cloud relative to the pre-shock medium.
+ ``SCpratio``, ``SCBratio``: pressure and (x-component of) magnetic field of the cloud relative to the pre-shock medium (default 1).
+ ``SCprevecRO``, ``SCprevecPG``, ``SCprevecVX``, ... and ``SCpostvecRO``, ...: pre-shock and post-shock states, as for ``ShockTube``, including ``BX``, ``BY``, ``BZ`` (Gauss) for MHD and ``TR[n]`` for the tracers.
+ ``SCcloudTR0``, ``SCcloudTR01``: values of tracers 0 and 1 in the cloud (note the name of the second one).


RadiativeShock
-------------------------------------------------------------

Sets up a radiative shock test.

+ ``RadiativeShock``: the whole grid flows towards negative x at speed ``RADSH_vs`` (cm/s), with density ``RADSH_r0`` (g/cm3) and temperature ``RADSH_T0`` (K), and hits a reflecting boundary.  Tracers are set with ``RADSH_upTR[n]``.
+ ``RadiativeShockOutflow``: the same inflow, plus a dense, static post-shock layer at small x.  Tracers are set with ``RADSH_upTR[n]`` and ``RADSH_dnTR[n]``; for MHD, the density of the layer is divided by ``RADSH_divisor`` (default 3).
+ ``RADSH_B0``: magnetic field (Gauss) for MHD (default 0).


BlastWave
-------------------------------------------------------------

Sets up a blast wave of energy ``BW_energy`` (erg, default :math:`10^{51}`) in a uniform medium, centred at the origin: in 1D spherical, 2D cylindrical, or 2D or 3D Cartesian coordinates.

+ ``BW_nzones``: radius of the region where the energy is injected, in grid cells.
+ ``BWdensity``, ``BWpressure``: density and pressure of the ambient medium.
+ ``BW_blast_dens``: density in the injection region.
+ ``BWmagfieldX``, ``BWmagfieldY``, ``BWmagfieldZ``: magnetic field (Gauss, default 0).
+ ``BW_amb_TR[n]``: tracer values (1D spherical).
+ 1D spherical only: a second ambient medium outside radius ``BW_interface`` (cm), with ``BW_amb2_RO``, ``BW_amb2_PG``, ``BW_amb2_VX``, ... and ``BW_amb2_TR[n]``.

``BlastWave_File`` (1D spherical only) reads the ambient medium from a text file ``BWfilename`` with one line per grid cell (radius, the fluid variables, and the H\ :sup:`+` tracer, whose index is ``BW_Hplus_tracer``), and then adds the blast wave.


Clump_Spherical and Clump_Axisymmetric
-------------------------------------------------------------

Sets up a clump centred at the origin, in 1D spherical coordinates (``Clump_Spherical``) or 2D cylindrical coordinates (``Clump_Axisymmetric``), in a uniform, static medium.

+ ``AMB_density``, ``AMB_pressure``: density and pressure of the ambient medium.
+ ``SC_radius``: radius of the clump (cm): the radius of a top-hat clump, the core radius, or the Gaussian :math:`\sigma`.
+ ``SC_overdensity``: overdensity of the clump.
+ ``SC_density_profile``: 0 = top-hat; 1 = :math:`\rho\propto1/(1+(r/r_c)^2)`; 2 = Gaussian.
+ ``SC_pressure``: 1 = isothermal (pressure proportional to density); 2 = constant pressure.
+ ``SC_magfieldX``, ``SC_magfieldY``, ``SC_magfieldZ``: magnetic field (Gauss, default 0).


PhotoEvaporatingClump
-------------------------------------------------------------

Sets up a simulation of a dense clump evaporated by an ionizing radiation field.
Parameters for all variants:

+ ``PEC_ambRO``, ``PEC_ambPG``, ``PEC_ambVX``, ``PEC_ambVY``, ``PEC_ambVZ`` (and ``PEC_ambBX``, ``PEC_ambBY``, ``PEC_ambBZ`` for MHD), ``PEC_ambTR[n]``: ambient medium.
+ ``PEC_xpos``, ``PEC_ypos``, ``PEC_zpos``: position of the clump (cm).
+ ``PECcloudradius``: radius of the clump, as a fraction of the y range (x range in 1D).
+ ``PECdratio``, ``PECpratio``, ``PECBratio``: density, pressure and magnetic field of the clump relative to the ambient medium (``PECpratio`` and ``PECBratio`` default to 1).
+ ``PECcloudTR[n]``: tracer values in the clump.

Variants:

+ ``PhotoEvaporatingClump``: a single spherical clump.
+ ``PhotoEvaporatingClump2``: two clumps, as in model 3 of Lim & Mellema (2003).
+ ``PhotoEvap_radial``: no clump, but a medium with density and pressure decreasing as :math:`(r_c/r)^{s}` with distance from the clump position, outside :math:`r_c` = ``PECcloudradius``, with slope ``PEC_radialslope``.
+ ``PhotoEvap_CloudClump``: a clump in a cloud with density profile :math:`1/(1+(r/r_0)^s)` around the origin, with :math:`r_0` = ``PEC_core_radius`` (fraction of the y range) and :math:`s` = ``PEC_radialslope``.
+ ``PhotoEvap_powerlaw`` and ``PhotoEvap_paralleltest``: special setups for particular tests.


PhotEvap_RandomClumps
-------------------------------------------------------------

Sets up a simulation with randomly placed clumps (with Gaussian profiles) in a uniform ISM, in Cartesian coordinates.

+ ``PERC_ambRO``, ``PERC_ambPG``, ... , ``PERC_ambTR[n]``: ambient medium.
+ ``PERCnumclumps``: number of clumps.
+ ``PERCclump_profile``: must be 1 (Gaussian).
+ ``PERCmin_size``, ``PERCmax_size``: size range of the clumps, as fractions of the y range.
+ ``PERCrandomseed``: seed for the random numbers.
+ ``PERCcloudTR[n]``: tracer values in the clumps.
+ ``PhotEvap_RandomClumps`` or ``PERC``: clumps with random overdensities between ``PERCmin_overdensity`` and ``PERCmax_overdensity``.
+ ``PhotEvap_RandomClumps2`` or ``PERC2``: a fixed total clump mass ``PERCclump_mass`` (fraction of the ambient mass), with clump masses between ``PERCmin_mass`` and ``PERCmax_mass``; ``PERCempty_xn``, ``PERCempty_xp``, etc. give regions near the boundaries without clumps.


PE_MC_FN or PE_MC_FM
-------------------------------------------------------------

Sets up a simulation with random clumps and/or clumps at given positions, in an ambient medium that can have a radial or exponential density profile.
The names ``PE_MC_FN``, ``PE_MC_FM``, ``PhotEvap_MultiClumps_FixNum`` and ``PhotEvap_MultiClumps_FixMass`` all set up the same problem; whether there is a fixed number of clumps or a fixed mass in clumps is set by ``PERC_selection_criterion`` (``Fixed_Number`` or ``Fixed_Mass_Range``).

+ ``PERC_ambRO``, ``PERC_ambPG``, ... , ``PERC_ambTR[n]``: ambient medium; optionally a second medium for :math:`x<` ``PERC_ALTdivider`` with ``PERC_ALTambRO``, etc.
+ ``PERCradialslope``, ``PERCcloudradius``: power-law density profile outside a core radius (fraction of the y range), around the origin.
+ ``PERC_amb_xscale true``, with ``PERC_xscale_x0``, ``PERC_xscale_l0`` and ``PERC_xscale_alpha``: density and pressure decreasing as :math:`\exp(-(|x-x_0|/l_0)^\alpha)`.
+ ``PERC_addrandomclumps YES``: add random clumps, with total mass ``PERC_mean_clump_density`` times the volume of the domain, ``PERCrandomseed``, ``PERCclump_profile`` (0 = top-hat, 1 = Gaussian), ``PERCmin_size``, ``PERCmax_size``, ``PERCmin_mass``, ``PERCmax_mass`` or ``PERCnumclumps``, ``PERCempty_xn``, etc., and ``PERCcloudTR[n]``.
+ ``PE_SC_addstrategicclumps YES``: add ``PE_SC_numclumps`` clumps at given positions, with profile ``PE_SC_clump_profile``, and for each clump ``c``: ``PE_SC[c]_overdensity``, ``PE_SC[c]_xpos``, ``PE_SC[c]_ypos``, ``PE_SC[c]_zpos`` (relative to ``Xmin``, as fractions of the y range), ``PE_SC[c]_radius`` (fraction of the y range), ``PE_SC[c]_vx``, ``PE_SC[c]_vy``, ``PE_SC[c]_vz`` and ``PE_SC[c]_cloudTR[n]``.


Advection
-------------------------------------------------------------

Sets up an advection test of an overdense clump (density 10, radius 0.1 of the x range) in a medium with density 1 and pressure 1, in 2D or 3D, with speed 1 in the direction given by the angles ``NDadv_thetaXY`` and ``NDadv_thetaXZ`` (degrees).
``AdvectSineWave`` is a variant with a sinusoidal y-velocity, and no parameters.


DoubleMachRef
-------------------------------------------------------------

Sets up the double Mach reflection test problem (2D, Euler equations).
The parameters ``DMRmach`` and ``DMRtheta`` must be present, but the test always uses Mach 10 and an angle of 60 degrees.


OrszagTang
-------------------------------------------------------------

Sets up the Orszag-Tang vortex test problem (2D, MHD).
Parameters: ``OTVbeta`` (plasma beta) and ``OTVmach`` (Mach number).


LiskaWendroffImplosion
-------------------------------------------------------------

Sets up the Liska and Wendroff (2003) implosion test problem (2D).  No parameters.


KelvinHelmholtz
-------------------------------------------------------------

Sets up a Kelvin-Helmholtz instability test problem (2D): ``KelvinHelmholtz`` is the test of Frank et al. (1996), and ``KelvinHelmholtzStone`` the version of J. Stone (Athena test suite).  No parameters.


FieldLoop
-------------------------------------------------------------

Sets up the field-loop advection test (2D, MHD).  ``FieldLoopVz`` adds a velocity in the z direction, and ``FieldLoopStatic`` has no velocity.  No parameters.


TestCIE and TestMC
-------------------------------------------------------------

1D tests of the microphysics, with the hydrodynamics switched off: ``TestCIE`` sets up a temperature that increases along the grid (:math:`10^{1.5}` to :math:`10^9` K), and ``TestMC`` a gas at :math:`10^8` K that cools.
Parameters: ``UNIFORM_ambRO`` and ``UNIFORM_ambTR[n]``.


1Dto2D
-------------------------------------------------------------

Reads a 1D spherically symmetric profile from the text file ``1D_InputFile`` (one line per radius: the radius, then the primitive variables and tracers), and maps it onto a 2D cylindrical or 3D Cartesian grid, centred at the origin.


Supernova
-------------------------------------------------------------

Reads an existing snapshot (``SN-snapshot-file``) and adds the ejecta of a supernova at the origin, with the two-component density profile of Whalen et al. (2008), on the finest grid level.
Only in parallel (MPI) builds of ``icgen``.

+ ``SN-radius-Ncells``: radius of the ejecta, in grid cells of the finest level.
+ ``SN-energy``: kinetic energy of the ejecta (erg).
+ ``SN-ejecta-mass``: mass of the ejecta (g).
+ ``SN-ejecta-H-frac``, ``SN-ejecta-He-frac``: hydrogen and helium mass fractions of the ejecta.

The ejecta tracers are found by name (``X_H``, ``X_He``, ``H1+``, ``WIND``, etc.).


LaserAblationAxi
-------------------------------------------------------------

Sets up a laser-ablation test for high-energy-density physics, in 2D cylindrical coordinates.
Parameters: ``LASERAB_vel0``, ``LASERAB_rho0``, ``LASERAB_Dratio``, ``LASERAB_Pressure0``, ``LASERAB_Pratio``, ``LASERAB_BX0``.


Jet
-------------------------------------------------------------

Sets up the ambient medium for a jet simulation (see `Jet inflow`_), with ``JETambRO``, ``JETambPG``, ``JETambVX``, ``JETambVY``, ``JETambVZ``, ``JETambBX``, ``JETambBY``, ``JETambBZ`` and ``JETambTR[n]``.
The jet itself is set by ``JETradius`` (in grid cells), ``JETdensity``, ``JETpressure``, ``JETvelocity``, ``JET_Bax``, ``JET_Btor`` and ``JETjetTR[n]``.
