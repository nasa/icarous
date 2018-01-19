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


class Traffic:
    def __init__(self,distH,bearing,z,S,H,V,tstart):
        self.x0 = distH*math.sin((90 - bearing)*3.142/180);
        self.y0 = distH*math.cos((90 - bearing)*3.142/180);
        self.z0 = z;

        self.vx0 = S*math.sin((90 - H)*3.142/180);
        self.vy0 = S*math.cos((90 - H)*3.142/180);
        self.vz0 = V;

        self.x = self.x0;
        self.y = self.y0;
        self.z = self.z0;

        self.tstart = tstart

    def get_pos(self,t):
        dt = t-self.tstart
        
        self.x = self.x0 + self.vx0*(dt)
        self.y = self.y0 + self.vy0*(dt)
        self.z = self.z0 + self.vz0*(dt)
    
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
        self.WCV = False;
        self.radius = 5.0;
        self.V2V = False;


        self.numBands = 0;
        self.oldNumBands = 0;
        self.Bands = [];
        self.kmbMsgCounter = 0;
        self.gotStart = False;
        self.lastUpdateTime = 0
        
        
        
    def idle_task(self):
        '''called on idle'''
        if self.module('console') is not None and not self.menu_added_console:
            self.menu_added_console = True
            self.module('console').add_menu(self.menu)
        if self.module('map') is not None and not self.menu_added_map:
            self.menu_added_map = True
            self.module('map').add_menu(self.menu)

        
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

        
        self.Update_traffic()

        if(len(self.traffic_list) > 0):
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
            print m.breach_status

        if m.get_type() == "COMMAND_LONG":
            if self.V2V:
                self.show_received_traffic(m)

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
                if not self.gotStart:
                    self.start_lat = self.module('map').lat
                    self.start_lon = self.module('map').lon
                    self.gotStart = True
                print len(self.traffic_list)
        elif args[0] == "radius":
            if len(args) == 2:
                self.radius = float(args[1]);

        elif args[0] == "V2V":
            self.V2V = True
            
        
        else:
            self.print_usage()

    def show_received_traffic(self, msg):
        """ Dislay received traffic """

        id = int(msg.param1)
        vn = msg.param2
        ve = msg.param3
        vd = msg.param4
        lat = msg.param5
        lon = msg.param6
        alt = msg.param7

        vehicle = 'Traffic%d' % id

        if (vehicle not in self.traffic_on_map):
            colour = "blue"
            vehicle_type = "copter"
            icon = self.mpstate.map.icon(colour + vehicle_type + '.png')

            self.mpstate.map.add_object(mp_slipmap.SlipIcon(vehicle, (0, 0), icon, layer=3, rotation=0, follow=False, \
                                                            trail=mp_slipmap.SlipTrail(colour=(0, 255, 0))))
            self.traffic_on_map.append(vehicle)

        heading = math.degrees(math.atan2(ve, vn))
        self.mpstate.map.set_position(vehicle, (lat, lon), rotation=heading)


    def Update_traffic(self):
        '''Update traffic icon on map'''
        
        #from MAVProxy.modules.mavproxy_map import mp_slipmap
        t = time.time()
        if(t - self.lastUpdateTime < 0.5):
            return
        
        for i,tffc in enumerate(self.traffic_list):
            vehicle = 'Traffic%d' % i
            
            if(vehicle not in self.traffic_on_map):
                
                colour = "blue"
                vehicle_type = "copter"
                icon = self.mpstate.map.icon(colour + vehicle_type + '.png')
                
                self.mpstate.map.add_object(mp_slipmap.SlipIcon(vehicle, (0,0), icon, layer=3, rotation=0, follow=False, \
                                                                trail=mp_slipmap.SlipTrail(colour=(0,255,0))))
                self.traffic_on_map.append(vehicle)
                                                        
            self.traffic_list[i].get_pos(t)
            (lat, lon) = mp_util.gps_offset(self.start_lat,self.start_lon, self.traffic_list[i].y, self.traffic_list[i].x)
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
        self.lastUpdateTime = t
            

    def print_usage(self):
        print("usage: traffic load range bearing height speed heading verticalspeed")
        

def init(mpstate):
    '''initialise module'''
    return TrafficModule(mpstate)
