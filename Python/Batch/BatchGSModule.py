import os, time, platform
import xml.etree.ElementTree as ET
from pymavlink import mavutil, mavwp
import pymavlink

from pymavlink.dialects.v10 import icarous

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

class BatchGSModule():
    def __init__(self, master,target_system,target_component):
        self.master = master
        self.fenceList = []
        self.sentFenceList = []
        self.fenceToSend = 0
        self.have_list = False
        self.wploader = mavwp.MAVWPLoader()
        self.wp_op = None
        self.wp_requested = {}
        self.wp_received = {}
        self.wp_save_filename = None
        self.wploader = mavwp.MAVWPLoader()
        self.loading_waypoints = False
        self.loading_waypoint_lasttime = time.time()
        self.last_waypoint = 0
        self.wp_period = mavutil.periodic_event(0.5)
        self.target_system = target_system
        self.target_component = target_component
        self.traffic_list = []
        self.start_lat = 0
        self.start_lon = 0
        
        def loadGeofence(self, filename):
        '''load fence points from a file'''
        try:
            self.GetGeofence(filename)
        except Exception as msg:
            print("Unable to load %s - %s" % (filename, msg))
            return

        for fence in self.fenceList:
            if fence not in self.sentFenceList:
                self.Send_fence(fence)

    def StartMission(self):
        self.master.mav.command_long_send(self.target_system, self.target_component,
                                          mavutil.mavlink.MAV_CMD_MISSION_START,
                                          0, 0, 0, 0, 0, 0, 0, 0)

    def Send_fence(self, fence):
        '''send fence points from fenceloader'''
        target_system = 2
        target_component = 0
        self.master.mav.command_long_send(target_system, target_component,
                                          mavutil.mavlink.MAV_CMD_DO_FENCE_ENABLE, 0,
                                          0, fence["id"], fence["type"], fence["numV"],
                                          fence["floor"], fence["roof"], 0)

        fence_sent = False

        while (not fence_sent):

            msg = None
            while (msg == None):
                msg = self.master.recv_match(blocking=True, type=["FENCE_FETCH_POINT", "COMMAND_ACK"], timeout=50)

            if (msg.get_type() == "FENCE_FETCH_POINT"):
                print "received fetch point"
                numV = fence["numV"]
                lat = fence["Vertices"][msg.idx][0]
                lon = fence["Vertices"][msg.idx][1]

                self.master.mav.fence_point_send(1, 0, msg.idx, numV, lat, lon)


            elif (msg.get_type() == "COMMAND_ACK"):
                if msg.result == 1:
                    fence_sent = True
                    print ("Geofence sent")
                else:
                    self.Send_fence(fence)
                    fence_sent = True

        points = fence["Vertices"][:]
        points.append(points[0])
        return

    def GetGeofence(self, filename):

        tree = ET.parse(filename)
        root = tree.getroot()

        for child in root:
            id = int(child.get('id'))
            type = int(child.find('type').text)
            numV = int(child.find('num_vertices').text)
            floor = float(child.find('floor').text)
            roof = float(child.find('roof').text)
            Vertices = []

            if (len(child.findall('vertex')) == numV):
                for vertex in child.findall('vertex'):
                    coord = (float(vertex.find('lat').text),
                             float(vertex.find('lon').text))

                    Vertices.append(coord)

                Geofence = {'id': id, 'type': type, 'numV': numV, 'floor': floor,
                            'roof': roof, 'Vertices': Vertices}

                self.fenceList.append(Geofence)

    def loadWaypoint(self,filename):
        '''load waypoints from a file'''
        self.wploader.target_system = self.target_system
        self.wploader.target_component = self.target_component
        try:
            self.wploader.load(filename)
        except Exception as msg:
            print("Unable to load %s - %s" % (filename, msg))
            return
        print("Loaded %u waypoints from %s" % (self.wploader.count(), filename))
        self.send_all_waypoints()
        while self.loading_waypoints:
            reqd_msgs = ['WAYPOINT_COUNT', 'MISSION_COUNT',
                         'WAYPOINT', 'MISSION_ITEM',
                         'WAYPOINT_REQUEST', 'MISSION_REQUEST'
                         'MISSION_ACK']            
            data = self.master.recv_msg()
            if data is not None:
                self.mavlink_packet_wp(data)

    def send_all_waypoints(self):
        '''send all waypoints to vehicle'''
        self.master.waypoint_clear_all_send()
        if self.wploader.count() == 0:
            return
        self.loading_waypoints = True
        self.loading_waypoint_lasttime = time.time()
        self.master.waypoint_count_send(self.wploader.count())

    def missing_wps_to_request(self):
        ret = []
        tnow = time.time()
        next_seq = self.wploader.count()
        for i in range(5):
            seq = next_seq + i
            if seq + 1 > self.wploader.expected_count:
                continue
            if seq in self.wp_requested and tnow - self.wp_requested[seq] < 2:
                continue
            ret.append(seq)
        return ret

    def send_wp_requests(self, wps=None):
        '''send some more WP requests'''
        if wps is None:
            wps = self.missing_wps_to_request()
        tnow = time.time()
        for seq in wps:
            # print("REQUESTING %u/%u (%u)" % (seq, self.wploader.expected_count, i))
            self.wp_requested[seq] = tnow
            self.master.waypoint_request_send(seq)

    def process_waypoint_request(self, m, master):
        '''process a waypoint request from the master'''
        if (not self.loading_waypoints or
                    time.time() > self.loading_waypoint_lasttime + 10.0):
            self.loading_waypoints = False
            print("not loading waypoints")
            return
        if m.seq >= self.wploader.count():
            print("Request for bad waypoint %u (max %u)" % (m.seq, self.wploader.count()))
            return
        wp = self.wploader.wp(m.seq)
        wp.target_system = self.target_system
        wp.target_component = self.target_component
        self.master.mav.send(self.wploader.wp(m.seq))
        self.loading_waypoint_lasttime = time.time()
        print("Sent waypoint %u : %s" % (m.seq, self.wploader.wp(m.seq)))
        if m.seq == self.wploader.count() - 1:
            self.loading_waypoints = False
            print("Sent all %u waypoints" % self.wploader.count())

    def mavlink_packet_wp(self, m):
        '''handle an incoming mavlink packet'''
        mtype = m.get_type()
        if mtype in ['WAYPOINT_COUNT', 'MISSION_COUNT']:
            if self.wp_op is None:
                print "No waypoint load started"
            else:
                self.wploader.clear()
                self.wploader.expected_count = m.count
                print("Requesting %u waypoints t=%s now=%s" % (m.count,
                                                                              time.asctime(
                                                                                  time.localtime(m._timestamp)),
                                                                              time.asctime()))
                self.send_wp_requests()

        elif mtype in ['WAYPOINT', 'MISSION_ITEM'] and self.wp_op != None:
            if m.seq < self.wploader.count():
                # print("DUPLICATE %u" % m.seq)
                return
            if m.seq + 1 > self.wploader.expected_count:
                print("Unexpected waypoint number %u - expected %u" % (m.seq, self.wploader.count()))
            self.wp_received[m.seq] = m
            next_seq = self.wploader.count()
            while next_seq in self.wp_received:
                m = self.wp_received.pop(next_seq)
                self.wploader.add(m)
                next_seq += 1
            if self.wploader.count() != self.wploader.expected_count:
                # print("m.seq=%u expected_count=%u" % (m.seq, self.wploader.expected_count))
                self.send_wp_requests()
                return
            if self.wp_op == 'list':
                for i in range(self.wploader.count()):
                    w = self.wploader.wp(i)
                    print("%u %u %.10f %.10f %f p1=%.1f p2=%.1f p3=%.1f p4=%.1f cur=%u auto=%u" % (
                        w.command, w.frame, w.x, w.y, w.z,
                        w.param1, w.param2, w.param3, w.param4,
                        w.current, w.autocontinue))
            self.wp_op = None
            self.wp_requested = {}
            self.wp_received = {}

        elif mtype in ["WAYPOINT_REQUEST", "MISSION_REQUEST"]:
            self.process_waypoint_request(m, self.master)

        elif mtype in ["WAYPOINT_CURRENT", "MISSION_CURRENT"]:
            if m.seq != self.last_waypoint:
                self.last_waypoint = m.seq

    
    def LLA2NED(self,origin,position):
        """
        Convert from geodetic coordinates to NED coordinates
        :param origin:  origin of NED frame in geodetic coordinates
        :param position: position to be converted to NED
        :return: returns position in NED
        """
        R    = 6371000  # radius of earth
        oLat = origin[0]*np.pi/180
        oLon = origin[1]*np.pi/180
        
        if(len(origin) > 2):
            oAlt = origin[2]
        else:
            oAlt = 0
            
        pLat = position[0]*np.pi/180
        pLon = position[1]*np.pi/180
            
        if(len (origin) > 2):
            pAlt = position[2]
        else:
            pAlt = 0

        # convert given positions from geodetic coordinate frame to ECEF
        oX   = (R+oAlt)*cos(oLat)*cos(oLon)
        oY   = (R+oAlt)*cos(oLat)*sin(oLon)
        oZ   = (R+oAlt)*sin(oLat)

        Pref = np.array([[oX],[oY],[oZ]])
        
        pX   = (R+pAlt)*cos(pLat)*cos(pLon)
        pY   = (R+pAlt)*cos(pLat)*sin(pLon)
        pZ   = (R+pAlt)*sin(pLat)
        
        P    = np.array([[pX],[pY],[pZ]])
        
        # Convert from ECEF to NED
        Rne  = np.array([[-sin(oLat)*cos(oLon), -sin(oLat)*sin(oLon), cos(oLat)],
                         [-sin(oLon),                cos(oLon),          0     ],
                         [-cos(oLat)*cos(oLon), -cos(oLat)*sin(oLon),-sin(oLat)]])
    
        Pn   = np.dot(Rne,(P - Pref))

        if(len (origin) > 2):
            return [Pn[0,0], Pn[1,0], Pn[2,0]]
        else:
            return [Pn[0,0], Pn[1,0]]
                
    def load_traffic(self,args):
        start_time = time.time();
        tffc = Traffic(float(args[1]),float(args[2]),float(args[3]), \
                       float(args[4]),float(args[5]),float(args[6]),start_time)
        self.traffic_list.append(tffc)

    def Update_traffic(self):
        '''Update traffic icon on map'''
        
        #from MAVProxy.modules.mavproxy_map import mp_slipmap
        t = time.time()
        
        for i,tffc in enumerate(self.traffic_list):
                                                                                
            self.traffic_list[i].get_pos(t)
            (lat, lon) = self.LLA2NED(self.start_lat,self.start_lon, self.traffic_list[i].y, self.traffic_list[i].x)
            heading = math.degrees(math.atan2(self.traffic_list[i].vy0, self.traffic_list[i].vx0))            
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
        
                
