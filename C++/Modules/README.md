
Multi Arch Support
------------------

If you are using a 64bit architecture, see https://wiki.debian.org/Multiarch/HOWTO to enable multi-arch support. CFS builds 32 bit binaries and hence you will have to enable multi-arch support.

-lwrap library not found
------------------------

- use find / -name "libwrap" to check if the tcp wrapper library is avaiable. It is typically located under /lib/[ARCH] where ARCH can be x86_64-linux-gnu or i386-linux-gnu.
- If libwrap is not available, install it
  - sudo apt-get install tcpd
  - sudo apt-get install tcpd:i386 (for 32bit libraries)

- Make sure libwrap.so is symbolically linked to the library soname (i.e. libwrap.so.x) which in turn is a symbolic link to the real name libwrap.so.x.x.x

Multiib
-------------------------
- make sure g++multlib, gcc-multilib are installed/updated.
  