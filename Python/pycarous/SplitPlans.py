#!/usr/bin/env python3

import sys
from ichelper import IsolateEUTLPlans

eutlFile = sys.argv[1]
prefix = sys.argv[2]

IsolateEUTLPlans(eutlFile,prefix)
