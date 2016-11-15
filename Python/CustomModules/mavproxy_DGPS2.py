#!/usr/bin/env python
'''
support for a GCS attached DGPS system
'''

import serial
import socket, errno
from pymavlink import mavutil
from MAVProxy.modules.lib import mp_module

class DGPSModule2(mp_module.MPModule):
    def __init__(self, mpstate):
        super(DGPSModule2, self).__init__(mpstate, "DGPS2", "DGPS2 injection support")                
        self.port = serial.Serial("/dev/ttyUSB1")
        
        

    def idle_task(self):
        '''called in idle time'''
        
        data = self.port.read(200)
        
        if len(data) > 110:
            print("DGPS data too large: %u bytes" % len(data))
            return
        try:
            self.master.mav.gps_inject_data_send(
                self.target_system,
                self.target_component,
                len(data),
                bytearray(data.ljust(110, '\0')))

        except Exception,e:
            print "DGPS Failed:", e

def init(mpstate):
    '''initialise module'''
    return DGPSModule2(mpstate)
