#!/bin/bash

# Apply plexil patches
patch -d ../cFS/apps/plexil/ < plexil.patch

# Apply elf utility patch
patch -d ../cFS/cFE/tools/elf2cfetbl < elfutility.patch
