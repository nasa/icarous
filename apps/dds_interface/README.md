# DDS-Interface application

ICAROUS provides an application to communicate using OpenSplice DDS. Enable this application as follows:

1. Download and install an OpenSplice DDS 32bit version

2. Create an OpenSplice configuration file (.xml) with **domain id** 100.  Follow the examples provided in the OpenSplice DDS distribution directory within `etc/conf`.

3. Add `dds_interface` to the `TGT1_APPLIST` in the file `cFS/apps/Icarous_defs/targets.cmake` to compile the dds application:

4. Define the following environment variable for building:

     - `OSPL_HOME`: the path of the OpenSplice local installation; for example, `/opt/OpenSplice/HDE/x86.linux`

5. Define the following environment variables before executing ICAROUS:

     - `OSPL_URI`: the URI of the OpenSplice `*.xml` configuration file for **domain id** 100 (the one of *Step 2*); for example, `file:///opt/OpenSplice/HDE/x86.linux/etc/icarous-ospl.xml` (note the three initial slashes)
     - `OSPL_HOME`: the path of the OpenSplice local installation; for example, `/opt/OpenSplice/HDE/x86.linux`
     - `LD_LIBRARY_PATH`: it has to include the library path of the OpenSplice local installation in order for ICAROUS to find them; for example, `/opt/OpenSplice/HDE/x86.linux/lib`

   An example command could look like this:
   
```
OSPL_URI="file:///opt/OpenSplice/HDE/x86.linux/etc/icarous-ospl.xml" \
OSPL_HOME="/opt/OpenSplice/HDE/x86.linux" \
LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/opt/OpenSplice/HDE/x86.linux/lib" \
sudo ./core-cpu1
```


