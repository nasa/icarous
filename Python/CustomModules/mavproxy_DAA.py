"""
    MAVProxy DAA module
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

class SlipEllipse(mp_slipmap.SlipObject):
    def __init__(self, key, layer, center, axes, angle, startAngle, endAngle, colour, linewidth, popup_menu=None):
        mp_slipmap.SlipObject.__init__(self, key, layer, popup_menu=popup_menu)
        self.center = center
        self.axes = axes
        self.angle = angle
        self.startAngle = startAngle
        self.endAngle = endAngle
        self.colour = colour
        self.linewidth = linewidth
        self._pix_points = []

    def bounds(self):
        '''return bounding box'''
        if self.hidden:
            return None
        return (self.center[0],self.center[1],0,0)

    def draw(self, img, pixmapper, bounds):
        '''draw a polygon on the image'''
        if self.hidden:
            return

        center = pixmapper(self.center)
        # figure out pixels per meter
        ref_pt = (self.center[0] + 1.0, self.center[1])
        dis = mp_util.gps_distance(self.center[0], self.center[1], ref_pt[0], ref_pt[1])
        ref_px = pixmapper(ref_pt)
        dis_px = math.sqrt(float(center[1] - ref_px[1]) ** 2.0)
        pixels_per_meter = dis_px / dis


        axes0 = int(self.axes[0]*pixels_per_meter)
        axes1 = int(self.axes[1]*pixels_per_meter)
        axes  = (axes0,axes1)
        mp_slipmap.cv2.ellipse(img,center,axes,self.angle,self.startAngle,self.endAngle,self.colour,self.linewidth)

    
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

        self.numBands = 0;
        self.oldNumBands = 0;
        self.Bands = [];
        self.kmbMsgCounter = 0;
        
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

    def AddBand(self,i,bands):

        if bands[2] == 0:
            colour = (0,255,0,100)
        elif bands[2] == 1:
            colour = (255,0,0,100)
        elif bands[2] == 2:
            colour = (255,255,0,100)
        
        center = (self.module('map').lat,self.module('map').lon)
        axes = (self.radius,self.radius)
        angle = -90
        startAngle = bands[0]
        stopAngle = bands[1]
        thickness = -1
        band_guidance = SlipEllipse("band"+str(i),1,center,axes,angle,startAngle,stopAngle,colour,thickness)
        self.mpstate.map.add_object(band_guidance)
            
                                
    def mavlink_packet(self, m):
        '''handle and incoming mavlink packet'''                        

        if(self.open is True):
            self.kmbMsgCounter = self.kmbMsgCounter+1;
            wcv_volume = mp_slipmap.SlipCircle("well_clear_volume", 3,\
                                               (self.module('map').lat,self.module('map').lon),\
                                               self.radius,\
                                               (0, 0, 255), linewidth=2)                                                
        
        
            self.mpstate.map.add_object(wcv_volume)               

            if(self.kmbMsgCounter == 95):
                self.Bands = []

            if m.get_type() == "KINEMATIC_BANDS":
                self.kmbMsgCounter = 0;
                self.oldNumBands = self.numBands;
                self.numBands = m.numBands;
                numBands = 0
                numBands = numBands + 1

                self.Bands = []

                if(numBands <= self.numBands):
                    low    = m.min1
                    high   = m.max1
                    bands  = [low,high,m.type1]
                    self.Bands.append(bands)
                    numBands = numBands + 1
                

                if (numBands <= self.numBands):
                    low = m.min2
                    high = m.max2
                    bands = [low, high,m.type2]
                    self.Bands.append(bands)
                    numBands = numBands + 1

                if (numBands <= self.numBands):
                    low = m.min3
                    high = m.max3
                    bands = [low, high,m.type3]
                    self.Bands.append(bands)
                    numBands = numBands + 1

                if (numBands <= self.numBands):
                    low = m.min4
                    high = m.max4
                    bands = [low, high,m.type4]
                    self.Bands.append(bands)
                    numBands = numBands + 1

                if (numBands <= self.numBands):
                    low = m.min5
                    high = m.max5
                    bands = [low, high,m.type5]
                    self.Bands.append(bands)
                    numBands = numBands + 1

            if (self.oldNumBands > self.numBands) or self.kmbMsgCounter == 100:
                for i in range(self.oldNumBands):
                    self.mpstate.map.remove_object("band" + str(i))

            for i,kmb in enumerate(self.Bands):
                self.AddBand(i,kmb)

        
        if m.get_type() == "TRAFFIC_INFO":
            print(m.breach_status)

            



    def load_traffic_dev(self, args):
        
        if args[0] == "dev":

            self.mav = mavutil.mavlink_connection(args[1], source_system=255, dialect="ardupilotmega", input = True)
	    self.open = True

        elif args[0] == "radius":
            if len(args) == 2:
                self.radius = float(args[1]);
        else:
            print('usage: traffic dev "address of mav"')



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
