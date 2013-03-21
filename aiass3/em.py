import argparse
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import math

from cov_ellipse import *
from stat_learning import *

default_image_location = "images/image1_a.png"

parser = argparse.ArgumentParser()
parser.add_argument("image", nargs='?', default=default_image_location, help="location of the image; default image used otherwise")
image_location = parser.parse_args().image

print "using image: %s" % image_location

# read the image
img = mpimg.imread(image_location)
# img[Y][X][R G B]
# print img = the entire contents of the array
# print len(img[0][0]) = 20
# img[0][19] = [0, 0, 1]  # makes the pixel blue
# print img[15][15] # contents of that pixel

# get the image resolution
img_y_lim = len(img)
img_x_lim = len(img[0])

# set starting params of clusters
num_clusters = 4
cluster = {}	# cluster dict

# each pixel is [R G B X Y]
# mean is [R G B X Y]
# covar matrix is:
# | R.R, R.G, R.B, R.X, R.Y |
# | G.R, G.G, G.B, G.X, G.Y |
# | B.R, B.G, B.B, B.X, B.Y |
# | X.R, X.G, X.B, X.X, X.Y |
# | Y.R, Y.G, Y.B, Y.X, Y.Y |
# 

# red cluster
red_cluster = {}
mu = np.array([1., 0., 0., 14., 14.]) # init the color as pure red
sigma = np.matrix([[1.0, 0., 0., 0., 0.],
				   [0., 1.0, 0., 0., 0.],
				   [0., 0., 1.0, 0., 0.],
				   [0., 0., 0., 4.0, 0.0],
				   [0., 0., 0., 0.0, 4.0]])

red_cluster["mean"] = mu
red_cluster["covar"] = sigma
x = np.append([14., 14.], img[14][14])

print norm_pdf_multivariate(x, mu, sigma)
print log_likelyhood(x, mu, sigma)



imgplot = plt.imshow(img)	# create plot
imgplot.set_interpolation('nearest') # we don't want interpolation so edges of pixels aren't "blurry"
plt.plot(15,15, marker='o', color='b', markersize=15, label="initial mean")
plt.plot(10,10, marker='o', color='g', markersize=10, label="final mean")
plt.legend(loc=4)

# limit plot bounds
plt.ylim([img_y_lim - 1,0])
plt.xlim([0,img_x_lim - 1])

cov = np.array([[4., 0.], [0., 4.]])
pos = np.array([7,7])

plot_cov_ellipse(cov, pos)

pos = np.array([15,15])

plot_cov_ellipse(cov, pos)

plt.show()	# show plot