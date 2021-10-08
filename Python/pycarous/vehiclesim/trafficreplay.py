from icutils.AccordUtil import ConvertNED2LLA
from icutils.units import From, From_string
import pandas as pd

from vehiclesim import VehicleSimInterface
from communicationmodels import get_transmitter
from CustomTypes import V2Vdata


class TrafficReplay(VehicleSimInterface):
    """ Vehicle sim to replay """
    def __init__(self, logfile, channel, delay=0, homePos = None,filter=[]):
        """
        Initialize traffic log replay
        :param logfile: filename for csv traffic log containing columns for:
            time, NAME, lat, lon, alt, vn, ve, vd, [position uncertainty, velocity uncertainty]
            Properly formatted csv traffic logs can be generated from tlog, json, or daa
            log files using the read_traffic.py tool in SIRIUS
        :param channel: communication channel to replay traffic messages on
        :param delay: number of seconds to wait before starting log replay
        :param filter: list of callsigns to skip
        """
        self.delay = delay
        self.transmitter = get_transmitter("GroundTruth", "TrafficReplay", channel)
        
        # Read position updates from csv file
        data =  pd.read_csv(logfile, index_col="time")
        units = data.iloc[0]
        self.position_updates = data.iloc[1:].astype(dict([(key,'double') for key in data.columns if key != 'NAME']))
        self.position_updates.index = self.position_updates.index.astype('double') + self.delay
        self.position_updates.sort_index(inplace=True)
        self.homePos = homePos
        self.VNED = False
        self.uncertainty = False
        
        self.units = {}
        # Position units
        if 'lat' not in units.index:
            self.units['sx'] = From_string(units.sx,1)
            self.units['sy'] = From_string(units.sy,1)
            self.units['sz'] = From_string(units.sz,1)
        else:
            self.units['lat'] = 1
            self.units['lon'] = 1
            self.units['alt'] = From_string(units.alt,1)

        # velocity units
        if 'vn' in units.index:
            self.units['vn'] =  From_string(units.vn,1)
            self.units['ve'] =  From_string(units.ve,1)
            self.units['vd'] =  From_string(units.vd,1)
            self.VNED = True
        else:
            self.units['vy'] =  From_string(units.vx,1)
            self.units['vx'] =  From_string(units.vy,1)
            self.units['vz'] =  From_string(units.vz,1)

        if 's_EW_std' in units.s_EW_std:
            self.uncertainty = True
            self.units['s_EW_std'] = From_string(units.s_EW_std,1)
            self.units['s_NS_std'] = From_string(units.s_NS_std,1)
            self.units['s_EN_std'] = From_string(units.s_EN_std,1)
            self.units['s_z_std'] = From_string(units.s_z_std,1)
            self.units['v_EW_std'] = From_string(units.v_EW_std,1)
            self.units['v_NS_std'] = From_string(units.v_NS_std,1)
            self.units['v_EN_std'] = From_string(units.v_EN_std,1)
            self.units['v_z_std'] = From_string(units.v_z_std,1)

        self.filter = filter

        self.lastBcastTime = 0

    def InputCommand(self, track=0, gs=0, climbrate=0):
        """ Replayed log can't react to input commands """
        pass

    def Run(self, windFrom=0, windTo=0):
        """ Replaying log doesn't need to simulate vehicles """
        return True

    def GetOutputPositionNED(self):
        pass

    def GetOutputVelocityNED(self):
        pass

    def TransmitPosition(self, current_time):
        """ Transmit recorded traffic positions """
        if self.transmitter is None:
            return

        # Transmit all traffic messages up to current time
        traffic_messages = self.position_updates[self.lastBcastTime:current_time]
        for i, traffic_msg in traffic_messages.iterrows():
            if traffic_msg.NAME in self.filter:
                continue 
            if self.homePos is not None:
                posLLA = ConvertNED2LLA(self.homePos,[traffic_msg.sy * self.units['sy'],traffic_msg.sx*self.units['sx'],-traffic_msg.sz*self.units['sz']])
            else:
                posLLA = [traffic_msg.lat*self.units['lat'], traffic_msg.lon*self.units['lon'], traffic_msg.alt*self.units['alt']]

            if self.VNED:
                velNED = [traffic_msg.vn*self.units['vn'], traffic_msg.ve*self.units['ve'], traffic_msg.vd*self.units['vd']]
            else:
                velNED = [traffic_msg.vy*self.units['vy'], traffic_msg.vx*self.units['vx'], -traffic_msg.vz*self.units['vd']]
            
            sigmaP = [0.0,0.0,0.0,0.0,0.0,0.0]
            sigmaV = [0.0,0.0,0.0,0.0,0.0,0.0]
            if self.uncertainty:
                sigmaP[0] = (traffic_msg.s_EW_std*self.units['s_EW_std'])**2
                sigmaP[1] = (traffic_msg.s_NS_std*self.units['s_NS_std'])**2
                sigmaP[2] = (traffic_msg.s_z_std*self.units['s_z_std'])**2,
                sigmaP[3] = (traffic_msg.s_EN_std*self.units['s_EN_std'])**2
                sigmaV[0] = (traffic_msg.v_EW_std*self.units['v_EW_std'])**2
                sigmaV[1] = (traffic_msg.v_NS_std*self.units['v_NS_std'])**2
                sigmaV[2] = (traffic_msg.v_z_std*self.units['v_z_std'])**2,
                sigmaV[3] = (traffic_msg.v_EN_std*self.units['v_EN_std'])**2

            msg_data = {
                "source": self.transmitter.sensorType,
                "callsign": "replay_" + traffic_msg.NAME,
                "pos": posLLA,
                "vel": velNED,
                "sigmaP": sigmaP,
                "sigmaV": sigmaV,
            }
            msg = V2Vdata("INTRUDER", msg_data)
            self.transmitter.transmit(current_time, posLLA, msg)

            self.lastBcastTime = current_time
