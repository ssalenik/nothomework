import argparse
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import math
from copy import deepcopy

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
num_pixels = img_y_lim * img_x_lim

# set starting params of clusters
num_clusters = 4
cluster_init = {}	# init cluster dict

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
red_cluster["weight"] = 1.0/num_clusters
cluster_init["red"] = red_cluster # add the red_cluster dict to our dict of clusters

# blue cluster
blue_cluster = {}
mu = np.array([0., 0., 1., 3., 14.]) # init the color as pure blue
sigma = np.matrix([[1.0, 0., 0., 0., 0.],
				   [0., 1.0, 0., 0., 0.],
				   [0., 0., 1.0, 0., 0.],
				   [0., 0., 0., 4.0, 0.0],
				   [0., 0., 0., 0.0, 4.0]])

blue_cluster["mean"] = mu
blue_cluster["covar"] = sigma
blue_cluster["weight"] = 1.0/num_clusters
cluster_init["blue"] = blue_cluster # add the blue_cluster dict to our dict of 

# black cluster
black_cluster = {}
mu = np.array([0., 0., 0., 8., 7.]) # init the color as pure black
sigma = np.matrix([[1.0, 0., 0., 0., 0.],
				   [0., 1.0, 0., 0., 0.],
				   [0., 0., 1.0, 0., 0.],
				   [0., 0., 0., 4.0, 0.0],
				   [0., 0., 0., 0.0, 4.0]])

black_cluster["mean"] = mu
black_cluster["covar"] = sigma
black_cluster["weight"] = 1.0/num_clusters
cluster_init["black"] = black_cluster # add the black_cluster dict to our dict of clusters

# white cluster
white_cluster = {}
mu = np.array([1., 1., 1., 14., 2.]) # init the color as pure white
sigma = np.matrix([[1.0, 0., 0., 0., 0.],
				   [0., 1.0, 0., 0., 0.],
				   [0., 0., 1.0, 0., 0.],
				   [0., 0., 0., 4.0, 0.0],
				   [0., 0., 0., 0.0, 4.0]])

white_cluster["mean"] = mu
white_cluster["covar"] = sigma
white_cluster["weight"] = 1.0/num_clusters
cluster_init["white"] = white_cluster # add the white_cluster dict to our dict of clusters

log_likelyhood_curr = 0	# init log likelyhood
cluster_curr = deepcopy(cluster_init)	# init current cluster vals to the initial ones
p = [[{} for x in range(img_x_lim)] for y in range(img_y_lim)] # init p matrix which is a matrix of pij vals corresponding to each pixel in the image

for step in range(2):
	log_likelyhood_prev = deepcopy(log_likelyhood_curr) # save the previous result for comparison later
	# E-step
	
	# init n to 0 every itteration
	for cluster in cluster_curr:
		cluster_curr[cluster]["n"] = 0

	# for every pixel
	for y in range(img_y_lim):
		for x in range(img_x_lim):
			# calc pij
			# get the the P(x | C = i) for each cluster
			prob_x_given_c = {}
			for cluster in cluster_curr:
				# get the sample RGB and append the X, Y
				sample = np.append(img[y][x], np.array([float(x), float(y)]))
				# get the probability by calculating the pdf
				prob_x_given_c[cluster] = norm_pdf_multivariate(sample, cluster_curr[cluster]["mean"], cluster_curr[cluster]["covar"])
				# while we're here, calculate the log likelyhood
				log_likelyhood_curr += log_likelyhood(sample, cluster_curr[cluster]["mean"], cluster_curr[cluster]["covar"])

			# get the denominator in the Bayes' rule calc
			denominator = 0
			for cluster in cluster_curr:
				denominator += prob_x_given_c[cluster] * cluster_curr[cluster]["weight"]

			# now calc the pij for each cluster and store it in the p matrix
			for cluster in cluster_curr:
				pij = prob_x_given_c[cluster]*cluster_curr[cluster]["weight"]/denominator
				cluster_curr[cluster]["n"] += pij
				#print pij
				p[y][x][cluster] = pij

			# ni += pij
			# total log likelyhood += loglikelyhood of current pixel

	# print the n for each cluster
	total = 0
	for cluster in cluster_curr:
		total += cluster_curr[cluster]["n"]
		print "%s : %f" % (cluster, cluster_curr[cluster]["n"])
	print "total %f" % total

	# M-step
	# for every cluster
		# calc mu
		# calc sigma
		# calc weight

x = np.append([14., 14.], img[14][14])

print norm_pdf_multivariate(x, mu, sigma)
print log_likelyhood(x, mu, sigma)



imgplot = plt.imshow(img)	# create plot
imgplot.set_interpolation('nearest') # we don't want interpolation so edges of pixels aren't "blurry"
plt.plot(15,15, marker='o', color='b', markersize=15, label="initial mean")
plt.plot(10,10, marker='o', color='g', markersize=10, label="final mean")
#plt.legend(loc=4)

# limit plot bounds
plt.ylim([img_y_lim - 1,0])
plt.xlim([0,img_x_lim - 1])

cov = np.matrix([[4., 0.], [0., 4.]])
pos = np.array([7,7])

plot_cov_ellipse(cov, pos)

pos = np.array([15,15])

plot_cov_ellipse(cov, pos)

plt.show()	# show plot