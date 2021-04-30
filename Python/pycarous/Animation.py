import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import patches
from matplotlib import text
from matplotlib.patches import Ellipse
import matplotlib.transforms as transforms
from math import sin,cos,atan2,pi
import numpy as np
import math
from scipy import interpolate


class AgentAnimation():
    def __init__(self,xmin,ymin,xmax,ymax,traces=True,playbkspeed=1,interval=5,record=False,filename=""):
        self.fig = plt.figure(frameon=True)
        
        self.ax = plt.axes(xlim=(xmin, xmax), ylim=(ymin, ymax))
        self.paths = {}
        self.plans = {}
        self.agents = []
        self.agentsRadius = {}
        self.agentNames = []
        self.agentLines = []
        self.data = {}
        self.interval = interval
        self.circle = {}
        self.bands = {}
        self.record = record
        self.status  = {}
        self.filename = filename
        self.speed = playbkspeed
        self.minlen = 0
        self.tmin = 0
        self.tmax = 0
        self.dt = -1
        self.agentIndex = {}
        self.traces = traces
        self.ellipses = {}

    def AddAgent(self,name,radius,color,data,show_circle=False,circle_rad = 10,show_ellipse=False,sensor_view=False):
        #agt = plt.Circle((0.0, 0.0), radius=radius, fc=color)
        if len(data['time']) < 2:
            return

        if name not in self.agentNames:
            self.agentNames.append(name)
            self.data[name] = data
            agt = self.GetTriangle(radius,(0.0,0,0),(1.0,0.0),color)
            agt.set_alpha(0.5)
            self.ax.add_patch(agt)
            self.agents.append(agt)
            self.agentIndex[name] = 0
        else:
            t0 = self.data[name]['time'][0]
            t1 = self.data[name]['time'][-1]
            index = 0
            if data['time'][0] < t0:
                for key in data.keys():
                    self.data[name][key]= data[key] + self.data[name][key]
            else:
                while data['time'][index] < t1:
                    index = index + 1
                    if index >= len(data['time']):
                        return
                for key in data.keys():
                    self.data[name][key].extend(data[key][index:])
        self.tmin = min(self.tmin,data['time'][0])
        self.tmax = max(self.tmax,data['time'][-1])
        if self.dt < 0:
            self.dt = data['time'][1] - data['time'][0]
        self.minlen = max(self.minlen,len(data['positionNED']))
        positionNED = np.array(self.data[name]['positionNED'])
        velocityNED = np.array(self.data[name]['velocityNED'])
        time = self.data[name]['time']
        if sensor_view:
            interptype = 'previous'
        else:
            interptype = 'linear'
        self.data[name]['positionNED_intp'] = [interpolate.interp1d(time,positionNED[:,0],kind=interptype,bounds_error=False,fill_value=(positionNED[0][0],positionNED[-1][0])),
                                               interpolate.interp1d(time,positionNED[:,1],kind=interptype,bounds_error=False,fill_value=(positionNED[0][1],positionNED[-1][1])),
                                               interpolate.interp1d(time,positionNED[:,2],kind=interptype,bounds_error=False,fill_value=(positionNED[0][2],positionNED[-1][2]))]
        self.data[name]['velocityNED_intp'] = [interpolate.interp1d(time,velocityNED[:,0],kind=interptype,bounds_error=False,fill_value=(0,0)),
                                               interpolate.interp1d(time,velocityNED[:,1],kind=interptype,bounds_error=False,fill_value=(0,0)),
                                               interpolate.interp1d(time,velocityNED[:,2],kind=interptype,bounds_error=False,fill_value=(0,0))]
        
        self.data[name]['reverseTime_intp'] = interpolate.interp1d(time,np.arange(len(time)),kind=interptype,bounds_error=False,fill_value=(0,len(time)-1))
        self.data[name]['plotX'] = []
        self.data[name]['plotY'] = []
        if self.traces:
            line, = plt.plot(0,0)
            self.paths[name] = line
        self.agentsRadius[name] = radius
        if show_circle:
            circlePatch = plt.Circle((0, 0), radius=circle_rad, fc='y',alpha=0.5)
            self.circle[name] = circlePatch
            self.ax.add_patch(circlePatch)
        # Draw bands
        sectors = []
        for i in range(15):
            ep = patches.Wedge((0,0),circle_rad,theta1=0,theta2=0,fill=True,alpha=0.6)
            sectors.append(ep)
            self.ax.add_patch(ep)
        self.bands[name] = sectors

        if show_ellipse:
            cov = np.array([[0.1, 0.0],[0.0, 0.1]])
            ellipse = confidence_ellipse(0,0,cov, self.ax, None,edgecolor=color)
            self.ax.add_patch(ellipse)
            self.ellipses[name] = ellipse
            sigma       = np.array(self.data[name]['sigma'])
            self.data[name]['sigma_intp']   = [interpolate.interp1d(time,sigma[:,0],kind=interptype,bounds_error=False,fill_value=(0,0)),
                                               interpolate.interp1d(time,sigma[:,1],kind=interptype,bounds_error=False,fill_value=(0,0)),
                                               interpolate.interp1d(time,sigma[:,2],kind=interptype,bounds_error=False,fill_value=(0,0))]

    def AddZone(self,xy,radius,color):
        circlePatch = patches.Arc((xy[0], xy[1]), width=2*radius,height =2*radius, fill =False, color=color)
        self.ax.add_patch(circlePatch)

    def GetTriangle(self, tfsize, pos, vel, col):
        x = pos[0]
        y = pos[1]

        t = atan2(vel[1],vel[0])

        x1 = x + 2*tfsize * cos(t)
        y1 = y + 2*tfsize * sin(t)

        tempX = x - tfsize * cos(t)
        tempY = y - tfsize * sin(t)

        x2 = tempX + tfsize * cos((t + pi/2))
        y2 = tempY + tfsize * sin((t + pi/2))

        x3 = tempX - tfsize * cos((t + pi/2))
        y3 = tempY - tfsize * sin((t + pi/2))


        triangle = plt.Polygon([[x1, y1], [x2, y2], [x3, y3]], color=col, fill=True)
        #triangle.labelText = plt.text(x+2*tfsize, y+2*tfsize, "", fontsize=8)

        return triangle

    def UpdateTriangle(self,tfsize, pos, vel, poly, id=''):
        x = pos[0]
        y = pos[1]
        z = pos[2]

        t = atan2(vel[1], vel[0])

        x1 = x + 2*tfsize * cos(t)
        y1 = y + 2*tfsize * sin(t)

        tempX = x - 1*tfsize * cos(t)
        tempY = y - 1*tfsize * sin(t)

        x2 = tempX + 1*tfsize * cos((t + pi/2))
        y2 = tempY + 1*tfsize * sin((t + pi/2))

        x3 = tempX - 1*tfsize * cos((t + pi/2))
        y3 = tempY - 1*tfsize * sin((t + pi/2))

        poly.set_xy([[x1, y1], [x2, y2], [x3, y3]])
        poly.labelText.set_position((x + 2*tfsize,y+2*tfsize))
        speed = np.sqrt(vel[0]**2 + vel[1]**2)
        #poly.labelText.set_text('%s Z:%.2f[m]\nS:%.2f[mps]' % (id,z,speed))

    def AddPath(self,path,color1,points = [],labels = [],color2='',time=0):
        def PlotPath():
            if (path.shape[0] < 2):
                return
            if len(points) > 0:
                plt.plot(points[:,1],points[:,0],color1)
            else:
                plt.plot(path[:,1],path[:,0],color1)
            if color2 == '':
                plt.scatter(path[:,1],path[:,0])
            else:
                plt.scatter(path[:,1],path[:,0],c=color2)
            
            #for i,label in enumerate(labels):
            #    plt.text(path[i,1],path[i,0],','.join(label))
        id = len(self.plans.keys())
        self.plans[id] = (time,PlotPath)

    def AddFence(self,fence,color):
        plt.plot(fence[:,1],fence[:,0],color)
        plt.scatter(fence[:,1],fence[:,0])

    def UpdateBands(self,position,bands,timeLookUp,t,sectors):
        i = int(timeLookUp(t))
        numBands = bands["numBands"][i]
        if(numBands) == 0:
            return
        low      = bands["low"][i]
        high     = bands["high"][i]
        btype    = bands["bandTypes"][i]
        h2c = lambda x:np.mod((360 -  (x - 90)),360)
        for sector in sectors:
            sector.set_theta1(0)
            sector.set_theta2(0)
            sector.set_center((0,0))
            sector.set_alpha(0)
        for i in range(numBands):
            max = h2c(low[i])
            min = h2c(high[i])
            if btype[i] != 1:
                if btype[i] == 4:
                    color = 'r'
                elif btype[i] == 5:
                    color = 'g'
                else:
                    color = 'b'
                sectors[i].set_center(position[:2])
                sectors[i].set_theta1(min)
                sectors[i].set_theta2(max)
                sectors[i].set_color(color)
                sectors[i].set_alpha(0.6)


    def init(self):
        return self.agents,self.paths,self.circle

    def animate(self,time,i):
