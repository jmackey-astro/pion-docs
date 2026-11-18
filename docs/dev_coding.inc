.. _ref-dev-coding:

Development workflow
================================

PION is written in C++14 and uses `git <https://git-scm.com/>`_ for version control.
The development repository is `https://gitlab.git.nrw/jmackey1/pion-dev <https://gitlab.git.nrw/jmackey1/pion-dev>`_ (ask for access, see :ref:`getting-pion`), and the development branch is ``devel``.
Releases are published on GitHub at `https://github.com/pion-cfd/PION <https://github.com/pion-cfd/PION>`_.

The NEMO microphysics library (see :ref:`ref-nemo`) is a separate repository, `https://gitlab.git.nrw/jmackey1/nemo <https://gitlab.git.nrw/jmackey1/nemo>`_.
If a change needs changes in both NEMO and PION, merge the NEMO change first.


Developing a new feature
----------------------------

1. Create a branch off ``devel``, e.g. ``git switch -c my-feature origin/devel``.  Optionally, open an issue on GitLab first to describe what you want to do.
2. Make your changes and commit them, keeping each commit to one logical change.
3. Format the C++ files you changed (see `Coding conventions`_).
4. Build PION with the tests switched on, and run the tests (see :ref:`tests`).  If you changed anything to do with boundaries or MPI communication, also test with at least 4 MPI processes and more than one OpenMP thread, because such bugs can depend on the number of processes and threads.
5. Push the branch with ``git push -o ci.skip -u origin my-feature``.  The ``ci.skip`` option stops GitLab from running the CI pipeline on every push.
6. Open a merge request into ``devel`` on GitLab.  The CI pipeline then checks the formatting, builds PION in several configurations, and runs the tests (see :ref:`tests-ci`).
7. When the pipeline passes, merge the branch.  The pipeline on ``devel`` then runs some longer tests, and updates the code documentation (see `Code documentation`_).

Don't commit build directories, simulation output, or scripts for your own machine.


Coding conventions
----------------------------

* **Formatting**: PION uses ``clang-format`` version 19 (``clang-format-19``), and the CI pipeline fails if any file is not formatted.  Format each C++ file you change with ``tools/format.sh <file>``.  Use this script rather than ``clang-format`` directly, because it protects the ``#pragma omp`` lines.  ``tools/run_format.sh`` formats the whole source tree; don't commit unrelated formatting changes.
* **Errors**: report an error with ``spdlog::error(...)``, and then either call ``exit_pion(code)`` (defined in ``source/constants.h``) for a fatal error, or return a non-zero error code that the calling function checks.  Never call ``exit()``: ``exit_pion()`` flushes the log files and stops all MPI processes with ``MPI_Abort``.
* **Log messages**: use ``spdlog::warn`` for problems that are not errors, ``spdlog::info`` for information, and ``spdlog::debug`` for debugging output (only written by ``Debug`` builds).  In ``Release`` builds only MPI rank 0 writes log messages, unless PION is compiled with ``-DLOG_ALL_PROCS=ON``.
* **OpenMP**: the loops over cells in the hydrodynamics, microphysics and ray-tracing run in OpenMP threads.  The microphysics object (including NEMO) is ``threadprivate``, so each thread has its own copy.
* Keep new code consistent with the style of the file you are editing, and comment it, with references to papers where relevant.
* Describe classes and functions in the header files with ``///`` comments, so that Doxygen includes them in the code documentation.


Code documentation
--------------------------------------------

The classes and functions of PION are documented in the source code, and `Doxygen <https://www.doxygen.nl/index.html>`_ makes HTML pages from these comments, including class diagrams and a browsable copy of the source code.

+ The CI pipeline on ``devel`` publishes these pages with GitLab Pages after every merge, at `https://jmackey1.pages.git.nrw/pion-dev/ <https://jmackey1.pages.git.nrw/pion-dev/>`_.  Only members of the PION GitLab project can view them.
+ To build them yourself, install ``doxygen`` and ``graphviz``, and add ``-DPION_BUILD_DOCUMENTATION=ON`` to the ``cmake`` command.  The pages are then made with the rest of the build (or with ``cmake --build build --target doc``) in ``build/doc/doc/html/``; open ``index.html`` in a web browser.
