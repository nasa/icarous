##
## Debug Options for build
##
DEBUG_OPTS = -O1
DEBUG_LEVEL = 0

##
## Table Debug options.
## In order for the elf2cfetbl utility to work
## The -g option must be used. It generates the symbols
## in the object file
##
## For some platforms the -mno-align-int option is needed
##
TABLE_DEBUG_OPTS = -O3

