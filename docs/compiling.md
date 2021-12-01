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

## Compilation (C++ Modules)
Compile modules as follows:

### Linux/OSX

```
cd Modules
mkdir build
cd build
cmake ..
make -j8
```

The compiled libraries are located under `Modules/lib`

### Setup library paths
- Define a new environment variable called `ICAROUS_HOME` to contain the path to the Icarous folder. We recommend using the export command in your ~/.bash_profile (OSX) or ~/.bashrc (Linux) for persistence. For example,
```
export ICAROUS_HOME=/path/to/icarous
```
- Append the `lib` folder to your `DYLD_LIBRARY_PATH` (OSX) or `LD_LIBRARY_PATH` (Linux) variable. 
```
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ICAROUS_HOME/Modules/lib
```

### Windows
- We recommend using [vscode](https://code.visualstudio.com/) for compiling on windows.
- We recommnd using [msys2](https://www.msys2.org/) to install required packaged. 
- Install MinGW-w64 compiler. See [msys2](https://www.msys2.org/) for installation instructions.
- Install cmake.
- Define environment variable called `ICAROUS_HOME` that contains the path of the Icarous root directory.
- Include the following in your vscode `settings.json` file (change path values as needed):
```
"cmake.cmakePath": "C:\\msys64\\mingw64\\bin\\cmake.exe",
    "cmake.mingwSearchDirs": [
      "C:\\msys64\\mingw64\\bin"
   ],
"cmake.generator": "MinGW Makefiles"
```
- Include the following in your vscode `cmake-tools-kits.json` file (change path values as needed):
```
  {
    "name": "Mingw64 GCC 9.3.0",
    "compilers": {
      "C": "C:\\msys64\\mingw64\\bin\\gcc.exe",
      "CXX": "C:\\msys64\\mingw64\\bin\\g++.exe"
    },
    "preferredGenerator": {
      "name": "MinGW Makefiles",
      "platform": "x64"
    },
    "environmentVariables": {
      "CMT_MINGW_PATH": "C:/msys64/mingw64/bin/"
    }
  }
```
- Add `%ICAROUS_HOME%\Modules\lib` to the windows path.

## Compiling Cython modules

If you don't intend to use the simulation framework, you can skip this step. 

### Install dependencies

Pycarous requires several python packages. These packages can be easily installed as follows:

    pip3 install -r requirements.txt

Note: If you are using windows, consult the [Gotchas](#Gotchas-on-Windows) section at the end.

### Generate Cython bindings

```
python3 setup.py build_ext --inplace
```

### Gotchas on Windows

- The python3 version should have been compiled using the same compiler used to generate the core Modules. For the Modules, we've recommended using the MinGW compiler tool chain. If you are using msys2 as recommended, you should be able to install a compatible python3 and python3-pip version from [msys2](https://www.msys2.org/).
- Make sure the windows environment path variable contains the `bin` folder where python3 and python3-pip are installed. The default place is `C:\msys64\mingw64\bin`.
- Unfortunately, not all packages can be installed via pip3. We recommend looking at the `requirements.txt` file for required packages and manually installing them via msys2. msys2 contains MinGW compatible package distributions. You should be able to search for the packages on msys2 as follows:
```
pacman -S package_name 
```

## Compiling cFS

List the apps to be compiled and run in [Icarous_defs/apps.cmake]().

```
# Run make to compile
make 
# Install the executable
make install
```

Please refer to the section on executing ICAROUS to learn how to configure ICAROUS appropriately and run ICAROUS.
