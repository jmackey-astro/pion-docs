.. _reference-guide:

*********************************
Reference Guide for PION modules
*********************************

.. _ref-microphysics:

Microphysics modules for PION
================================

Microphysics refers to anything happening on the atomic scale relating
to the composition and ionization state of the gas. Examples include
when calculating the mean mass per particle, the gas temperature, the
heating and cooling rates, and chemical kinetics.

PION implements a number of different microphysics classes, starting
from simple heating and cooling prescriptions such as the collisional
ionization equilibrium cooling curves of Sutherland & Dopita (1993),
Wiersma et al. (2009), or the simplified model of Koyama & Inutsuka
(2002). More complicated descriptions are also implemented, focussing
especially on the non-equilibrium ionization of hydrogen.

-  mp_only_cooling
-  MPv0 (LEGACY)
-  MPv1 (LEGACY)
-  MPv2 (LEGACY)
-  MPv3
-  MPv4 (LEGACY)
-  MPv5
-  MPv6
-  MPv7
-  MPv8 (LEGACY)
-  MPv9

Legacy code should not be used because it is no longer maintained, and
could be broken by newer code. Also, it is legacy code because it has
been superseded by better microphysics modules.

--------------

The microphysics class to be used is controlled by the parameter
chem_code. Possible options are:

::

   chem_code NONE
   chem_code MPv3
   chem_code MPv5
   chem_code MPv6
   chem_code MPv7
   chem_code MPv9

--------------

Production Modules
--------------------------

mp_only_cooling
^^^^^^^^^^^^^^^^^^

Despite its name, this class includes radiative heating as well as
cooling when some options are selected. It approximates the radiative
cooling properties of the gas as being a function of temperature and
density only (ignoring chemical composition and ionization state) and
uses tabulated functions to get a cooling rate. The same goes for the
heating rate, although it is usually an analytic function. A number of
schemes have been coded controlled by the runtime paramters

::

   chem_code  NONE
   EP_cooling [INTEGER>0]
   EP_chemistry 0

This tells PION not to use a chemistry module, but to include a
radiative heating and cooling module. The integer value tells PION which
heating and cooling routines to use:

-  **Case 1**: Non-equilibrium ionization (NEQ) tabulated cooling curve
   by `Sutherland & Dopita
   (1993) <https://ui.adsabs.harvard.edu/#abs/1993ApJS...88..253S/abstract>`__,
   from file pk6ff75.neq.
-  **Case 2**: Heating and cooling according to the analytic
   prescription by `Koyama & Inutsuka
   (2002) <https://ui.adsabs.harvard.edu/#abs/2002ApJ...564L..97K/abstract>`__,
   with typo corrections following `Vasquez-Semadeni et al.
   (2007) <https://ui.adsabs.harvard.edu/#abs/2007ApJ...657..870V/abstract>`__.
-  **Case 3**: Collisional ionization equilibrium (CIE) tabulated
   cooling curve by `Dalgarno & McCray
   (1972) <https://ui.adsabs.harvard.edu/#abs/1972ARA%26A..10..375D/abstract>`__.
-  **Case 4**: CIE tabulated cooling curve by `Sutherland & Dopita
   (1993) <https://ui.adsabs.harvard.edu/#abs/1993ApJS...88..253S/abstract>`__,
   from file m-00.cie.
-  **Case 5**: As case 4, but including photoionization heating assuming
   hydrogen is kept fully ionized, so that every recombination is
   followed instantly by a photoionization that inputs 5 eV of thermal
   energy to the plasma. This is appropriate for an HII region ionized
   by an O star, except that the CIE cooling function has the wrong
   cooling rates at about 10000 K (for HII regions) because it doesn’t
   assume H,C,N,O are photoionized.
