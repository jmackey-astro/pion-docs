############################################
PION: simulations of nebulae around stars
############################################


`PION <https://www.pion.ie/>`_ is a grid-based fluid dynamics code for hydrodynamics and magnetohydrodynamics, including a ray-tracing module for calculating the attenuation of radiation from point sources of ionizing photons. It also has a module for coupling fluid dynamics and the radiation field to microphysical processes such as heating/cooling and ionization/recombination. The algorithms are described in `Mackey et al. (2021) <https://ui.adsabs.harvard.edu/abs/2021MNRAS.tmp..790M/abstract>`_, `Mackey (2012) <https://ui.adsabs.harvard.edu/abs/2012A%26A...539A.147M/abstract>`_, `Mackey & Lim (2011) <https://ui.adsabs.harvard.edu/abs/2011MNRAS.412.2079M/abstract>`_, and `Mackey & Lim (2010) <https://ui.adsabs.harvard.edu/abs/2010MNRAS.403..714M/abstract>`_.

PION was written to model the evolution of HII regions, photoionized bubbles that form around hot stars, and developed to include stellar wind sources so that both wind bubbles and photoionized bubbles can be simulated at the same time. It is versatile enough to be extended to other applications.

The current version of PION is 2.0.0, available `at the release page <https://git.dias.ie/massive-stars-software/pion/-/releases/pion2.0.0>`_.

There are two versions of PION, the released version and development version.  Only the released version is freely available.  If you want to work with the development version you need to request access.

+ Documentation for the released version, 2.0.0: `https://pion.ie/docs/ <https://pion.ie/docs/>`_.
+ Documentation for the development version: `https://pion.ie/docs/dev/ <https://pion.ie/docs/dev/>`_.

**This documentation is for the released version.**

Guide
^^^^^
.. toctree::
  :numbered:
  :maxdepth: 3

  getting_started.rst
  example_sim.rst
  example_cwb.rst
  python.rst
  parameter-file.rst
  modules.rst
  developers_guide.rst
  usage/license.rst
  usage/readme.rst 


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
