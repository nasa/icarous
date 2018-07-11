"""
    MAVProxy geofence module
"""
import os, time, platform
import xml.etree.ElementTree as ET
import numpy as np
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
        self.sentFenceList = []
        self.fenceToSend= 0;
        self.have_list = False        
        self.menu_added_console = False
        self.menu_added_map = False
        self.fetchPointMessage = None
        self.sentVertexCount = 0
        self.fence2send = None
        self.sentFence = False
        self.t1 = 0
        self.t2 = 0
        self.numSentFence = 0
        self.communicating = False

        if mp_util.has_wxpython:
            self.menu = MPMenuSubMenu('Geofence',
                                  items=[MPMenuItem('Clear', 'Clear', '# geofence clear'),
                                         MPMenuItem('Save', 'Save', '# geofence save ',
                                                    handler=MPMenuCallFileDialog(flags=('save', 'overwrite_prompt'),
                                                                                 title='Fence Save',
                                                                                 wildcard='*.xml')),
                                         MPMenuItem('Load', 'Load', '# geofence load ',
                                                    handler=MPMenuCallFileDialog(flags=('open',),
                                                                                 title='Geofence Load',
                                                                                 wildcard='*.txt')),                                       
                                         MPMenuItem('Draw', 'Draw', '# geofence draw ',
                                                    handler=MPMenuCallTextDialog(title='Fence info:id,type [0 in/1 out],#vertices,floor [m],ceiling [m]',
                                                                                 default='0,0,0,0,0'))])
        
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

        if self.startSendingFence:
            self.t2 = time.time()
            if self.t2 - self.t1 > 10:
                self.startSendingFence = False
                self.communicating = False
                self.fence2send = None
                self.console.writeln("Sending failed")
            if self.numSentFence == len(self.fenceList):
                self.startSendingFence = False
                self.communicating = False
                self.fence2send = None
            elif self.fence2send is None:
                self.commmunicating = True
                self.fence2send = self.fenceList[self.numSentFence]
                target_system    = 2;
                target_component = 0;
                fence = self.fence2send
                self.console.writeln("sending fence description")
                self.master.mav.command_long_send(target_system,target_component,
                                          mavutil.mavlink.MAV_CMD_DO_FENCE_ENABLE,0,
                                          0,fence["id"],fence["type"],fence["numV"],
                                          fence["floor"],fence["roof"],0);    

        if m.get_type() == "FENCE_FETCH_POINT":
            self.t1 = time.time()
            fence = self.fence2send
            numV = fence["numV"]
            lat  = fence["Vertices"][m.idx][0]
            lon  = fence["Vertices"][m.idx][1]
            
            self.master.mav.fence_point_send(2,0,m.idx,numV,lat,lon)            
            self.console.writeln("sending vertex %u" % m.idx)
            self.sentVertexCount = m.idx+1
     

        if m.get_type() == "COMMAND_ACK":
            self.t1 = time.time()
            if self.sentVertexCount == self.fence2send["numV"] and m.result == 0:
                self.numSentFence = self.numSentFence + 1
                self.console.writeln("Geofence sent")

                points = self.fence2send["Vertices"][:]
                points.append(points[0])
        
                from MAVProxy.modules.mavproxy_map import mp_slipmap
                name = 'Fence'+str(self.fence2send["id"])

                if(self.fence2send["type"] == 0):
                    gcf = (255,190,0)
                else:
                    gcf = (255,0,0)
            
                self.mpstate.map.add_object(mp_slipmap.SlipPolygon(name, points, layer=1,
                                                               linewidth=2, colour=gcf))
                self.fence2send = None

                
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

        elif args[0] == "save":
            if(len(args) != 2):
               return
            else:
               print "saving"
               self.save_geofence(args[1])
            
        elif args[0] == "clear":
            for fence in self.fenceList:
                self.clear_fence(fence);

        elif args[0] == "draw":
            if not 'draw_lines' in self.mpstate.map_functions:
                print("No map drawing available")
                            
            params = args[1].split(',');
            self.Geofence0 = {'id':int(params[0]),
                              'type':int(params[1]),
                              'numV':int(params[2]),
                              'floor':float(params[3]),
                              'roof':float(params[4]),
                              'Vertices':[]}

            
            self.mpstate.map_functions['draw_lines'](self.geofence_draw_callback)
            print "Drawing geofence on map with %d vertices" % int(params[2])
        
        else:
            self.print_usage()

    def load_fence(self, filename):
        '''load fence points from a file'''
        try:
            self.fenceList = []  # uncomment this line if you want to send fences from different files
            self.GetGeofence(filename)
        except Exception as msg:
            print("Unable to load %s - %s" % (filename, msg))
            return

        if len(self.fenceList) > 0:
            self.t1 = time.time()
            self.t2 = time.time()
            self.numSentFence = 0
            self.startSendingFence = True


    def clear_fence(self,fence):

        '''send fence points from fenceloader'''
        target_system    = 2;
        target_component = 0;

        self.master.mav.command_long_send(target_system,target_component,
                                          mavutil.mavlink.MAV_CMD_DO_FENCE_ENABLE,0,
                                          0,fence["id"],fence["type"],0,
                                          fence["floor"],fence["roof"],0);

        name = 'Fence'+str(fence["id"])
        self.mpstate.map.remove_object(name);
        

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

    def geofence_draw_callback(self, points):
        '''callback from drawing waypoints'''
        if len(points) != self.Geofence0["numV"]:            
            print "Insufficient points in polygon, try drawing polygon again"
            return
        from MAVProxy.modules.lib import mp_util

        for pts in points:
            self.Geofence0["Vertices"].append(pts);
        

        if(self.Geofence0["id"] > len(self.fenceList)-1):        
            self.Send_fence(self.Geofence0);
            self.fenceList.append(self.Geofence0)
        else:
            self.fenceList[self.Geofence0["id"]] = self.Geofence0;
            self.Send_fence(self.Geofence0);

    def save_geofence(self,filename):

        top = ET.Element('Geofence')

        for fence in self.fenceList:
            print "constructing"
            child1   = ET.SubElement(top,'fence',id=str(fence['id']))
            child1_1 = ET.SubElement(child1,'type')
            child1_1.text = str(fence['type'])
            child1_2 = ET.SubElement(child1,'num_vertices')
            child1_2.text = str(fence['numV'])
            child1_3 = ET.SubElement(child1,'floor')
            child1_3.text = str(fence['floor'])
            child1_4= ET.SubElement(child1,'roof')
            child1_4.text = str(fence['roof'])            
            for vertex in range(len(fence['Vertices'])):
                child1_5 = ET.SubElement(child1,'vertex',id=str(vertex))
                child1_5_1 = ET.SubElement(child1_5,'lat')
                child1_5_1.text = str(fence['Vertices'][vertex][0])
                child1_5_2 = ET.SubElement(child1_5,'lon')
                child1_5_2.text = str(fence['Vertices'][vertex][1])


        print "writing" 
        ET.ElementTree(top).write(filename)
               

def init(mpstate):
    '''initialise module'''
    return GeoFenceModule(mpstate)
