---
layout: default 
title: Compilation
nav_order: 2
---

## Downloading 
ICAROUS is publicly available through the NASA open source agreement from [NASA/Icarous github repository](https://github.com/nasa/icarous.git). To download ICAROUS, clone the repository and its submodules:
```
git clone --recurse-submodules https://github.com/nasa/icarous.git
```
Note that the above repository includes the NASA [core Flight Executive (cFE)](https://github.com/nasa/cFE) repository as a submodule.

## Compilation 
ICAROUS comes with a CMake build system. Currently supported platforms include x86_64 and ARM architecture running a unix based operating system (preferrably Ubuntu). The compilation requires setting a few environment variables. For convenience, these variables are defined by the `SetEnv.sh` script. 
```
cd icarous
# This script should be sourced from icarous/ to set the path variables correctly.
source SetEnv.sh 
``` 
### Patches for ARM: 
If you are compiling ICAROUS on an ARM platform, apply the ARM patches located on the patches/arm folder before compilation.
```
cd patches/arm
bash apply_patches.sh
```
### Patches for OSX:
Or, if you are just compiling ICAROUS on OSX to play with it, apply the OSX patches located on the patches/osx folder before compilation.
```
cd patches/osx
bash apply_patches.sh
```
NOTE: ICAROUS and cFS can be compiled and run on OSX. Due to the lack of extended posix functionality on OSX, the patches provide implementations of osal's timer api. However, this timer api implementation currently is incompatible with the scheduler cFS application. We recommend using the macScheduler app instead.

### Select apps to run
List the apps to be compiled and run in cFS under [CMake/apps.cmake]().

### Compiling
```
# from icarous/ create a build folder to store all the build files
mkdir build
cd build

# run cmake to generate the make files
cmake ..

# compile icarous (use -j<#num of cores> to parallelize the build process)
make cpu1-install -j4
```

Please refer to the section on executing ICAROUS to learn how to configure ICAROUS appropriately and run ICAROUS.
