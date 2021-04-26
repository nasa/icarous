#!/usr/bin/env python3

import sys
from ichelper import IsolateEUTLPlans

eutlFile = sys.argv[1]
prefix = sys.argv[2]
zeroStart = True if sys.argv[3].lower() == 'true' else False
randomize = True if sys.argv[4].lower() == 'true' else False

IsolateEUTLPlans(eutlFile,prefix,bool(zeroStart),bool(randomize))
