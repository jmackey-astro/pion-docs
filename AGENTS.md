# pion-docs: notes for coding agents

Sphinx documentation for the development version of PION, an MPI + OpenMP
C++ code for astrophysical (radiation-)magnetohydrodynamics. It is
published at https://pion-docs-dev.readthedocs.io from the `dev` branch.

- PION source: https://gitlab.git.nrw/jmackey1/pion-dev, branch `devel`
  (usually checked out in `~/code/pion`, with its own `AGENTS.md`).
- NEMO microphysics library: https://gitlab.git.nrw/jmackey1/nemo
  (usually `~/code/nemo`).
- PyPion (Python reader for PION snapshots):
  https://github.com/jmackey-astro/PyPion.

## Branches and pull requests

- `dev` documents PION `devel`. `main` documents the last public release
  and is only changed to fix errors.
- Make a branch off `dev` for each change and open a pull request against
  `dev` on GitHub (`jmackey-astro/pion-docs`). Commit, push or open PRs
  only when asked.
- PRs are rebase-merged, which changes the commit hashes. For a branch
  stacked on another PR, after the parent is merged:
  `git rebase --onto origin/dev <old-parent-tip>`, then push with
  `--force-with-lease`.
- `gh pr edit` fails on this repo (Projects classic deprecation). Change
  a PR's title or description with
  `gh api -X PATCH repos/jmackey-astro/pion-docs/pulls/<N> -F body=@file`.
- If a page describes PION behaviour that is still on an unmerged PION
  branch, name that branch in the PR description, and say that it should
  be merged first.
- `TODO.md` lists open and done documentation tasks with their PR
  numbers (until PR #14 is merged it is only on the `docs-todo` branch).
  Update it when a task is done or a new one is found.

## Building

```bash
python3 -m venv /mnt/scratch/claude/sphvenv
/mnt/scratch/claude/sphvenv/bin/pip install -r docs/requirements.txt
cd docs && /mnt/scratch/claude/sphvenv/bin/sphinx-build -q -b html . /mnt/scratch/claude/pion-docs-html
```

- Read the Docs builds with `docs/requirements.txt` (`docs/requirements.in`
  is the input for `pip-compile`).
- The build must have **no warnings** (unresolved references, duplicate
  labels, title underlines that are too short, toctree problems). Read the
  Docs doesn't fail on warnings (`fail_on_warning: false`), so check
  locally before every commit.
- Build outside the repository, or in `build/` (`make html`), and never
  commit build output.

## Layout

- `docs/index.rst`: the main toctree, with numbered sections. Add new
  pages here.
- `docs/examples.rst`: the "Examples" section, with its own toctree of
  `example_*.rst` pages.
- `docs/developers_guide.rst` includes `dev_coding.inc`, `dev_mpi.inc`
  and `dev_winds.inc` (`.. include::`). Long sections of the developer
  guide go in their own `.inc` file.
- Reference pages:
  - `parameter-file.rst`: every parameter-file setting;
  - `command_line.rst`: command-line options;
  - `modules.rst`: microphysics modules and boundary conditions;
  - `nested_grid.rst`, `parallel.rst`, `tests.rst`,
    `analysis_tools.rst`, `troubleshooting.rst`, `python.rst` (PyPion).
- Files for the examples (parameter files, plotting scripts, figures)
  are in `docs/ex-<name>/`, e.g. `docs/ex-nemo/`, `docs/ex-sim-img/`.
  Pages offer them with `:download:` and show figures with `.. figure::`.
- `conf.py` sets `numfig = True`, so figures are referenced with
  `:numref:` and need a label (`.. _fig-name:`) just before the figure.

## Writing

- Every page starts with a label (`.. _page-name:`). Refer to pages and
  sections with `:ref:`, not with file names or URLs.
- Write one sentence per line in new text, so that diffs are readable.
  Keep the style of the surrounding text when editing old pages.
- Write for PION users, most of whom are astronomers rather than
  programmers: what a feature does, what to set, and what goes wrong.
  Put implementation details in the developer guide.
- Spelling is mixed (ionisation/ionization); keep each page consistent.
  Recent pages use British spelling.
- Write ions as H⁺, He²⁺, O²⁺, i.e. ``:math:`\mathrm{O}^{2+}` `` in RST
  and LaTeX labels such as `O$^{2+}$` in plots, never in spectroscopic
  notation (He II, O III).
- Give parameter names, command-line options, file names and log
  messages exactly as in the code, as RST literals (double backticks).
- Units: cgs, as in PION (cm, g, s, erg). Give times in Myr or kyr in the
  text when that is clearer, with the value in seconds that goes in the
  parameter file.

## Accuracy

Everything in the docs must match the PION `devel` source. Before writing
about a feature, read the code; don't rely on old docs, comments, or
usage messages, which are often out of date.

- **Parameters:** search for the parameter name in `source/`.
  - Most are read in `source/dataIO/dataio_base.cpp` (snapshot header
    parameters).
  - Initial-condition parameters are read in `source/ics/`.
  - `EP_*` parameters are in the microphysics setup.
  - Check the type (`atoi` or `atof`), the default, and the allowed
    values.
- **Command-line options:** `source/sim_control/sim_init.cpp` (pion) and
  `source/main_icgen.cpp` (icgen).
- **Tools in `analysis/`:** usage messages are often wrong (e.g.
  `silo2fits` advertised domain-resizing options that were never
  implemented). Read the argument parsing and run the tool.
- **Run what you describe:** run the commands in a scratch directory, and
  check outputs, file names, and log messages. Don't invent output or
  numbers: copy them from a real run, or leave them out. Say how a
  number was obtained if it is an estimate (e.g. a run time).
- **Bugs in PION:** if you find one while documenting, report it to the
  user and, if asked, fix it on a PION branch (following PION's
  `AGENTS.md`). Document the fixed behaviour, and say in the PR which
  PION branch it depends on.
- When a PION change affects the docs (removed parameters, new options,
  new warnings), update every page that mentions it. Search the whole of
  `docs/` with `grep -rn`.

## Running PION for examples and checks

- Build PION as in its `AGENTS.md` (out of source, in `build/`). Some
  tools need options that the default build may not have, e.g.
  `silo2fits` and `projection2D` need `-DPION_USE_FITS=ON`. Make a
  separate build directory for these, outside the PION repository.
- Make initial conditions with `icgen-ug`/`icgen-ng`, and run with
  `mpirun -np N pion-ug ... omp-nthreads=M`. Use a power of 2 for the
  number of MPI processes.
- PyPion needs the Silo Python module, e.g.
  `PYTHONPATH=$HOME/.local/silo/lib:/path/to/PyPion/src`.
- Keep test runs in a scratch directory, not in either repository.
- NEMO runs are expensive, because the microphysics usually limits the
  timestep. Estimate the cost with a short low-resolution run first. Ask
  the user before starting runs that take more than about 30 minutes;
  they can run large jobs on an HPC system.

## Things to avoid

- Don't run PION's `tools/format.sh` (clang-format) on anything here,
  including parameter files: it reformats them.
- Don't edit files in the PION or NEMO repositories as part of a docs
  task without being asked. Another agent may be working in the NEMO
  repository.
- Don't commit large binary files other than the PNG figures of the
  examples, and keep figures small (about 100 kB, `dpi=120`).
