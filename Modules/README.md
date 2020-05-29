# ICAROUS Modules

To compile:

```
mkdir build
cd build
cmake ..
make -j8
```

The compiled libraries are located under `lib`

## Setup library paths
Add the `lib` folder to your `DYLD_LIBRARY_PATH` (OSX) or `LD_LIBRARY_PATH` (Linux) variable. We recommend using the export command in your ~/.bash_profile (OSX) or ~/.bashrc (Linux) for persistence. 

For example,

```
# `/path/to/lib` here should refer to the absolute path for the Icarous/Modules/lib folder
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/path/to/lib
```

