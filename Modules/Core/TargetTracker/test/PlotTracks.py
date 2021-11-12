import pandas as pd
import sys
from matplotlib import pyplot as plt
from matplotlib.patches import Ellipse
import matplotlib.transforms as transforms
import numpy as np

fname = sys.argv[1]

data = pd.read_csv(fname)
data = data.set_index('time')
data = data.sort_index()

callsigns = data['callsign'].unique()

uniqueTracks = len(callsigns)

def PlotEllipse(ax,x,y,n_std,cov,color):
    pearson = cov[0, 1]/np.sqrt(cov[0, 0] * cov[1, 1])
    # Using a special case to obtain the eigenvalues of this
    # two-dimensionl dataset.
    ell_radius_x = np.sqrt(1 + pearson)
    ell_radius_y = np.sqrt(1 - pearson)
    ellipse = Ellipse((0, 0), width=ell_radius_x * 2, height=ell_radius_y * 2, edgecolor=color, fill=False)

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
    ax.add_patch(ellipse)


fig = plt.figure()
ax  = fig.gca()
for callsign in callsigns:
    data_cs = data[data['callsign'] == callsign]
    plt.plot(data_cs['x'].to_numpy(),data_cs['y'].to_numpy())
    count = 0
    for rowdata in data_cs.iterrows():
        rowdata = rowdata[1]
        cov = np.array([[rowdata['sxx'],rowdata['sxy']],[rowdata['sxy'],rowdata['syy']]])
        if count%10 == 0:
            PlotEllipse(ax,rowdata['x'],rowdata['y'],3,cov,'r')
        count += 1

plt.show()
    