<<<<<<< HEAD
        i = int(i*self.speed)
        maxLen = len(time)
        print("generating animation: %.1f%%" % (i/maxLen*100), end="\r")

        for key in self.plans:
            tFuncPair = self.plans[key]
            if tFuncPair is not None:
                t = tFuncPair[0]
                plotFunc = tFuncPair[1]
                if time[i] >= t:
                    plotFunc()
                    self.plans[key] = None

        if i < maxLen:
=======
        i = int(np.floor(i*self.speed))
        print("generating animation: %.1f%%" % (i/self.minlen*100), end="\r")
        if i < len(time):
>>>>>>> [pycarous/Modules] Debugging updates, refactoring
            for j, vehicle in enumerate(self.agents):
                k = time[i]
                id = self.agentNames[j]
                #vehicle.center = (self.data[id][i][0], self.data[id][i][1])
                
                position = (self.data[id]["positionNED_intp"][1](k), self.data[id]["positionNED_intp"][0](k),self.data[id]["positionNED_intp"][2](k))
                velocity = (self.data[id]["velocityNED_intp"][1](k), self.data[id]["velocityNED_intp"][0](k),self.data[id]["velocityNED_intp"][2](k))
                if np.linalg.norm(velocity) < 1e-3:
                    continue
                self.data[id]['plotX'].append(float(position[0]))
                self.data[id]['plotY'].append(float(position[1]))
                
                radius = self.agentsRadius[id]
                self.UpdateTriangle(radius,position,velocity,vehicle,id)
                if self.traces:
                    self.paths[id].set_xdata(self.data[id]['plotX'])
                    self.paths[id].set_ydata(self.data[id]['plotY'])
                if "trkbands" in self.data[id].keys():
                    self.UpdateBands(position,self.data[id]["trkbands"],self.data[id]["reverseTime_intp"],k,self.bands[id])
                if id in self.circle.keys():
                    if self.circle[id] is not None:
                        self.circle[id].center = position
                if id in self.ellipses.keys():
                    sigma_xx = self.data[id]["sigma_intp"][0](k)
                    sigma_yy = self.data[id]["sigma_intp"][1](k)
                    sigma_xy = self.data[id]["sigma_intp"][2](k)
                    cov = np.array([[sigma_xx, sigma_xy],[sigma_xy, sigma_yy]])
                    confidence_ellipse(position[0],position[1],cov, self.ax, self.ellipses[id], edgecolor='red')
        else:
            plt.close(self.fig)
        return self.agents,self.paths,self.circle

    def run(self):
        time = np.arange(self.tmin,self.tmax+self.dt,self.dt)
        animate = lambda x: self.animate(time,x)
        init = lambda:self.init()

        self.anim = animation.FuncAnimation(self.fig, animate,
                                       init_func=init,
<<<<<<< HEAD
                                       frames=len(time),
=======
                                       frames=np.max([self.minlen,int(self.minlen/self.speed)]),
>>>>>>> [pycarous/Modules] Debugging updates, refactoring
                                       interval=self.interval,
                                       repeat = False,
                                       blit=False)
        
        # Save animation as a movie
        if self.record:
            self.anim.save(self.filename, writer= "ffmpeg", fps=60)
        else:
            plt.xlabel("x [m]")
            plt.ylabel("y [m]")
            #plt.axis('off')
            plt.show()


