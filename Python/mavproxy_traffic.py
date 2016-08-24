"""
    MAVProxy geofence module
"""
import os, time, platform, math
import xml.etree.ElementTree as ET
from pymavlink import mavwp, mavutil
from MAVProxy.modules.lib import mp_util
from MAVProxy.modules.lib import mp_module
if mp_util.has_wxpython:
    from MAVProxy.modules.lib.mp_menu import *

class Traffic:
    def __init__(self,x,y,z,vx,vy,vz,tstart):
        self.x0 = x;
        self.y0 = y;
        self.z0 = z;

        self.vx0 = vx;
        self.vy0 = vy;
        self.vz0 = vz;

        self.tstart = tstart

    def get_pos(self,t):
        self.x0 = self.x0 + self.vx0*(0.5)
        self.y0 = self.y0 + self.vy0*(0.5)
        self.z0 = self.z0 + self.vz0*(0.5)
    
class TrafficModule(mp_module.MPModule):
    def __init__(self, mpstate):
        super(TrafficModule, self).__init__(mpstate, "traffic", "traffic management", public = True)
        
        self.add_command('traffic', self.load_traffic,
                         "start traffic",
                         ["load <x,y,z,vx,vy,vz>"])
 
        self.menu_added_console = False
        self.menu_added_map = False
        self.traffic_list = [];
        self.traffic_on_map = [];
        
        
        
    def idle_task(self):
        '''called on idle'''
        if self.module('console') is not None and not self.menu_added_console:
            self.menu_added_console = True
            self.module('console').add_menu(self.menu)
        if self.module('map') is not None and not self.menu_added_map:
            self.menu_added_map = True
            self.module('map').add_menu(self.menu)            

    def mavlink_packet(self, m):
        '''handle and incoming mavlink packet'''                        

        
        self.Update_traffic();
        
        if m.get_type() == "TRAFFIC_INFO":
            print m.breach_status                    
        
                    
    def load_traffic(self, args):
        '''fence commands'''
        if len(args) < 1:
            self.print_usage()
            return        
        elif args[0] == "load":
            if len(args) != 7:
                print len(args)
                self.print_usage();                
                return
            else:
                start_time = time.time();
                tffc = Traffic(float(args[1]),float(args[2]),float(args[3]), \
                               float(args[4]),float(args[5]),float(args[6]),start_time)
                self.traffic_list.append(tffc)
                print len(self.traffic_list)
            
        
        else:
            self.print_usage()

    def Update_traffic(self):
        '''Update traffic icon on map'''
        
        from MAVProxy.modules.mavproxy_map import mp_slipmap
        t = time.time()
        
        for i,tffc in enumerate(self.traffic_list):
            vehicle = 'Traffic%d' % i
            
            if(vehicle not in self.traffic_on_map):
                
                colour = "blue"
                vehicle_type = "copter"
                icon = self.mpstate.map.icon(colour + vehicle_type + '.png')
                
                self.mpstate.map.add_object(mp_slipmap.SlipIcon(vehicle, (0,0), icon, layer=3, rotation=0, follow=False, \
                                                                trail=mp_slipmap.SlipTrail()))
                self.traffic_on_map.append(vehicle)
                                                        
            self.traffic_list[i].get_pos(t);
            (lat, lon) = mp_util.gps_offset(37.1021769,-76.3872069, self.traffic_list[i].y0, self.traffic_list[i].x0)
            heading = math.degrees(math.atan2(self.traffic_list[i].vy0, self.traffic_list[i].vx0))            
            self.mpstate.map.set_position(vehicle, (lat, lon), rotation=heading)

            self.master.mav.command_long_send(
                1,  # target_system
                0, # target_component
                mavutil.mavlink.MAV_CMD_SPATIAL_USER_1, # command
                0, # confirmation
                i, # param1
                self.traffic_list[i].vx0, # param2
                self.traffic_list[i].vy0, # param3
                self.traffic_list[i].vz0, # param4
                lat, # param5
                lon, # param6
                self.traffic_list[i].z0) # param7
            

    def print_usage(self):
        print("usage: traffic load <x,y,z,vx,vy,vz>")
        

def init(mpstate):
    '''initialise module'''
    return TrafficModule(mpstate)
