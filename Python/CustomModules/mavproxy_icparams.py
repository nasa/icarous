"""
    MAVProxy geofence module
"""
import os, time, platform, math
from pymavlink import mavutil
from MAVProxy.modules.lib import mp_util
from MAVProxy.modules.lib import mp_module
from MAVProxy.modules.mavproxy_map import mp_slipmap
from MAVProxy.modules.lib import mp_settings
from MAVProxy.modules.lib.mp_menu import *  # popup menus

if mp_util.has_wxpython:
    from MAVProxy.modules.lib.mp_menu import *
    
class ICParamModule(mp_module.MPModule):
    def __init__(self, mpstate):
        super(ICParamModule, self).__init__(mpstate, "icparams", "ICAROUS parameters", public = True)
        
        self.add_command('icparams', self.load_params,
                         "load parameters",
                         ["load FILENAME"])
 
        self.menu_added_console = False
        self.menu_added_map = False
        
    def idle_task(self):
        '''called on idle'''
        if self.module('console') is not None and not self.menu_added_console:
            self.menu_added_console = True
            self.module('console').add_menu(self.menu)
        if self.module('map') is not None and not self.menu_added_map:
            self.menu_added_map = True
            self.module('map').add_menu(self.menu)

    def mavlink_packet(self, m):
        x = 1

    def load_params(self, args):
        '''fence commands'''
        if len(args) < 1:
            self.print_usage()
            return        
        elif args[0] == "load":
            if len(args) != 2:
                print len(args)
                self.print_usage();                
                return
            else:
                try:
                    f = open(args[1], mode='r')
                except:
                    print("Failed to open file '%s'" % args[1])

                for line in f:
                    line = line.replace('=',' ')
                    line = line.strip()
                    if not line or line[0] == "#":
                        continue
                    a = line.split()
                    if len(a) < 1:
                        print("Invalid line: %s" % line)
                        continue
                    self.mav_param.mavset(self.master,a[0],a[1])
            
        
        else:
            self.print_usage()

    def print_usage(self):
        print("usage: icparams load FILENAME")
        

def init(mpstate):
    '''initialise module'''
    return ICParamModule(mpstate)
