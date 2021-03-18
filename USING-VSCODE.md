Using VS Code
=============

Installation
------------

The following extensions must be installed:

* C/C++: From Microsoft
* Cortex-Debug: From marus25
* Makefile Tools: From Microsoft
* CMake Tools: From Microsoft

Other extensions are useful:

* CMake: From twxs
* Doxygen Documentation Generator: From Christoph Schlosser
* gitignore: From CodeZombie
* GitLens - Git supercharged: From Eric Amodio
* LinkerScript: From Zixuan Wang
* Platform IDE: From PlatformIO


Usage
-----

One can use the embedded terminal and run make

    make build
    make clean
    make flash
    make docs

or run *make help* to see other options.

Or using the GUI, select *Run Build Task...* option from the *Terminal* menu. At the command line, a menu is shown with a subset of the Makefile options:

    Make: make build
    Make: make clean
    Make: make flash

To run, select *Run without debugging* or *Start Debugging* from the *Run* menu. Or press Ctrl-F5 or F5, respectively.

There are builtin dependencies. For example, running automatically run the "Make: make build* task before starting.



