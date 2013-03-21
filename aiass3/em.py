import argparse
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
from matplotlib.patches import Ellipse

from cov_ellipse import *

defaultImageLocation = "images/image1_a.png"

parser = argparse.ArgumentParser()
parser.add_argument("image", nargs='?', default=defaultImageLocation, help="location of the image; default image used otherwise")
imageLocation = parser.parse_args().image

print "using image: %s" % imageLocation

# read the image
img = mpimg.imread(imageLocation)
# img[Y][X][R G B]
# print img = the entire contents of the array
# print len(img[0][0]) = 20
# img[0][19] = [0, 0, 1]  # makes the pixel blue

print img[15][15]

# get the image resolution
imgYLim = len(img)
imgXLim = len(img[0])

imgplot = plt.imshow(img)	# create plot
imgplot.set_interpolation('nearest') # we don't want interpolation so edges of pixels aren't "blurry"
plt.plot(15,15, marker='o', color='b', markersize=15, label="initial mean")
plt.plot(10,10, marker='o', color='g', markersize=10, label="final mean")
plt.legend(loc=4)
plt.ylim([imgYLim - 1,0])
plt.xlim([0,imgXLim - 1])

cov = np.array([[4., 0.], [0., 4.]])
pos = np.array([7,7])

plot_cov_ellipse(cov, pos)

pos = np.array([15,15])

plot_cov_ellipse(cov, pos)

plt.show()	# show plot