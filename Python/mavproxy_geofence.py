"""
    MAVProxy geofence module
"""
import os, time, platform
import xml.etree.ElementTree as ET
from pymavlink import mavwp, mavutil
from MAVProxy.modules.lib import mp_util
from MAVProxy.modules.lib import mp_module
if mp_util.has_wxpython:
    from MAVProxy.modules.lib.mp_menu import *

class GeoFenceModule(mp_module.MPModule):
    def __init__(self, mpstate):
        super(GeoFenceModule, self).__init__(mpstate, "geofence", "geo-fence management", public = True)
        
        self.add_command('geofence', self.cmd_fence,
                         "geo-fence management",
                         ["load (FILENAME)"])
        self.fenceList = []
        self.fenceToSend= 0;
        self.have_list = False        
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
        '''handle and incoming mavlink packet'''                        
        
        if m.get_type() == "FENCE_STATUS":
            print m.breach_status
            if m.breach_status == 1:
                self.fenceToSend = self.fenceToSend + 1
                print "Geofence %d of %d sent successfully" % (self.fenceToSend,len(self.fenceList))
                if(self.fenceToSend < len(self.fenceList)):
                    fence = self.fenceList[self.fenceToSend]                  
                    self.send_fence(fence)
        
        
                    
    def cmd_fence(self, args):
        '''fence commands'''
        if len(args) < 1:
            self.print_usage()
            return        
        elif args[0] == "load":
            if len(args) != 2:
                print("usage: fence load <filename>")
                return
            self.load_fence(args[1])
        
        else:
            self.print_usage()

    def load_fence(self, filename):
        '''load fence points from a file'''
        try:
            self.GetGeofence(filename)
        except Exception as msg:
            print("Unable to load %s - %s" % (filename, msg))
            return

        for fence in self.fenceList:
            self.Send_fence(fence)
            

    def Send_fence(self,fence):
        '''send fence points from fenceloader'''
        target_system    = 2;
        target_component = 0;

        self.master.mav.command_long_send(target_system,target_component,
                                          mavutil.mavlink.MAV_CMD_DO_FENCE_ENABLE,0,
                                          0,fence["id"],fence["type"],fence["numV"],
                                          fence["floor"],fence["roof"],0);

        print("sent fence count")

        fence_sent = False;
                            
        while(not fence_sent):

            msg = None
            while(msg == None):
                msg = self.master.recv_msg();
                
                
            if(msg.get_type() == "FENCE_FETCH_POINT"):            

                numV = fence["numV"]
                lat  = fence["Vertices"][msg.idx][0]
                lon  = fence["Vertices"][msg.idx][1]
                
                self.master.mav.fence_point_send(2,0,msg.idx,numV,lat,lon)            
                    
                
            elif(msg.get_type() == "COMMAND_ACK" ):
                
                if msg.result == 1:            
                    fence_sent = True;
                    print ("Geofence sent")
                else:
                    self.Send_fence(fence);
                    fence_sent = True;

        points = fence["Vertices"]
        points.append(points[0])
        
        from MAVProxy.modules.mavproxy_map import mp_slipmap
        name = 'Fence'+str(fence["id"])
        self.mpstate.map.add_object(mp_slipmap.SlipPolygon(name, points, layer=1,
                                                               linewidth=2, colour=(0,255,0)))

        return
                

    def fetch_fence_point(self):
        '''fetch one fence point'''
        target_system = 2;
        target_component = 0;
      
        tstart = time.time()
        p = None
        while time.time() - tstart < 5:
            p = self.master.recv_match(type='FENCE_FETCH_POINT', blocking=False)
            if p is not None:
                break
            time.sleep(0.1)
            continue
        if p is None:
            self.console.error("Did not receive fence point")
            return None
        return p

   
    def print_usage(self):
        print("usage: geofence load <filename>")
           
    def GetGeofence(self,filename):

        tree = ET.parse(filename)
        root = tree.getroot()        
        
        for child in root:
            id    = int(child.get('id'));
            type  = int(child.find('type').text);
            numV  = int(child.find('num_vertices').text);            
            floor = float(child.find('floor').text);
            roof  = float(child.find('roof').text);
            Vertices = [];
        
            if(len(child.findall('vertex')) == numV):        
                for vertex in child.findall('vertex'):
                    coord = (float(vertex.find('lat').text),
                             float(vertex.find('lon').text))
                
                    Vertices.append(coord)

                Geofence = {'id':id,'type': type,'numV':numV,'floor':floor,
                            'roof':roof,'Vertices':Vertices}

                self.fenceList.append(Geofence)
        

def init(mpstate):
    '''initialise module'''
    return GeoFenceModule(mpstate)