-  **Case 6**: Similar to case 5, except that it uses a newer CIE
   cooling function by `Wiersma et al.
   (2009) <https://ui.adsabs.harvard.edu/#abs/2009MNRAS.393...99W/abstract>`__.
   This has weaker cooling than SD93 by about a factor of 2 at the peak
   of the cooling curve, and has the same problems with consistency as
   case 5.
-  **Case 7**: Similar to case 4, except that it uses a newer CIE
   cooling function by `Wiersma et al.
   (2009) <https://ui.adsabs.harvard.edu/#abs/2009MNRAS.393...99W/abstract>`__.
-  **Case 8**: Same as case 6, but with additional cooling associated
   with a photoionized plasma at temperatures of about 5000-20,000 K,
   assuming that H and He are always ionized at any temperature. It uses
   the CIE cooling from metals only (subtracting out the H and He
   cooling) from `Wiersma et al.
   (2009) <https://ui.adsabs.harvard.edu/#abs/2009MNRAS.393...99W/abstract>`__,
   adds thermal bremsstahlung from H and He, and forbidden line cooling
   from photoionized C,N,O from `Henney et al.
   (2009) <https://ui.adsabs.harvard.edu/#abs/2009MNRAS.398..157H/abstract>`__.
   To avoid double counting, we take the maximum of the WSS09 metal-line
   cooling rate and the Henney et al. forbidden-line cooling rate.

Cases 5 and 6 should not be used because they are inconsistent and
better alternatives are available (cases 7 and 8).

**Publications**

- Cases 7 and 8 were used in `Mackey et al. (2012) <https://ui.adsabs.harvard.edu/#abs/2012ApJ...751L..10M/abstract>`__ to study bow shocks around Betelgeuse.
- Case 8 was used by `Green et al. (2019) <https://ui.adsabs.harvard.edu/abs/2019A%26A...625A...4G/abstract>`__ for simulating the bow shock nebula `NGC 7635 <https://en.wikipedia.org/wiki/NGC_7635>`__.


MPv3
^^^^^^^^^^^^^^^^^^

This class uses an integration scheme such that photon transmission
through the cell (calculated via raytracing) does not have to be
integrated with the rate and energy equations. The ionization fraction
of hydrogen is followed in a non-equilibrium time-integration, including
photo-ionization, collisional ionization, and radiative recombination.
The ionization state of He,C,N,O are assumed to follow H (although there
is a compile-time flag to keep He neutral if needed), so that the
electron fraction can include electrons from He, and so that
forbidden-line cooling from C,N,O can be calculated in a
semi-self-consistent way.

-  It uses multi-frequency photoionisation including spectral hardening
   with optical depth, using the method outlined in Frank & Mellema
   (1994,A&A,289,937), and updated by Mellema et al. (2006,NewA,11,374).
-  It uses the Hummer (1994,MN,268,109) rates for radiative
   recombination and its associated cooling, and Bremsstrahlung cooling.
-  For collisional ionisation the function of Voronov (1997,ADNDT,65,1)
   is used.
-  Collisional excitation of neutral H, table from Raga, Mellema,
   Lundqvist (1997,ApJS,109,517), using data from Aggarwal (1993).
-  Various formulae from Henney et al. (2009,MN,398,157) are used for
   cooling due to collisional excitation of photoionised O,C,N (eq.A9),
   collisional excitation of neutral metals (eq.A10), and the Wiersma et
   al (2009) CIE metals-only curve.
   The max of the Wiersma et al. curve and the Henney et al. functions
   is taken, to avoid double counting. For neutral gas heating and
   cooling rates from Wolfire et al. (2003) are used.
-  Photoheating from ionisation is discussed above. Cosmic ray heating
   will use a standard value, X-ray heating is ignored. UV heating due
   to the interstellar radiation field (ISRF) uses a standard value
   (unattenuated), using e.g. Henney et al. eq.A3, if requested. UV
   heating from the star uses the same equation, but with the optical
   depth from the source (using a total H-nucleon column density) used
   to attenuate the radiation.

