.. _ref-dev-coding:

Coding style and guides
================================

PION is written in C++ and developed using `git <https://git-scm.com/>`_ as the version control system.
The PION development branch is on a DIAS gitlab repo `https://git.dias.ie/compastro/pion <https://git.dias.ie/compastro/pion>`_, and the name of the development branch is ``devel``.
All active development of new features takes place on ``devel``.
Once a set of changes are complete and a new version of PION is to be released, ``devel`` will be merged/rebased back to the ``master`` branch and tagged as a release.
The new ``master`` branch is then pushed to the public PION repository at `https://git.dias.ie/massive-stars-software/pion <https://git.dias.ie/massive-stars-software/pion>`_.


Developing a new feature
----------------------------

Feature branches can be created off ``devel`` as follows, using the gitlab web interface:

1. Create an issue with a short title describing the feature you want to work on.

2. Open the issue page, and there is a dropdown menu for ``create merge request``.  Click the down arrow and choose a name for the new branch (the number automatically chosen is the number of the issue you have created) and select that the source branch should be ``devel``.  Then click ``create merge request``.

3. This brings you to a page for the merge request which is by default marked as ``Draft``.  You don't need to do anything here, just note the name of the branch, e.g. ``2-test-stuff``.

4. Pull the server updates to your local copy of the repo, and checkout the new branch: ``$ git checkout 2-test-stuff``.

5. Then you can work on this branch until you are finished with the new feature and it is time to merge the branch back into devel.  There are at least two checks to make before merging:

    i. run ``$ ./tools/run_format.sh`` to autoformat your changes according to the PION conventions.  Then commit and push changes.

    ii. Pushing to a branch off ``devel`` should trigger the pipeline to run.  This is a series of jobs that are sent to a server and run in a docker container somewhere.  The first checks that the source code is formatted.  The next 4 check that the PION executables compile without errors, and the last runs the Double Mach Reflection test and checks that the result has an L1 error norm of :math:`<10^{-10}` compared with a reference solution (i.e. it checks for consistency with older code versions).

6. Once these steps are completed successfully, navigate to the merge request page on your browser and click ``Mark as ready`` at the top right of the page.Beside the (hopefully green) ``Merge`` button you should select ``delete source branch`` and ``squash commits``.  You can add an informative squash commit message that explains what you did on this branch with all the commits.  Then click ``Merge``.

7. Check that the merged changes now on ``devel`` pass the pipeline checks (this can take up to 20 mins).


Coding conventions
----------------------------

* Set your code editor to use spaces not tabs for indentation (or run ``$ ./tools/run_format.sh`` before each commit).
* Each indendation block is 2 spaces
* Try to add comments to your code to explain what it does, including references to papers if possible.
* Header files should have class and function descriptions in comment blocks denoted with ``///`` so that automatic documentation generators (like doxygen) can parse the comments.


Generating documentation from source code
--------------------------------------------

PION has plenty of in-place documentation that can be generated with `doxygen <https://www.doxygen.nl/index.html>`_.
Here is how to compile the docs:

+ On debian/ubuntu systems you can get all the functionality by installing the packages ``doxygen doxygen-latex graphviz``.
+ Add ``-DPION_BUILD_DOCUMENTATION=ON`` to the list of options in your cmake command (maybe in the build script).
+ When you build PION this should generate documentation in the directory ``build/doc/doc``, the HTML docs in a subdirectory called ``html`` and the LaTeX docs in ``latex``.
+ Navigate to this directory and run e.g. ``$ firefox html/index.html``
+ Use the dropdown menus to explore the class hierarchy and the source code.
+ To generate a PDF you can navigate to the ``latex`` subdirectory of the generated docs and run ``$ make``; this should generate a PDF called ``refman.pdf``.


