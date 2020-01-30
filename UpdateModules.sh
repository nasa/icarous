#!/bin/bash
git submodule update --init --recursive cfe
git submodule update --init --recursive osal
git submodule update --init --recursive psp
git submodule update --init --recursive tools/elf2cfetbl
git submodule update --init --recursive tools/gen_sch_tbl
