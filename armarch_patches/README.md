* Apply patches for compilation on ARM. Commands are as follows:
   - cd armarch_patches/
   - bash apply_arm_patches.sh 

* Set pixhawk connection to TX2
   - Open cFS/apps/ardupilot/fsw/tables/intf_tbl.c
   - Change the port parameters located on line 17 (after the #else)
   - Typically you can do "ls /dev/tty*" on the TX2 to list all ports.
   - For example, the pixhawk appears as /dev/ttyTHS2 on our TX2 and we use
     the following settings.

     .PortType = SERIAL,
     .BaudRate = 57600,
     .Portin   = 0,
     .Portout  = 0,
     .Address  = "/dev/ttyTHS2"

* Set ground station parameters
   - Open cFS/apps/gsInterface/fsw/tables/gsIntf_tbl.c
   - Change the port parameters located on line 17 (after the #else)
   - If you are using another radio for the ground station telemetry,
     these setting will be similar to the above example. However, if you are
     connecting a ground station over a network, Then it would like like this

     .PortType = SOCKET,
     .BaudRate = 0,
     .Portin   = 14552,
     .Portout  = 14553,
     .Address  = "192.168.1.2" 

     NOTE: The address here is the IP address of the ground staion laptop. 
           Also make sure that the ground station is listening on the port
           set in .Portout

* ICAROUS compilation requires that java is installed on the TX2. Typically, the
  java jdk for arm can be obtained from
  https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html
  if it is not already installed.

* Once java is installed, specify the location of the installed files in the
  SetEnv.sh script located in the Icarous repository.

* Compile ICAROUS using the following linux commands:
    - source SetEnv.sh
    - mkdir build 
    - cd build
    - cmake .. -DARM=ON
    
* Launch ICAROUS
    - cd cFS/bin/cpu1
    - ./core-cpu1 -C 1 -I 0


