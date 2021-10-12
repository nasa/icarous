from icutils.AccordUtil import ConvertNED2LLA
from icutils.units import From, From_string
from icutils.ichelper import ConvertToLocalCoordinates
import pandas as pd
import numpy as np

from vehiclesim import VehicleSimInterface
from communicationmodels import get_transmitter
from CustomTypes import V2Vdata


class TrafficReplay(VehicleSimInterface):
    """ Vehicle sim to replay """
    def __init__(self, logfile, channel, delay=0, homePos = None,filter=[],sigmaP=[1.0,1.0,1.0,1.0,1.0,1.0],sigmaV=[1.0,1.0,1.0,1.0,1.0,1.0],dt=0.05):
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
        self.dt = dt
        self.transmitter = get_transmitter("GroundTruth", "TrafficReplay", channel)
        
        # Read position updates from csv file
        data =  pd.read_csv(logfile, index_col="time")
        units = data.iloc[0]
        self.data = data.iloc[1:].astype(dict([(key,'double') for key in data.columns if key != 'NAME']))
        self.data.index = self.data.index.astype('double') + self.delay
        self.data.sort_index(inplace=True)
        self.ownshipUpdates = None
        self.home_gps = homePos
        self.firstPos = None
        self.VNED = False
        self.uncertainty = False
        self.units = {}
        self.minSigmaP = sigmaP 
        self.minSigmaV = sigmaV 
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

        if 's_EW_std' in units.index:
            self.uncertainty = True
            self.units['s_EW_std'] = From_string(units.s_EW_std,1)
            self.units['s_NS_std'] = From_string(units.s_NS_std,1)
            self.units['s_EN_std'] = From_string(units.s_EN_std,1)
            self.units['sz_std'] = From_string(units.sz_std,1)
            self.units['v_EW_std'] = From_string(units.v_EW_std,1)
            self.units['v_NS_std'] = From_string(units.v_NS_std,1)
            self.units['v_EN_std'] = From_string(units.v_EN_std,1)
            self.units['vz_std'] = From_string(units.vz_std,1)

        self.filter = filter

        self.lastBcastTime = 0
        self.current_time = 0.0
        self.i = 0
        self.nextTime = self.data.index[self.i]
        self.numRows = self.data.shape[0]

    def InputCommand(self, track=0, gs=0, climbrate=0):
        """ Replayed log can't react to input commands """
        pass

    def Run(self, windFrom=0, windSpeed=0):
        """ Replaying log doesn't need to simulate vehicles """

        if self.ownshipUpdates is None:
            self.ownshipUpdates = self.data[self.data.NAME.isin(['Ownship'])]

        self.current_time = self.current_time + self.dt
        if self.nextTime > self.current_time:
            return False
        
        while self.nextTime <= self.current_time:
            if self.home_gps is not None:
                self.pos=[self.ownshipUpdates.sy.at[self.nextTime]* self.units['sy'],
                          self.ownshipUpdates.sx.at[self.nextTime]* self.units['sx'],
                          self.ownshipUpdates.sz.at[self.nextTime]* self.units['sz']]
            else:
                self.pos=[self.ownshipUpdates.lat.at[self.nextTime],
                          self.ownshipUpdates.lon.at[self.nextTime],
                          self.ownshipUpdates.alt.at[self.nextTime] * self.units['alt']]
                if self.firstPos is None:
                    self.firstPos = self.pos

            if self.VNED:
                self.vel=[self.ownshipUpdates.vn.at[self.nextTime] * self.units['vn'],
                          self.ownshipUpdates.ve.at[self.nextTime] * self.units['ve'],
                          self.ownshipUpdates.vd.at[self.nextTime] * self.units['vd']] 
            else:
                self.vel=[self.ownshipUpdates.vy.at[self.nextTime] * self.units['vy'],
                          self.ownshipUpdates.vx.at[self.nextTime] * self.units['vx'],
                          self.ownshipUpdates.vz.at[self.nextTime] * self.units['vz']] 

            if self.uncertainty:
                sxx = (self.ownshipUpdates.s_EW_std.at[self.nextTime]*self.units['s_EW_std'])**2
                syy = (self.ownshipUpdates.s_NS_std.at[self.nextTime]*self.units['s_NS_std'])**2
                szz = (self.ownshipUpdates.sz_std.at[self.nextTime]*self.units['sz_std'])**2
                sxy = (self.ownshipUpdates.s_EN_std.at[self.nextTime]*self.units['s_EN_std'])**2
                vxx = (self.ownshipUpdates.v_EW_std.at[self.nextTime]*self.units['v_EW_std'])**2
                vyy = (self.ownshipUpdates.v_NS_std.at[self.nextTime]*self.units['v_NS_std'])**2
                vzz = (self.ownshipUpdates.vz_std.at[self.nextTime]*self.units['vz_std'])**2
                vxy = (self.ownshipUpdates.v_EN_std.at[self.nextTime]*self.units['v_EN_std'])**2

                self.sigmaP = [np.max([sxx,self.minSigmaP[0]]),
                               np.max([syy,self.minSigmaP[1]]),
                               np.max([szz,self.minSigmaP[2]]),
                               np.min([sxy,np.sqrt(sxx*syy * 0.95)]),0,0]
                self.sigmaV = [np.max([vxx,self.minSigmaV[0]]),
                               np.max([vyy,self.minSigmaV[1]]),
                               np.max([vzz,self.minSigmaV[2]]),
                               np.min([vxy,np.sqrt(vxx*vyy * 0.1)]),0,0]

            else:
                self.sigmaP = [0 for i in range(6)]
                self.sigmaV = [0 for i in range(6)]
                
            self.i = self.i + 1
            if self.i < self.numRows:
                self.nextTime = self.ownshipUpdates.index[self.i]
        return True

    def GetOutputPositionNED(self):
        locPos = ConvertToLocalCoordinates(self.firstPos,self.pos)
        locPos[2] = -locPos[2]
        return locPos

    def GetOutputVelocityNED(self):
        return self.vel

    def GetCovariances(self):
        return self.sigmaP,self.sigmaV

    def TransmitPosition(self, current_time):
        """ Transmit recorded traffic positions """
        if self.transmitter is None:
            return

        # Transmit all traffic messages up to current time
        traffic_messages = self.data[self.lastBcastTime:current_time]
        for i, traffic_msg in traffic_messages.iterrows():
            if traffic_msg.NAME in self.filter:
                continue 
            if self.home_gps is not None:
                posLLA = ConvertNED2LLA(self.home_gps,[traffic_msg.sy * self.units['sy'],traffic_msg.sx*self.units['sx'],-traffic_msg.sz*self.units['sz']])
            else:
                posLLA = [traffic_msg.lat*self.units['lat'], traffic_msg.lon*self.units['lon'], traffic_msg.alt*self.units['alt']]

            if self.VNED:
                velNED = [traffic_msg.vn*self.units['vn'], traffic_msg.ve*self.units['ve'], traffic_msg.vd*self.units['vd']]
            else:
                velNED = [traffic_msg.vy*self.units['vy'], traffic_msg.vx*self.units['vx'], -traffic_msg.vz*self.units['vz']]
            
            sigmaP = [0.0,0.0,0.0,0.0,0.0,0.0]
            sigmaV = [0.0,0.0,0.0,0.0,0.0,0.0]
            if self.uncertainty:
                sxx = (traffic_msg.s_EW_std*self.units['s_EW_std'])**2
                syy = (traffic_msg.s_NS_std*self.units['s_NS_std'])**2
                szz = (traffic_msg.sz_std*self.units['sz_std'])**2
                sxy = (traffic_msg.s_EN_std*self.units['s_EN_std'])**2
                vxx = (traffic_msg.v_EW_std*self.units['v_EW_std'])**2
                vyy = (traffic_msg.v_NS_std*self.units['v_NS_std'])**2
                vzz = (traffic_msg.vz_std*self.units['vz_std'])**2
                vxy = (traffic_msg.v_EN_std*self.units['v_EN_std'])**2

                sigmaP = [np.max([sxx,self.minSigmaP[0]]),
                          np.max([syy,self.minSigmaP[1]]),
                          np.max([szz,self.minSigmaP[2]]),
                          np.min([sxy,np.sqrt(sxx*syy * 0.95)]),0,0]
                sigmaV = [np.max([vxx,self.minSigmaV[0]]),
                          np.max([vyy,self.minSigmaV[1]]),
                          np.max([vzz,self.minSigmaV[2]]),
                          np.min([vxy,np.sqrt(vxx*vyy * 0.1)]),0,0]

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
