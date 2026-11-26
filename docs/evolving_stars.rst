.. _evolving-stars:

==============
Evolving stars
==============

A star (see :ref:`parameter-stars`) can have an *evolution table*: a text file with the star's properties at a series of times, usually from a stellar-evolution code such as MESA.
The star's stellar-wind sources and ionising radiation sources then take their properties from the table as the simulation runs: mass-loss rate, wind velocity and surface composition for the wind; luminosity, effective temperature and spectrum for the radiation.
One table drives both, so wind and radiation always describe the same star.

This page describes the table, the parameters, what PION does with the table, and how to make a table from a MESA model.

.. versionadded:: 3.0.0-dev.1
   Evolution tables of stars (``STAR_[i]_evo_*``), SED switching with NEMO (which needs NEMO 1.7), the timestep limits, and ``mesa2pion.py`` version 2.
   Before, evolving winds and radiation sources each read their own file (``WIND_[i]_evofile``, ``RT_EVO_FILE_[i]``).
   For the version numbers, see :ref:`pion-versions`.

.. _evolving-stars-params:

Parameters
==========

The table belongs to the star, set with these parameters (``[i]`` is the index of the star):

+-------------------------------+--------------------------------------------------+---------------------------------+
| Parameter                     | Description                                      | Units/values                    |
+===============================+==================================================+=================================+
| ``STAR_[i]_evo_file``         | Name of the evolution table                      | Default ``NOFILE`` (no table)   |
+-------------------------------+--------------------------------------------------+---------------------------------+
| ``STAR_[i]_evo_time_offset``  | Added to the times in the table                  | years (default 0)               |
+-------------------------------+--------------------------------------------------+---------------------------------+
| ``STAR_[i]_evo_time_scale``   | The table's times (plus the offset) are divided  | Default 1                       |
|                               | by this, to run the evolution faster than in     |                                 |
|                               | reality                                          |                                 |
+-------------------------------+--------------------------------------------------+---------------------------------+
| ``STAR_[i]_evo_end``          | What happens after the last row: ``off`` (the    | ``off`` (default), ``end`` or   |
|                               | star's wind and radiation switch off),           | ``hold``                        |
|                               | ``end`` (the simulation ends), ``hold`` (the     |                                 |
|                               | star keeps the values of the last row)           |                                 |
+-------------------------------+--------------------------------------------------+---------------------------------+
| ``STAR_[i]_evo_update``       | Relative change of luminosity or effective       | Default 0.05                    |
|                               | temperature after which the star's radiation     |                                 |
|                               | sources are updated                              |                                 |
+-------------------------------+--------------------------------------------------+---------------------------------+

The simulation time of a row is :math:`t_\mathrm{sim} = (t_\mathrm{table} + t_\mathrm{offset}) / s`, with :math:`s` the time scale.
For example, to start a simulation at the zero-age main sequence (ZAMS) with a table whose first row is the ZAMS at an age of :math:`3\times10^4` years, set ``STAR_0_evo_time_offset -3.0e4``.
PION's year is :math:`3.155815\times10^7` s, MESA's is :math:`3.15576\times10^7` s (a Julian year); the difference (0.002%) matters only for offsets that should land exactly on a row.

Attach the wind and radiation sources to the star with ``WIND_[i]_star`` and ``RT_star_____[i]``.
A wind attached to an evolving star takes its properties from the table if it is of an evolving type (``WIND_[i]_type`` 1, 2 or 3); a constant wind (type 0) stays constant.
``icgen`` and ``pion`` read the table (every MPI process reads it once), so it must be in the directory they run in, or be given with its path.

A minimal example, a star with a wind and an ionising source for NEMO:

.. code-block:: none

  STAR_Nstars 1
  STAR_0_pos0 0.0
  STAR_0_pos1 0.0
  STAR_0_pos2 0.0
  STAR_0_mass 35.0
  STAR_0_moving 0
  STAR_0_evo_file star35.pion
  STAR_0_evo_time_offset -2.928e4   # first row (ZAMS) at t = 0
  STAR_0_evo_end off

  WIND_NSRC 1
  WIND_0_star 0
  WIND_0_type 1                      # evolving wind
  ...                                # the other WIND_0_ parameters
  RT_Nsources 1
  RT_star_____0 0
  RT_effect___0 7                    # an SED type: the table's SED column is used
  ...                                # the other RT_ parameters

The wind and radiation parameters that the table provides (mass-loss rate, velocities, temperature, radius, luminosity) must still be in the parameter file, but their values are replaced by the table's.


The evolution table
===================

The table is a text file in CGS units, with one row per time.
Lines starting with ``#`` are comments.
If a comment line names the columns, with ``time`` as the first name (for example ``# time M L Teff Mdot vrot vcrit vinf X_H X_He ... SED``), PION finds the columns by name and ignores columns it does not know.
Files without such a line are read in the order of the first 15 columns below (older wind files), or the first 8 (older radiation-source files).

+-----------------------+-----------------------------------------------+--------------------+
| Column                | Quantity                                      | Unit               |
+=======================+===============================================+====================+
| ``time``              | time (stellar age)                            | s                  |
+-----------------------+-----------------------------------------------+--------------------+
| ``M``                 | stellar mass                                  | g                  |
+-----------------------+-----------------------------------------------+--------------------+
| ``L``                 | luminosity                                    | erg/s              |
+-----------------------+-----------------------------------------------+--------------------+
| ``Teff``              | effective temperature                         | K                  |
+-----------------------+-----------------------------------------------+--------------------+
| ``Mdot``              | mass-loss rate                                | g/s                |
+-----------------------+-----------------------------------------------+--------------------+
| ``vrot``              | surface (equatorial) rotation velocity        | cm/s               |
+-----------------------+-----------------------------------------------+--------------------+
| ``vcrit``             | critical rotation velocity                    | cm/s               |
+-----------------------+-----------------------------------------------+--------------------+
| ``vinf``              | wind terminal velocity                        | cm/s               |
+-----------------------+-----------------------------------------------+--------------------+
| ``X_H`` ... ``X_O``,  | surface mass fractions of H, He, C, N, O and  | mass fraction      |
| ``X_Z``               | of all other elements                         |                    |
+-----------------------+-----------------------------------------------+--------------------+
| ``X_D``               | dust                                          | mass fraction      |
+-----------------------+-----------------------------------------------+--------------------+
| ``SED``               | type of stellar atmosphere for the ionising   | ``RT_effect``      |
|                       | spectrum (NEMO, see below)                    | value, 7-12        |
+-----------------------+-----------------------------------------------+--------------------+
| ``X_Ne``, ``X_Si``,   | other elements (any ``X_<element>`` column)   | mass fraction      |
| ``X_S``, ``X_Fe``     |                                               |                    |
+-----------------------+-----------------------------------------------+--------------------+

``time``, ``L`` and ``Teff`` are required; missing columns are zero (``SED``: none).
The stellar radius is not in the table: PION calculates it from :math:`L = 4\pi R^2\sigma T_\mathrm{eff}^4`.
The table needs at least two rows, with increasing times.

**Interpolation.**
Between two rows, PION interpolates linearly in time: in the logarithm for ``L``, ``Teff`` and ``Mdot`` (``Mdot`` linearly if any row has :math:`\dot{M}\leq0`), linearly for all other quantities.
The SED type is not interpolated: it is that of the row at or before the current time, so the spectrum changes abruptly at the row where the type changes.

**Before the first row and after the last row.**
Before the first row the star is not there yet: its wind and its NEMO radiation sources are off.
An MPv3 radiation source cannot be switched off, so a simulation with MPv3 must not start before the first row (PION stops with an error).
After the last row, ``STAR_[i]_evo_end`` decides: ``off`` switches the wind and the NEMO sources off (MPv3 sources keep their last values), ``end`` ends the simulation, ``hold`` keeps the last row.


What PION does with the table
=============================

**Winds.**
An evolving wind takes :math:`\dot{M}`, :math:`v_\infty`, :math:`v_\mathrm{rot}`, :math:`v_\mathrm{crit}`, :math:`T_\mathrm{eff}`, the radius and the tracers from the table, at every timestep.
Every tracer named like an element column (``X_H``, ``X_He``, ``X_C``, ...) gets that column; a tracer ``dust`` or ``X_D`` gets the dust column.
An element tracer without a column gets the solar mass fraction (Asplund et al. 2009) scaled by the table's metallicity (:math:`1 - X_\mathrm{H} - X_\mathrm{He} - X_\mathrm{D}` of the first row), with a warning.
With NEMO, the wind's ion fractions can be set to the equilibrium state in the star's radiation field (``WIND_[i]_equilibrium 1``, see :ref:`ref-nemo`); they are recalculated whenever the wind or the star's radiation changes.

**Radiation sources.**
A radiation source attached to the star takes its luminosity, effective temperature and radius from the table.
It is updated when :math:`L` or :math:`T_\mathrm{eff}` has changed by more than ``STAR_[i]_evo_update`` (default 5%) since its last update, or when the SED type changes.

+ **MPv3** (``RT_effect 5``) recalculates its photoionisation rate tables for the new star.
+ **NEMO** (``RT_effect`` 7-12) gets the new luminosity, temperature, radius and SED type.
  If the table has an ``SED`` column, it sets the source's SED type, replacing ``RT_effect___[i]``: the spectrum switches, for example, from an ATLAS O-star atmosphere to a PoWR Wolf-Rayet atmosphere at the row where the star becomes a WR star.
  Sources with other ``RT_effect`` values ignore the ``SED`` column.
  An effective temperature outside the range of the SED table is clamped to the table's edge, with a warning at every update.

**Timesteps.**
The timestep is shortened so that the simulation lands exactly on the star's first row (where it switches on), on every row where the SED type changes, and on the last row (unless ``STAR_[i]_evo_end`` is ``hold``).
On nested grids, the coarse (level-0) timestep ends there.
Also, the timestep of each level is limited to :math:`\mathrm{CFL}\,\Delta x / v_\mathrm{wind}`, where :math:`v_\mathrm{wind}=\sqrt{v_\infty^2+v_\mathrm{rot}^2}+c_\mathrm{s}(T_\mathrm{eff})` is the largest signal speed in the wind region during the coming level-0 step.
The cells of the wind region are not part of the usual CFL condition, so without this limit a wind that switches on or speeds up could cross more than one cell in a timestep.
This applies to all winds, including those that are constant in time; where the wind has been blowing for a while, the cells next to the wind region already limit the timestep in the same way.

**Old parameters.**
``RT_EVO_FILE_[i]`` and ``WIND_[i]_evofile`` still work but are deprecated.
PION turns each file into the table of a star: the star the source is attached to, or a new star at the source's position (sources with the same file and position share one star).
Their old behaviour is kept: the simulation ends after the last row, radiation sources are updated after a 1% change, and wind times use ``WIND_[i]_t_offset`` and ``WIND_[i]_t_scalefac``.
The converted parameters are written to the snapshots, so a restart does not convert them again.
``WIND_[i]_updatefreq`` is no longer used: evolving winds are updated at every timestep.


.. _evolving-stars-mesa:

Making a table from a MESA model
================================

``tools/stellar-evolution/mesa2pion.py`` in the PION repository converts a MESA ``history.data`` file into an evolution table (it needs only Python and NumPy):

.. code-block:: bash

  python3 tools/stellar-evolution/mesa2pion.py LOGS/history.data star35.pion

It prints how many models it used, and how long the star spends in each spectral class and SED type.
The README in ``tools/stellar-evolution/`` describes every column and option; the main points are:

+ **From the ZAMS.**
  A MESA history starts on the pre-main sequence, whose cool models would get dust.
  The table starts at the ZAMS, the first model whose central hydrogen has fallen by 0.001 (``--keep-pre-ms`` keeps the earlier models).
  The times are MESA's ages, so the first row is at the ZAMS age; use ``STAR_[i]_evo_time_offset`` to shift it.
+ **Fewer rows** (``--thin``, default 0.01).
  A MESA model is left out if PION's interpolation between the models that are kept reproduces it to 1% in every column.
  For a 35 :math:`\mathrm{M_\odot}` model from the ZAMS to core helium depletion this keeps about 220 of 700 models.
  ``--thin 0`` keeps every model.
+ **Wind velocity.**
  :math:`v_\infty` is the escape velocity times a factor that depends on :math:`T_\mathrm{eff}` for OB stars and supergiants (Eldridge et al. 2006), and on the luminosity for Wolf-Rayet stars (Nugis & Lamers 2000).
+ **Dust**, for cool winds below 6000 K, as silicates and oxides of red-supergiant winds (``--dust-model rsg``, default) or with interstellar depletions (``--dust-model ism``).
+ **Wolf-Rayet stars and the SED column.**
  A star is a WR star when :math:`T_\mathrm{eff}>10^4` K and its surface :math:`X_\mathrm{H}<0.4`, as in MESA's "Dutch" wind scheme (``--t-wr``, ``--x-wr``).
  WR stars are WNL while hydrogen is left, and without hydrogen WNE (C < N) or WC.
  The SED column is:

  ======  ======================  ======================  =====================================
  SED     NEMO table              :math:`T_\mathrm{eff}`  used for
  ======  ======================  ======================  =====================================
  7       ATLAS                   3.5 – 49 kK             stars that are not WR stars
  11      PoWR WNL, X_H = 0.5     25 – 100 kK             WN stars with :math:`X_\mathrm{H}\geq0.35`
  12      PoWR WNL, X_H = 0.2     25 – 100 kK             WN stars with :math:`0.1\leq X_\mathrm{H}<0.35`
  9       PoWR WNE                32 – 200 kK             WN stars with :math:`X_\mathrm{H}<0.1`
  8       PoWR WC                 40 – 200 kK             WC stars
  10      blackbody               3.5 – 100 kK            only if no table covers the temperature
  ======  ======================  ======================  =====================================

  If a star's temperature is outside the table of its class, the script uses another table that covers it (a hot hydrogen-poor star just before it becomes a WR star, above ATLAS's 49 kK, gets WNL-H50).

``tools/stellar-evolution/mesa-example/`` has the MESA inlists of a 35 :math:`\mathrm{M_\odot}` model at solar metallicity, rotating at 200 km/s, with the mixing of the Bonn models (Brott et al. 2011).
It goes from an O star through a short yellow-supergiant phase to WNL, WNE and WC stars, and is the reference track used to test PION's evolving stars.


Things that go wrong
====================

+ **"cannot read evolution file"**: the file is not in the directory where ``pion`` runs, or a row has a missing or non-numeric value.
+ **An MPv3 source before the first row**: PION stops at the start; set ``STAR_[i]_evo_time_offset`` so that the simulation starts at or after the first row.
+ **The wind or the radiation switches off during a run**: the simulation has passed the last row with ``STAR_[i]_evo_end off``.
+ **Warnings that an element tracer is not in the evolution file**: that tracer gets a scaled solar value; add the column (``mesa2pion.py`` writes H, He, C, N, O, Ne, Si, S and Fe) or ignore the warning if the element does not matter.
+ **NEMO warns that Teff is outside an SED table**: the star is hotter or cooler than the table of its SED type; NEMO uses the table's edge.
  Tables made with ``mesa2pion.py`` choose another table in such cases, so this mostly happens with tables made by hand.