def confidence_ellipse(x,y,cov, ax, ellipse, n_std=3.0, facecolor='none', **kwargs):
    """
    src: https://matplotlib.org/devdocs/gallery/statistics/confidence_ellipse.html
    Create a plot of the covariance confidence ellipse

    Parameters
    ----------
    covariance : covariance matrix

    ax : matplotlib.axes.Axes
        The axes object to draw the ellipse into.

    ellipse: prev ellipse

    n_std : float
        The number of standard deviations to determine the ellipse's radiuses.

    **kwargs
        Forwarded to `~matplotlib.patches.Ellipse`

    Returns
    -------
    matplotlib.patches.Ellipse
    """

    pearson = cov[0, 1]/np.sqrt(cov[0, 0] * cov[1, 1])
    # Using a special case to obtain the eigenvalues of this
    # two-dimensionl dataset.
    ell_radius_x = np.sqrt(1 + pearson)
    ell_radius_y = np.sqrt(1 - pearson)
    if ellipse is None:
        ellipse = Ellipse((0, 0), width=ell_radius_x * 2, height=ell_radius_y * 2,
                          facecolor=facecolor, **kwargs)

    # Calculating the stdandard deviation of x from
    # the squareroot of the variance and multiplying
    # with the given number of standard deviations.
    scale_x = np.sqrt(cov[0, 0]) * n_std
    mean_x = x

    # calculating the stdandard deviation of y ...
    scale_y = np.sqrt(cov[1, 1]) * n_std
    mean_y = y

    transf = transforms.Affine2D() \
        .rotate_deg(45) \
        .scale(scale_x, scale_y) \
        .translate(mean_x, mean_y)

    ellipse.set_transform(transf + ax.transData)

    return ellipse