The integration method uses the CVODES solver from the SUNDIALS package
by (Cohen, S. D., & Hindmarsh, A. C. 1996, Computers in Physics, 10,
138) available from https://computation.llnl.gov/ The method chosen is
backwards differencing (i.e. implicit) with Newton iteration.

It requires the following run-time parameters:

::

   chem_code     MPv3
   ntracer       1   # or >1 if there are other tracer variables needed
   Tracer000     H1+ # This allocates tracer 0 to the ionized H fraction

   EP_raytracing         1
   EP_phot_ionisation    1
   EP_cooling            1
   EP_chemistry          1
   EP_coll_ionisation    1
   EP_rad_recombination  1
   EP_update_erg         1
   EP_MP_timestep_limit  1 
   EP_Min_Temperature    50.0  # or some appropriate value
   EP_Max_Temperature    1.0e9 # or some appropriate value
   EP_Hydrogen_MassFrac  0.73  # or some appropriate value
   EP_Helium_MassFrac    0.27  # or some appropriate value
   EP_Metal_MassFrac     0.014 # or some appropriate value

**Publications**

This class was used:

-  for studying HII regions in `Mackey et al.
   (2013) <http://adsabs.harvard.edu/abs/2013MNRAS.436..859M>`__;
-  for studying shells around supergiants in `Szecsi et al.
   (2018) <http://adsabs.harvard.edu/abs/2017arXiv171104007S>`__.


MPv5
^^^^^^^^^^^^^^^^^^

This class is for modelling photoevaporation of dense molecular clouds.
The file inherits from MPv3, and instead of the Wolfire et al. (2003)
neutral gas heating/cooling rates, it uses the Henney et al. (2009)
molecular cooling rate and heating rates. So the only significant
difference is in the treatment of non-ionized gas as being molecular
instead of atomic.

It requires the same run-time parameters as MPv3, except we should set:

::

   chem_code     MPv5

**Publications**

This module was used for studying stellar wind bubbles within HII
regions around massive stars in:

-  `Mackey, Gvaramadze et al.
   (2015) <http://adsabs.harvard.edu/abs/2015A%26A...573A..10M%7D>`__,
-  `Mackey, Castro et al.
   (2015) <https://ui.adsabs.harvard.edu/#abs/2015A&A...582A..24M/abstract>`__,
-  `Mackey, Haworth et al.
   (2016) <http://adsabs.harvard.edu/abs/2015arXiv151206857M>`__, and
-  `Gvaramadze, Mackey et al.
   (2017) <http://adsabs.harvard.edu/abs/2017MNRAS.466.1857G>`__.

MPv6
^^^^^^^^^^^^^^^^^^

This class is for running the cosmological radiative transfer test
problems I `(Iliev et al.,2006,MNRAS,371,1057) <https://ui.adsabs.harvard.edu/#abs/2006MNRAS.371.1057I/abstract>`__
and II `(Iliev et al.,2009,MNRAS,400,1283) <https://ui.adsabs.harvard.edu/#abs/2009MNRAS.400.1283I/abstract>`__.
It has been used for testing the ionizing radiative transfer code
modules of PION.

The file inherits from mpv3, and instead of the Wolfire et al. (2003)
and Henney et al. (2009) heating/cooling rates, it uses pure atomic
hydrogen heating/cooling, and also sets the He and metal abundances to
zero (hardcoded) regardless of the settings in the parameterfile.


MPv7
^^^^^^^^^^^^^^^^^^

This class is for running calculations with a simple two-temperature
isothermal equation of state, where T is a constant low value when gas
is neutral, and a constant high value when ionised, and linearly
interpolated for partial ionisation.

The file inherits from mpv3, and instead of the Wolfire et al. (2003)
and Henney et al. (2009) heating/cooling rates, it sets the temperature
based on Hydrogen ion fraction. This is appropriate for dense and
metal-rich gas, where thermal equilibrium is a good approximation.

