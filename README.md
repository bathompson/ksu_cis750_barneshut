# ksu_cis750_barneshut

## Building and Running on Beocat

This project uses CMake for building.
You'll need to install it before you can compile on Beocat.

### To install CMake,
1. Create the `cmake` folder.
2. Download the `cmake-3.22.4-linux-x86_64.sh` file from https://cmake.org/download/.
3. Put the downloaded file in the `cmake` folder.
4. Run the downloaded file
   1. You may need to `chmod +x` the .sh first
5. Step through the installer steps
   1. When you reach the one that asks about adding a folder, answer no

### To build the project
1. Run `./cmake/bin/cmake .` in the root project directory to generate Makefiles
2. Run `make serial_direct_sum` to build that target

There will eventually be a script to build all the targets and run our tests.
