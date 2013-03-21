import argparse
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import math
from copy import deepcopy

from cov_ellipse import *
from stat_learning import *
from util import *

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
mu = np.array([1., 0., 0., translate_20_to_1(14.), translate_20_to_1(14.)]) # init the color as pure red
# sigma = np.matrix([[1.0, 0.9, 0.9, 0.9, 0.9],
# 				   [0.9, 1.0, 0.9, 0.9, 0.9],
# 				   [0.9, 0.9, 1.0, 0.9, 0.9],
# 				   [0.9, 0.9, 0.9, 4.0, 0.90],
# 				   [0.9, 0.9, 0.9, 0.9, 4.0]])

red_cluster["mean"] = mu
#red_cluster["covar"] = sigma
red_cluster["weight"] = 1.0/num_clusters
cluster_init["red"] = red_cluster # add the red_cluster dict to our dict of clusters

# blue cluster
blue_cluster = {}
mu = np.array([0., 0., 1., translate_20_to_1(2.), translate_20_to_1(15.)]) # init the color as pure blue
# sigma = np.matrix([[1.0, 0., 0., 0., 0.],
# 				   [0., 1.0, 0., 0., 0.],
# 				   [0., 0., 1.0, 0., 0.],
# 				   [0., 0., 0., 4.0, 0.0],
# 				   [0., 0., 0., 0.0, 4.0]])

blue_cluster["mean"] = mu
# blue_cluster["covar"] = sigma
blue_cluster["weight"] = 1.0/num_clusters
cluster_init["blue"] = blue_cluster # add the blue_cluster dict to our dict of 

# black cluster
black_cluster = {}
mu = np.array([0., 0., 0., translate_20_to_1(8.), translate_20_to_1(7.)]) # init the color as pure black
# sigma = np.matrix([[1.0, 0., 0., 0., 0.],
# 				   [0., 1.0, 0., 0., 0.],
# 				   [0., 0., 1.0, 0., 0.],
# 				   [0., 0., 0., 4.0, 0.0],
# 				   [0., 0., 0., 0.0, 4.0]])

black_cluster["mean"] = mu
# black_cluster["covar"] = sigma
black_cluster["weight"] = 1.0/num_clusters
cluster_init["black"] = black_cluster # add the black_cluster dict to our dict of clusters

# white cluster
white_cluster = {}
mu = np.array([1., 1., 1., translate_20_to_1(16.), translate_20_to_1(.5)]) # init the color as pure white
# sigma = np.matrix([[1.0, 0., 0., 0., 0.],
# 				   [0., 1.0, 0., 0., 0.],
# 				   [0., 0., 1.0, 0., 0.],
# 				   [0., 0., 0., 4.0, 0.0],
# 				   [0., 0., 0., 0.0, 4.0]])

white_cluster["mean"] = mu
# white_cluster["covar"] = sigma
white_cluster["weight"] = 1.0/num_clusters
cluster_init["white"] = white_cluster # add the white_cluster dict to our dict of clusters

# init covar with estimate
for cluster in cluster_init:
	cluster_init[cluster]["covar"] = np.matrix(np.zeros((5,5)))
	# now calc sigma:
	for y in range(img_y_lim):
		for x in range(img_x_lim):
			# get the sample RGB and append the X, Y
			sample = np.append(img[y][x], np.array([translate_20_to_1(float(x)), translate_20_to_1(float(y))]))
			# xj - ui
			diff = sample - cluster_init[cluster]["mean"]
			# add up sigmas
			sigma_temp = np.matrix( diff * (np.reshape(diff, (5,1)) ))
			cluster_init[cluster]["covar"] += sigma_temp

	# finish sigma calc
	cluster_init[cluster]["covar"] = cluster_init[cluster]["covar"]/(num_pixels - 1)

	print cluster_init[cluster]["covar"]


log_likelyhood_curr = 0	# init log likelyhood
cluster_curr = deepcopy(cluster_init)	# init current cluster vals to the initial ones
p = [[{} for x in range(img_x_lim)] for y in range(img_y_lim)] # init p matrix which is a matrix of pij vals corresponding to each pixel in the image

