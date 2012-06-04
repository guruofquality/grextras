########################################################################
GR Extras - Misc GNU Radio Blocks and Features
########################################################################

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Documentation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Extremely useful documentation:
https://github.com/guruofquality/grextras/wiki

See the various blocks and QA code in this source tree as examples.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Questions, comments, issues?
discuss-gnuradio@gnu.org

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Build dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
* MSVC or GCC
* GNU Radio
* Boost
* SWIG
* UHD (optional)

Notes:
- UHD install is optional for message passing UHD amsg source block.
- The Boost and SWIG dependencies should already be satisfied
by a standard GNU Radio development environment.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Build instructions UNIX Makefiles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
mkdir <build dir>
cd <build dir>
cmake <source dir>
make
sudo make install

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Build instructions MSVC
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
* Run CMake GUI, configure with grextras source path and Visual Studio.
* Point CMake to the gnuradio-core, gruel, and volk library and include path.
* Point CMake to the SWIG executable.
* Point CMake to the UHD library and include path (optional).
* Generate Visual Studio Project file.
* Buiult the ALL_BUILD and INSTALL targets.
