#!/usr/bin/env python3

import sys
from ichelper import IsolateEUTLPlans

eutlFile = sys.argv[1]
prefix = sys.argv[2]
randomize = True if sys.argv[3].lower() == 'true' else False

IsolateEUTLPlans(eutlFile,prefix,bool(randomize))
