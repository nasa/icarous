#!/usr/bin/env python
import getopt
import re
from optparse import OptionParser

## Parsing arguments
usage = "usage: %prog [options] file"
parser = OptionParser(usage)
parser.add_option("-o", "--output", dest="outfile",metavar="OUTFILE",help="Write data to OUTFILE")
(options, args) = parser.parse_args()
if len(args) != 1:
    parser.error("Incorrect number of arguments")
try:
    infile = open(args[0],'r')
except IOError:
    parser.error("File "+args[0]+" not found")
outfile = options.outfile 

line = infile.readline()  # ignore the first line (protocol)

next = True;
while(next):
    f = infile.readline();
    if f[0] == "#":
        continue
    else:
        entries = f.split("\t")
        if entries[0] == "0":
            print entries[8]+","+entries[9]+","+entries[10]+",0"
            next = False;



                  