**Publications**

-  This class was used in `Mackey, Mohamed et al.
   (2014) <http://adsabs.harvard.edu/abs/2014Natur.512..282M>`__ for
   studying shells around red supergiants.
-  Also used for the `StarBench project (Bisbas et al., 2015) <https://ui.adsabs.harvard.edu/#abs/2015MNRAS.453.1324B/abstract>`__, and related work.

MPv9
^^^^^^^^^^^^^^^^^^


Microphysics class for low metallicity gas at high redshift, as
described in `Dhanoa, Mackey & Yates
(2014) <http://adsabs.harvard.edu/abs/2014MNRAS.444.2085D>`__. This was
used for studying the chemistry and thermodynamics of metal-free gas in
the early universe, following the non-equilibrium chemistry of hydrogen,
helium and deuterium.

**Publications**

-  `Dhanoa, Mackey & Yates
   (2014) <http://adsabs.harvard.edu/abs/2014MNRAS.444.2085D>`__


Legacy Modules
------------------

MPv0
^^^^^^^^^^^^^^^^^^

This is the original microphysics class written for PION by J.Mackey,
but it had some problems (speed, reliability) and was never used for
research resulting in publications. Will probably be removed in future
versions of PION. It includes non-equilibrium collisional ionization of
a number of ions.

MPv1
^^^^^^^^^^^^^^^^^^

This microphysics class was used in Mackey & Lim
(`2010 <http://adsabs.harvard.edu/abs/2010MNRAS.403..714M>`__,
`2011 <http://adsabs.harvard.edu/abs/2011MNRAS.412.2079M>`__) for
studying the expansion of HII regions and the formation of pillars,
globules, elephant trunks at the borders of HII regions. It uses an
implementation of the C2-ray algorithm developed by G.Mellema and
collaborators (Mellema et al., 2006), but with significant
modifications. It has since been superseded by updated algorithms that
scale better on supercomputers (see `Mackey, 2012,
A&A <http://adsabs.harvard.edu/abs/2012A&A...539A.147M>`__).

MPv2
^^^^^^^^^^^^^^^^^^

This class is broken and can no longer be used. It was a fore-runner of
MPv3, was never used for production work, and has been replaced by MPv3
and MPv5.

MPv4
^^^^^^^^^^^^^^^^^^

This class updates the implicit (C2-ray type) integrator of MPv1. It
uses the same implicit raytracing scheme, so photon transmission through
the cell is integrated with the rate and energy equations to obtain a
time-averaged value. The main advantage here is that we can use
multi-frequency photoionisation rates which depend on the optical depth.
It was used in `Mackey, (2012,
A&A) <http://adsabs.harvard.edu/abs/2012A&A...539A.147M>`__ but found to
be less efficient than MPv3.

MPv8
^^^^^^^^^^^^^^^^^^

This class is for running calculations with a simple heating and cooling
prescription for photoionization calculations in the StarBench Workshop
test problems. The class inherits from MPv3, and instead of the Wolfire
et al. (2003) and Henney et al. (2009) heating/cooling rates, it uses a
much simpler prescription based on non-equilibrium heating and an
ad-hoc, density-dependent cooling timescale. It offers few advantages
over MPv7 and is considered to be legacy code.


.. _ref-boundary-conditions:

Boundary Conditions
========================

Boundary conditions are a key component of any grid-based code for fluid
dynamics. PION implements a number of boundary conditions, listed below.
Many of them are used exclusively for certain test problems and are not
for general use (e.g. DMR and DMR2 are only for the Double Mach
Reflection test problem).

External Boundary Conditions (governing flow onto or off the grid
boundary):

-  periodic
-  outflow or zero-gradient
-  one-way-outflow
-  inflow
-  reflecting
-  fixed
-  equator-reflect
-  DMR
-  SB1

