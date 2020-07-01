So you'd like to test SBN? Here's some suggestions:

### Configuration

1. Set up at least two targets in the cFS build system (targets.cmake).

    a. Target 1 - "cpu1" with the applist of "ci_lab sbn_app sbn_udp sbn_tcp"

    b. Target 2 - "cpu2" with the applist of "to_lab sbn_app sbn_udp sbn_tcp"

2. Configure ES startup scripts:

    a. cpu1:

>CFE_APP, /cf/sbn.so, SBN_AppMain, SBN, 80, 16384, 0x0, 0;
>CFE_APP, /cf/ci_lab.so, CI_Lab_AppMain, CI_LAB, 80, 16384, 0x0, 0;
>!

    b. cpu2:

>CFE_APP, /cf/sbn.so, SBN_AppMain, SBN, 80, 16384, 0x0, 0;
>CFE_APP, /cf/to_lab.so, TO_Lab_AppMain, TO_LAB, 80, 16384, 0x0, 0;
>!

3. Make sure CPU_PLATFORM_CPU_ID and CPU_PLATFORM_CPU_NAME are correct in cpu*_platform_cfg.h (e.g. "1" and "CPU1" for cpu1_platform_cfg.h) Otherwise their contents should be identical between files.

4. cpu*_msgids.h can be all identical. Same with toolchain-cpu*.cmake.

5. Copy `to_lab_sub_table.h` to `apps/to_lab/fsw/platform_inc`.

### Building

1. Follow the standard build, but with a `make mission-install` as the final step to create (by default) `/usr/local/cpu*`.

### Running

1. In one window, cd to `/usr/local/cpu1` and start the instance with `./core-cpu1 -R PO`.
2. In a second window, cd to `/usr/local/cpu2` and start the instance with `./core-cpu2 -R PO`. -- Confirm you see "CPU N connected" on each.
3. In a third window, start `to_recv`.
4. In a fourth window, run the `to_start` command to start telemetry output.
5. In that fourth window, run the command `testsend` to send a test telemetry packet to CPU1, which should be relayed to CPU2 and down to to_recv to be displayed in the third window.
