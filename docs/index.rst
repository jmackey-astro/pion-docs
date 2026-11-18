############################################
PION: simulations of nebulae around stars
############################################


`PION <https://www.pion.ie/>`_ is a grid-based fluid dynamics code for hydrodynamics and magnetohydrodynamics, including a ray-tracing module for calculating the attenuation of radiation from point sources of ionizing photons. It also has a module for coupling fluid dynamics and the radiation field to microphysical processes such as heating/cooling and ionization/recombination. The algorithms are described in `Mackey et al. (2021) <https://scixplorer.org/abs/2021MNRAS.504..983M/abstract>`_, `Mackey (2012) <https://scixplorer.org/abs/2012A%26A...539A.147M/abstract>`_, `Mackey & Lim (2011) <https://scixplorer.org/abs/2011MNRAS.412.2079M/abstract>`_, and `Mackey & Lim (2010) <https://scixplorer.org/abs/2010MNRAS.403..714M/abstract>`_.

PION was written to model the evolution of HII regions, photoionized bubbles that form around hot stars, and developed to include stellar wind sources so that both wind bubbles and photoionized bubbles can be simulated at the same time. It is versatile enough to be extended to other applications.

There are two versions of PION, the released version and the development version.  The released version is freely available on GitHub at `https://github.com/pion-cfd/PION <https://github.com/pion-cfd/PION>`_.  If you want to work with the development version you need to request access (see :ref:`getting-pion`).

+ Documentation for the released version: `https://pion.ie/docs/ <https://pion.ie/docs/>`_.
+ Documentation for the development version: `https://pion-docs-dev.readthedocs.io/ <https://pion-docs-dev.readthedocs.io/>`_ (this site).

**This documentation is for the development version.**

Guide
^^^^^
.. toctree::
  :numbered:
  :maxdepth: 3

  getting_started.rst
  example_sim.rst
  example_cwb.rst
  example_nemo.rst
  python.rst
  command_line.rst
  parallel.rst
  nested_grid.rst
  parameter-file.rst
  modules.rst
  tests.rst
  troubleshooting.rst
  developers_guide.rst
  publications.rst
  usage/license.rst
  Code reference (Doxygen) <https://jmackey1.pages.git.nrw/pion-dev/>


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
