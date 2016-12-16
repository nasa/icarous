"""
    MAVProxy DAA module
"""
#import os, time, platform, math
from pymavlink import mavutil
from MAVProxy.modules.lib import mp_util
from MAVProxy.modules.lib import mp_module
from MAVProxy.modules.mavproxy_map import mp_slipmap
from MAVProxy.modules.lib import mp_settings
from MAVProxy.modules.lib.mp_menu import *  # popup menus

if mp_util.has_wxpython:
    from MAVProxy.modules.lib.mp_menu import *

class DAA:
    def __init__(self):

        self.open = False;		#flag indicating if a device is loaded


class DAAModule(mp_module.MPModule):
    def __init__(self, mpstate):
        super(DAAModule, self).__init__(mpstate, "DAA", "DAA management", public = True)
        

        self.add_command('DAA', self.load_traffic_dev,
                         "open traffic dev",
                         ["dev device"])
 
        self.menu_added_console = False
        self.menu_added_map = False
        self.traffic_list = [];
        self.traffic_on_map = [];
        self.WCV = False;
        self.radius = 10.0;

        
    def idle_task(self):
        '''called on idle'''


        if self.module('console') is not None and not self.menu_added_console:
            self.menu_added_console = True
            self.module('console').add_menu(self.menu)
        if self.module('map') is not None and not self.menu_added_map:
            self.menu_added_map = True
            self.module('map').add_menu(self.menu)

	#recive packet if gps packet
        data = self.mav.recv_match(type='GLOBAL_POSITION_INT', blocking=False)
        
        #if data contains data
        if data is not None:
            #print data

            #format data as expected by ICAROUS
            self.intr_lat = float(data.lat)/10000000
            self.intr_lon = float(data.lon)/10000000
            self.intr_ralt= float(data.relative_alt)/1000 	#in  m
            self.intr_hdg = float(data.hdg)/100			#in degrees	

            self.intr_vx = float(data.vx)/100			#in m
            self.intr_vy = float(data.vy)/100			#in m
            self.intr_vz = float(data.vz)/100			#in m

            #if a device has been open, update traffic
            if self.open is True:
                self.Update_traffic();

        
            
                                
    def mavlink_packet(self, m):
        '''handle and incoming mavlink packet'''                        

        #self.Update_traffic();                        

        if(self.open is True):
            wcv_volume = mp_slipmap.SlipCircle("well_clear_volume", 3,\
                                               (self.module('map').lat,self.module('map').lon),\
                                               self.radius,\
                                               (0, 0, 255), linewidth=2)                                                
        
        
        self.mpstate.map.add_object(wcv_volume)               
        
        if m.get_type() == "TRAFFIC_INFO":
            print m.breach_status                    





    def load_traffic_dev(self, args):
        
        if args[0] == "dev":

            self.mav = mavutil.mavlink_connection(args[1], source_system=255, dialect="ardupilotmega", input = True)
	    self.open = True

        elif args[0] == "radius":
            if len(args) == 2:
                self.radius = float(args[1]);
        else:
            print 'usage: traffic dev "address of mav"'



    def Update_traffic(self):
        '''Update traffic icon on map'''

        #place info in local variable becuase map.set_position will not accept it otherwise
        i_lat = self.intr_lat;
        i_lon = self.intr_lon;
        i_hdg = self.intr_hdg;

        vehicle = 'Traffic1'
        colour = "blue"
        vehicle_type = "copter"
        icon = self.mpstate.map.icon(colour + vehicle_type + '.png')
                
        self.mpstate.map.add_object(mp_slipmap.SlipIcon(vehicle, (0,0), icon, layer=3, rotation=0, follow=False, trail=mp_slipmap.SlipTrail()))
        

        self.traffic_on_map.append(vehicle)
        self.mpstate.map.set_position(vehicle, (i_lat, i_lon), rotation=i_hdg)

        v_num = 1
        self.master.mav.command_long_send(
            1,  # target_system
            0, # target_component
            mavutil.mavlink.MAV_CMD_SPATIAL_USER_1, # command
            0, # confirmation
            v_num, # param1
            self.intr_vx, # param2
            self.intr_vy, # param3
            self.intr_vz, # param4
            i_lat, # param5
            i_lon, # param6
            self.intr_ralt) # param7

'''
        print (1,  # target_system
            0, # target_component
            mavutil.mavlink.MAV_CMD_SPATIAL_USER_1, # command
            0, # confirmation
            v_num, # param1
            self.intr_vx, # param2
            self.intr_vy, # param3
            self.intr_vz, # param4
            i_lat, # param5
            i_lon, # param6
            self.intr_ralt)
'''

def init(mpstate):
    '''initialise module'''
    return DAAModule(mpstate)
