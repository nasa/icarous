import pandas as pd

from vehiclesim import VehicleSimInterface
from communicationmodels import get_transmitter
from CustomTypes import V2Vdata
from ichelper import ConvertToLocalCoordinates

class AccordReplay(VehicleSimInterface):
    """ Vehicle sim to replay """
    def __init__(self, callsign,homepos,logfile,channel,id='vehicle',delay=0):
        """
        Initialize traffic log replay
        :param logfile: filename for csv traffic log containing columns for:
            time, vehicleID, lat, lon, alt, vN, vE, vD, [position uncertainty, velocity uncertainty]
            Properly formatted csv traffic logs can be generated from tlog, json, or daa
            log files using the read_traffic.py tool in SIRIUS
        :param channel: communication channel to replay traffic messages on
        :param delay: number of seconds to wait before starting log replay
        """
        super().__init__(callsign,homepos)
        self.delay = delay
        self.transmitter = get_transmitter("GroundTruth", callsign, channel)

        # Read position updates from csv file
        data = pd.read_csv(logfile, index_col="time",skiprows=[1])
        data = data[data.NAME.isin([id])]
        self.data = data.sort_index()
        self.pos= None
        self.vel= None
        self.sigmaP = None
        self.sigmaV = None

        self.current_time = 0.0
        self.i = 0
        self.nextTime = data.index[self.i]
        self.lastTime = data.index[-1]
        self.numRows = data.shape[0]

    def InputCommand(self, track=0, gs=0, climbrate=0):
        """ Replayed log can't react to input commands """
        pass

    def Run(self, windFrom=0, windSpeed=0):
        """ Replaying log doesn't need to simulate vehicles """
        self.current_time = self.current_time + self.dt
        while self.nextTime <= self.current_time:
            self.pos=[self.data.lat.at[self.nextTime],
                      self.data.lon.at[self.nextTime],
                      self.data.alt.at[self.nextTime] * 0.3048]
            self.vel=[self.data.vx.at[self.nextTime] * 0.5144,
                      self.data.vy.at[self.nextTime] * 0.5144,
                      self.data.vz.at[self.nextTime] * 0.00508] 
            if self.data.shape[1] > 8:
                self.sigmaP = [(self.data.s_EW_std.at[self.nextTime]*1852)**2,
                               (self.data.s_NS_std.at[self.nextTime]*1852)**2,
                               (self.data.sz_std.at[self.nextTime]*0.3048)**2,
                               (self.data.s_EN_std.at[self.nextTime]*0.3048)**2,0,0]
                self.sigmaV = [(self.data.v_EW_std.at[self.nextTime]*0.5144)**2,
                               (self.data.v_NS_std.at[self.nextTime]*0.5144)**2,
                               (self.data.vz_std.at[self.nextTime]*0.00508)**2,
                               (self.data.v_EN_std.at[self.nextTime]*0.5144)**2,0,0]
            else:
                self.sigmaP = [0 for i in range(6)]
                self.sigmaV = [0 for i in range(6)]
                
            self.i = self.i + 1
            if self.i < self.numRows:
                self.nextTime = self.data.index[self.i]
            else:
                return

        pass

    def GetOutputPositionNED(self):
        return ConvertToLocalCoordinates(self.home_gps,self.pos)

    def GetOutputVelocityNED(self):
        return [self.vel[1],self.vel[0],self.vel[2]]

    def GetCovariances(self):
        return self.sigmaP,self.sigmaV

    def TransmitPosition(self, current_time):
        """ Transmit recorded traffic positions """
        if self.transmitter is None:
            return

        if self.pos is not None:
            # Transmit all traffic messages up to current time
            sigmaP,sigmaV = self.GetCovariances()
            msg_data = {
               "callsign": self.vehicleID,
               "pos": self.pos,
               "vel": [self.vel[1],self.vel[0],self.vel[2]],
               "sigmaP": sigmaP,
               "sigmaV": sigmaV
            }
            msg = V2Vdata("INTRUDER", msg_data)
            self.transmitter.transmit(current_time, self.pos, msg)

