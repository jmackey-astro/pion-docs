---
name: add-example
description: Add a worked example page to the PION docs (parameter file, run instructions, plotting script, figure and description of the results), or update the figure and results of an existing example after re-running it. Use when asked to document a simulation setup as an example.
---

# Adding a worked example to the PION docs

An example page lets a user download a parameter file, run it, make the
same figure, and understand what they see. Everything on the page comes
from a real run. Read `AGENTS.md` first (building, accuracy, style).

## 1. Choose the setup

- Start from an existing parameter file if possible: PION's tests
  (`test/ci-inputs/` in the PION repository) or an existing example.
  Check every parameter against the source; test files may contain old
  or unused parameters.
- Pick a resolution and end time that run in minutes to about an hour on
  a workstation. Estimate the cost with a short run first:
  - time per step (from the `New time ... dt ... steps ... runtime` log
    lines);
  - the timestep, and what limits it (hydrodynamics or microphysics);
  - the number of steps to the end time.

  If the full run takes more than about 30 minutes, tell the user the
  estimate and ask before running it; they may run it on an HPC system
  and give you the output directory.
- Check that the physics happens in the run, e.g. with analytic
  estimates (Strömgren radius, expansion law, cooling time) before
  choosing the domain and end time. Then confirm it in the output.

## 2. Files

Put all files for the example in `docs/ex-<name>/` (create it, or use an
existing directory for a related example):

- `params_<Name>_n<cells>.txt`: the parameter file. Start it with a
  comment block saying what the example is, what it needs (e.g.
  `-DPION_NEMO=ON`), the grid and resolution, and what happens.
- `plot_<name>.py`: a plotting script that takes the parameter file and
  snapshot(s) on the command line, reads them with PyPion
  (`pypion.ReadData`), and writes a PNG.
  - Read tracer names and source positions from the parameter file
    rather than hard-coding them. Silo names tracer n `Tr%03d_<name>`,
    with `+` written as `p`, e.g. `Tr011_He1p`.
  - Use LaTeX ion labels (`O$^{2+}$`), units in the axis labels, and
    the simulation time in the title or a label.
  - Save with `dpi=120`; keep figures to about 100 kB.
- `<name>.png`: the figure, made by the script from the run.

## 3. Run and check

1. Run `icgen-ug`/`icgen-ng` and `pion-ug`/`pion-ng` in a scratch
   directory (not in a repository), with a power-of-2 number of MPI
   processes and `omp-nthreads=N`.
2. Record the number of steps and the run time, and the number of
   processes and threads used.
3. Make the figure with the plotting script exactly as the page will
   tell the user to.
4. Check each statement you will make about the result against the
   snapshot data, with a short Python check: sizes, densities,
   temperatures, ion fractions, times. Don't describe features you only
   saw by eye without checking the numbers.
5. If you changed the run's settings partway through (e.g. restarted
   with different options), either repeat the run without the change,
   or describe what you did on the page. Ask the user which.

## 4. The page

Create `docs/example_<name>.rst` and add it to the toctree in
`docs/examples.rst`. Use these sections (see `example_nemo.rst` and
`example_nemo2d.rst`):

1. A label (`.. _example-<name>:`), a title that says what it is and the
   dimension (e.g. "Two merging HII regions with NEMO (2D)"), and a short
   introduction.
2. **Requirements:** the build options, PyPion/matplotlib, and the
   `:download:` links to the parameter file and the script. Mention the
   related PION test, if there is one.
3. **The parameter file** (or **The setup**): the important settings,
   with short `code-block:: none` excerpts and links to
   `:ref:`parameter-file``. Don't repeat the whole file.
4. **Running the simulation:** the commands, the number of processes and
   threads, and the measured run time and number of steps.
5. **Plotting the results:** the plotting command, the figure (with a
   label and `:numref:`), and a list of what the figure shows, checked
   against the data.
6. **Things to try:** a few changes to the parameters, and what they
   show.

If the example replaces a pointer elsewhere (e.g. "see the test
script..."), change that pointer to a `:ref:` to the new page.

## 5. Finish

- Build the docs; there must be no warnings.
- Check the downloads are in the built HTML (`_downloads/`).
- Add the example to `TODO.md` if it is tracked there.
- Commit (when asked) the page, `examples.rst` and the files in
  `docs/ex-<name>/`; don't commit the snapshots or the scratch runs.
