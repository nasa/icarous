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

## print("Reading %s" % args[0])
next = False
for line in infile:
    linestr = line.strip()
    if linestr != "" and not re.match('#',linestr):
          lstln = linestr.split(",")
          if lstln[0] == "id":
              next = True
          elif next:
              l0 = lstln[0].strip()
              l1 = lstln[1].strip()
              l3 = lstln[3].strip()
              l4 = lstln[4].strip()
              l5 = "0" #lstln[5].strip()
              l6 = lstln[6].strip();
              if l0 == "0" and l1 == "0":
                  wp = l3+","+l4+","+l5+","+l6
                  print(wp)
                  next = False
                  