Internal Boundary Conditions (governing sources or sinks of mass/energy)

-  stellar-wind
-  jet
-  DMR2
-  RadShock
-  RadShock2


General-purpose boundary conditions
------------------------------------------------------

Periodic boundaries: “periodic”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Along a given dimension, both boundaries of the grid can be set to have
the solution be periodic, using the boundary type “periodic”. This means
that there are an infinite number of copies of the domain along this
coordinate dimension or, alternatively, that gas flowing out of one side
of the domain will wrap around and flow back into the other side.

Zero-gradient boundaries: “outflow” or “zero-gradient”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This assumes that all flow quantities have zero gradient moving off the
domain, so that each boundary cell will have the same state vector as
the edge cell.

An exception is the for the Psi field of the GLM divergence cleaning
algorithm, for which the first layer of boundary cells have a value
corresponding to the first layer of grid cells multiplied by -1, and the
second layer corresponding to the second layer of grid cells multiplied
by -1.

Absorbing/outflow boundaries: “one-way-outflow”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is the same as the zero-gradient boundary, except that it does not
allow boundary cells to have velocities corresponding to inflow. It is
sometimes called a “diode” boundary because it allows gas to flow freely
only in one direction. In PION the normal component of velocity is set
to zero if a zero-gradient boundary would have inflowing gas (another
option is to switch to reflecting, but this is not done in PION).

The Psi field of the GLM divergence cleaning algorithm is treated as in
zero-gradient boundaries

Reflecting boundaries: “reflecting”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here the boundary is treated as a reflecting wall, so that the normal
component of velocity is a reflection of the grid values, but with the
sign reversed. The boundary is assumed to allow slipping, so the
tangential velocity of the boundary cells is a reflection of the grid
values with the sign unchanged.

For the magnetic field components, the same rules apply as to the
velocity, i.e. a normal magnetic field coming out of the reflecting wall
is not allowed. To allow a normal component, use the “equator-reflect”
boundaries instead.


Axisymmetric boundaries: “axisymmetric”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These only work for 2D cylindrically symmetric calculations in the :math:`R-z` plane.
They are the same as reflecting boundaries except that the third :math:`\theta` component of the velocity and magnetic field change sign across :math:`R=0`, to account for the rotational symmetry.
This has no effect on the flow dynamics for hydrodynamics because the flux across the symmetry axis is zero by definition, but is important for the calculation of :math:`\nabla \cdot\mathbf{B}` for divergence cleaning in MHD simulations.

Inflow boundaries: “inflow”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here a certain inflow state is imposed all along the boundary and this
remains fixed for the whole simulation.

Fixed boundaries: “fixed”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is basically the same as the inflow boundary, except that the
velocity of the gas can be zero, or corresponding to outflow.

Stellar wind boundary: “stellar-wind”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is an internal boundary condition, setting up a radially expanding
wind within a certain radius of a point, where a star exists. It is a
mass, momentum, and energy source on the grid, and it works in 1D, 2D,
and 3D. It is switched on by setting the parameter WIND_NSRC to an
integer value greater than zero.

2D simulations in Cartesian geometry assume a line source and not a point
source.  This means the density falls off as 1/r, so such a simulation is only
suitable for testing code.

For moving stars, a file "trajectory.txt" is created in the current working
directory and the trajectory of the stars is saved there, with a line being
written every 10 times the star's position is updated.  When a simulation is
stopped and restarted, this file is appended to, and a blank line followed by
the header line is inserted to indicate that the simulation was restarted.


Example parameters for a wind source:

