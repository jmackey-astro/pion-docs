This is for PION documentation.

The `website/` directory is approximately a clone of the wordpress site at [www.pion.ie](https://www.pion.ie/)

The `docs/` directory is where the Sphinx documentation is hosted, which can be manually synced with [https://www.pion.ie/docs/](https://www.pion.ie/docs/).
Only Jonathan Mackey can update the online version of the PION docs at [https://www.pion.ie/docs/](https://www.pion.ie/docs/) via the DIAS webpage updating interface.
This repo is for keeping a master copy of the documentation.

Sphinx can be installed via `python3 -m pip install sphinx sphinx-rtd-theme` in your python virtual environment.
You may be able to use system python on linux by installing `python3-sphinx python3-sphinx-rtd-theme` on debian/ubuntu.

From the `docs/` directory you can generate and view html in `build/html/` by running the command:

```
    $ make html
    $ firefox build/html/index.html
```

The source text is using [reStructuredText](https://www.sphinx-doc.org/en/stable/usage/restructuredtext/index.html) format, and is in `docs/source/`


Please don't add anything in `/docs/build` to the repository, only your changes to the source code.
A good way to not add in new files is with the `-a` flag:

```
    $ git commit -a
    $ git push
```

This commits changes to all files that are currently under version control by git, but doesn't add any new files.
If you want to add a new file, you can do this via:
```
    $ git add <filename>
    $ git commit -a
    $ git push
```


Development docs
===================

There is a `devel` branch which applies to the `devel` branch of PION.
The `master` branch of the docs only applies to the latest publicly released version of PION, and it should only be updated if there are errors.
If you are adding documentation that applies only to the development version, then put your changes into `devel` by creating an *issue*, then an associated *merge request*, putting modifications in the new branch, and *merging* this branch back into `devel`.

