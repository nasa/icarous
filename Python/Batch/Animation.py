import matplotlib.pyplot as plt
import matplotlib.animation as animation
from math import sin,cos,atan2,pi
import numpy as np

class AgentAnimation():
    def __init__(self,xmin,ymin,xmax,ymax,interval):
        self.fig = plt.figure()
        plt.xlabel("x [m]")
        plt.ylabel("y [m]")

        self.ax = plt.axes(xlim=(xmin, xmax), ylim=(ymin, ymax))
        self.paths = {}
        self.agents = []
        self.agentsRadius = {}
        self.agentNames = []
        self.agentLines = []
        self.data = {}
        self.interval = interval
        self.circle = {}

    def AddAgent(self,name,radius,color,data,show_circle=False,circle_rad = 10):
        #agt = plt.Circle((0.0, 0.0), radius=radius, fc=color)
        agt = self.GetTriangle(radius,(0.0,0,0),(1.0,0.0),color)
        self.ax.add_patch(agt)
        self.agents.append(agt)
        self.agentNames.append(name)
        self.data[name] = data
        self.minlen = len(data['positionNED'])
        line, = plt.plot(0,0)
        self.paths[name] = line
        self.agentsRadius[name] = radius
        if show_circle:
            circlePatch = plt.Circle((0, 0), radius=circle_rad, fc='y',alpha=0.5)
            self.circle[name] = circlePatch
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

        return plt.Polygon([[x1, y1], [x2, y2], [x3, y3]], color=col, fill=True)

    def UpdateTriangle(self,tfsize, pos, vel, poly):
        x = pos[0]
        y = pos[1]

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

    def AddPath(self,path,color):
        plt.plot(path[:,1],path[:,0],color)
        plt.scatter(path[:,1],path[:,0])

    def init(self):
        return self.agents,self.paths,self.circle

    def animate(self,i):
        if i < self.minlen:
            for j, vehicle in enumerate(self.agents):
                id = self.agentNames[j]
                #vehicle.center = (self.data[id][i][0], self.data[id][i][1])
                position = (self.data[id]["positionNED"][i][1], self.data[id]["positionNED"][i][0])
                velocity = (self.data[id]["velocityNED"][i][1], self.data[id]["velocityNED"][i][0])
                radius = self.agentsRadius[id]
                self.UpdateTriangle(radius,position,velocity,vehicle)
                self.paths[id].set_xdata(np.array(self.data[id]["positionNED"])[:i,1])
                self.paths[id].set_ydata(np.array(self.data[id]["positionNED"])[:i,0])
                if id in self.circle.keys():
                    if self.circle[id] is not None:
                        self.circle[id].center = position
        return self.agents,self.paths,self.circle

    def run(self):
        animate = lambda x: self.animate(x)
        init = lambda:self.init()
        self.anim = animation.FuncAnimation(self.fig, animate,
                                       init_func=init,
                                       frames=self.minlen,
                                       interval=self.interval,
                                       blit=False)
        
        # Save animation as a movie
        #self.anim.save('animation.mp4', fps=10, 
        #          extra_args=['-vcodec', 'h264', 
        #                      '-pix_fmt', 'yuv420p'])

        plt.show()