::

   WIND_NSRC 1
   WIND_0_pos0  0.0e18     # cm
   WIND_0_pos1  0.0e18     # cm
   WIND_0_pos2  0.0e18     # cm
   WIND_0_velocity0          0.0 # cm/s
   WIND_0_velocity1          0.0 # cm/s
   WIND_0_velocity2          0.0 # cm/s
   WIND_0_radius 2.4688e17 # Radius of wind boundary (cm)
   WIND_0_type   0         # type of wind (constant=0, evolving=1)
   WIND_0_mass   10.0      # stellar mass in Msun
   WIND_0_mdot   1.73e-6   # Msun per year
   WIND_0_vinf   2500.0    # km/s
   WIND_0_vrot   200.0     # equatorial surface rotation velocity km/s
   WIND_0_temp   1.0e4     # K
   WIND_0_Rstr   2.0e12    # Radius of star (cm)
   WIND_0_Bsrf   1.0       # surface magnetic field in Gauss
   WIND_0_TR0 1.0          # values of any tracer variables
   WIND_0_TR1 0.0
   WIND_0_TR2 0.0
   WIND_0_TR3 0.0
   WIND_0_TR4 0.0
   WIND_0_evofile NOFILE   # data file for evolving wind
   WIND_0_t_offset   0.0   # offset between evolutionary time and simulation time
   WIND_0_t_scalefac 1.0   # Accelerates evolution by this factor (for MS phase)
   WIND_0_updatefreq 1.0   # How often to update the wind quantities
   WIND_0_moving_star  0   # 1=star moves on grid, 0=static
   WIND_0_acceleration 0   # 0=inject wind at v_inf, 1=accelerate with beta law


Special-purpose boundaries
----------------------------------------------------------------

Boundaries for Jet/Disk simulations: “equator-reflect” and “jet”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The “jet” internal boundary condition sets a strong supersonic inflow in
the boundary region near the origin. It is an internal boundary
condition because it only operates on a small part of one boundary. The
opening angle of the jet is zero, and its width (in grid cells) is
specified by the parameter “JETradius” in the paramter file. The jet
properties are also in the paramter file, e.g.,

::

   JETradius 4
   JETdensity 1.0
   JETpressure 1.0
   JETvelocity 6.45
   JETambRO 1.0
   JETambPG 0.001
   JETambVX 0.0
   JETambVY 0.0
   JETambVZ 0.0
   JETambBX 1.0
   JETambBY 0.0
   JETambBZ 0.0

The “JETambXX” parameters set the state of the uniform external medium.

Generally only one side of a jet is simulated, because the system is
assumed to have reflection symmetry in the equatorial plane. This is not
a simple reflecting boundary however, because we assume that the system
has a poloidal magnetic field and so the normal component of the field
is continuous across the equator whereas the tangential component is
assumed to be reversed. This is what the “equator-reflect” boundary
does.

This has not been tested in 3D (where the radial component of the field
in the equatorial plane should be reversed, but the rotational component
should not be if there is a toroidal component), but in 2D axisymmetry
it works well. Probably in 3D you should model the whole system if you
are interested in the disk at all, and for that these boundary
conditions are not going to be useful anyway.

Double Mach Reflection test: “DMR” and “DMR2”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These are for the double Mach reflection test problem. DMR is the upper
boundary where the shock is inflowing. DMR2 is the lower boundary at
x<1/6, where we ignore the reflecting boundary condition and just
enforce the post-shock state at the grid cells adjacent to the boundary.

StarBench test problems: “SB1”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is not used for any published test problem; a test was proposed but
not ever developed to the point where it was considered a good test of
any specific property of a solver.

Radiative Shock Tests: “RadShock” and “RadShock2”
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These boundary conditions are for 1D and 2D simulations of radiative
shocks, where the aim is to set up a steady (or overstable) shock wave
with a post-shock cooling region. The downstream boundary is either
reflecting or absorbing. It can have a sink term that removes mass from
the grid and/or enforce a specific mass flux through the downstream
boundary. All of these conditions have some issues because the flow is
inherently non-steady if the shock is overstable, and so it is probably
best to just use a simple reflecting boundary condition in this case.
For stable shocks the condition that tries to enforce a given mass flux
through the boundary is probably good.