for step in range(10):
	log_likelyhood_prev = log_likelyhood_curr # save the previous result for comparison later
	log_likelyhood_curr = 0.

	# E-step
	
	# init n to 0 every itteration
	for cluster in cluster_curr:
		cluster_curr[cluster]["n"] = 0.

	# for every pixel
	for y in range(img_y_lim):
		for x in range(img_x_lim):
			# calc pij
			# get the the P(x | C = i) for each cluster
			prob_x_given_c = {}
			for cluster in cluster_curr:
				# get the sample RGB and append the X, Y
				sample = np.append(img[y][x], np.array([translate_20_to_1(float(x)), translate_20_to_1(float(y))]))
				# get the probability by calculating the pdf
				prob_x_given_c[cluster] = norm_pdf_multivariate(sample, cluster_curr[cluster]["mean"], cluster_curr[cluster]["covar"])
				# while we're here, calculate the log likelyhood
				log_likelyhood_curr += log_likelyhood(sample, cluster_curr[cluster]["mean"], cluster_curr[cluster]["covar"])

			# get the denominator in the Bayes' rule calc
			denominator = 0.
			for cluster in cluster_curr:
				denominator += prob_x_given_c[cluster] * cluster_curr[cluster]["weight"]

			# now calc the pij for each cluster and store it in the p matrix
			for cluster in cluster_curr:
				pij = prob_x_given_c[cluster]*cluster_curr[cluster]["weight"]/denominator
				# add the pij val to the n val of each cluster
				cluster_curr[cluster]["n"] += pij
				# if y == 2 and x == 15 :
				# 	print "%s: %f" % (cluster, pij)
				p[y][x][cluster] = pij


	# print the n for each cluster to make sure it makes sense
	# total = 0
	# for cluster in cluster_curr:
	# 	total += cluster_curr[cluster]["n"]
	# 	print "%s : %f" % (cluster, cluster_curr[cluster]["n"])
	# print "total %f" % total

	# print log likelyhood
	print "step %d log likelyhood %f" % (step, log_likelyhood_curr)

	# M-step
	# for every cluster
	for cluster in cluster_curr:
		# init mean and sigma to 0 :
		cluster_curr[cluster]["mean"] = np.array([0., 0., 0., 0., 0.])
		cluster_curr[cluster]["covar"] = np.matrix(np.zeros((5,5)))
		
		# first calculate mu:
		for y in range(img_y_lim):
			for x in range(img_x_lim):
				# get the sample RGB and append the X, Y
				sample = np.append(img[y][x], np.array([translate_20_to_1(float(x)), translate_20_to_1(float(y))]))
				# add up mu
				cluster_curr[cluster]["mean"] += p[y][x][cluster] * sample
				# if y == 2 and x == 15 :
				# 	print "%s: %f" % (cluster, p[y][x][cluster])

		# finis mu calc
		cluster_curr[cluster]["mean"] = cluster_curr[cluster]["mean"]/cluster_curr[cluster]["n"]

		# now calc sigma:
		for y in range(img_y_lim):
			for x in range(img_x_lim):
				# get the sample RGB and append the X, Y
				sample = np.append(img[y][x], np.array([translate_20_to_1(float(x)), translate_20_to_1(float(y))]))
				# xj - ui
				diff = sample - cluster_curr[cluster]["mean"]
				# add up sigmas
				sigma_temp = p[y][x][cluster] * np.matrix( diff * (np.reshape(diff, (5,1)) ))
				cluster_curr[cluster]["covar"] += sigma_temp

		# finish sigma calc
		cluster_curr[cluster]["covar"] = cluster_curr[cluster]["covar"]/cluster_curr[cluster]["n"]

		# finally, update weight
		cluster_curr[cluster]["weight"] = cluster_curr[cluster]["n"] / float(num_pixels)

		# print stuff
		# print "%s:" % cluster
		# print cluster_curr[cluster]["mean"]
		# print cluster_curr[cluster]["covar"]
		if cluster == "black" :
			print cluster_curr[cluster]["mean"]



imgplot = plt.imshow(img)	# create plot
imgplot.set_interpolation('nearest') # we don't want interpolation so edges of pixels aren't "blurry"

for cluster in cluster_init:
	x = translate_1_to_20(cluster_init[cluster]["mean"][3])
	y = translate_1_to_20(cluster_init[cluster]["mean"][4])
	plt.plot(x,y, marker='o', color='b', markersize=15, label="initial mean")

	# cov = cluster_init[cluster]["covar"][3:,3:]
	# plot_cov_ellipse(cov, np.array([x,y]))
	
for cluster in cluster_curr:
	x = translate_1_to_20(cluster_curr[cluster]["mean"][3])
	y = translate_1_to_20(cluster_curr[cluster]["mean"][4])
	plt.plot(x,y, marker='o', color='g', markersize=10, label="final mean")

	cov = cluster_curr[cluster]["covar"][3:,3:]
	plot_cov_ellipse(cov, np.array([x,y]))

#plt.legend(loc=4)

# limit plot bounds
plt.ylim([img_y_lim - 1,0])
plt.xlim([0,img_x_lim - 1])

# cov = np.matrix([[4., 0.], [0., 4.]])
# pos = np.array([7,7])

# plot_cov_ellipse(cov, pos)

# pos = np.array([15,15])

# plot_cov_ellipse(cov, pos)

plt.show()	# show plot