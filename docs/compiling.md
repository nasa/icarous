---
layout: default 
title: Compilation
nav_order: 2
---

## Downloading 
ICAROUS is publicly available through the NASA open source agreement from [NASA/Icarous github repository](https://github.com/nasa/icarous.git). To download ICAROUS, clone the repository and its submodules:
```
git clone https://github.com/nasa/icarous.git
```
Note that the above repository includes the NASA [core Flight Executive (cFE)](https://github.com/nasa/cFE) repository as a submodule. Update the publicly available submodules using the provided script:
```
bash UpdateModules.sh
```

## Compilation 
ICAROUS comes with a CMake build system. 

### Select apps to run
List the apps to be compiled and run in [Icarous_defs/apps.cmake]().

### Compiling
```
# Run make to compile
make 
# Install the executable
make install
```

Please refer to the section on executing ICAROUS to learn how to configure ICAROUS appropriately and run ICAROUS.
