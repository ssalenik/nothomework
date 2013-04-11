import argparse
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import math
from copy import deepcopy
import math

from cov_ellipse import *
from stat_learning import *
from util import *

default_image_location = "fish_02_02.png"

parser = argparse.ArgumentParser()
parser.add_argument("image", nargs='?', default=default_image_location, help="location of the image; default image used otherwise")
parser.add_argument("-i", default=10, type=int, required=False, help="number of iterations to try")
parser.add_argument("-c", default=3, type=int, required=False, help="number of clusters to start with")
image_location = parser.parse_args().image
iterations = parser.parse_args().i
num_clusters = parser.parse_args().c

print "using image: %s" % image_location

# read the image
img = mpimg.imread(image_location)
if img.shape[2] > 2:
	img = img[:,:,:-1]
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
cluster_init = {}	# init cluster dict

for cluster_idx in range(num_clusters):
	new_cluster = {}
	mu = np.array([
		np.random.uniform(), #0, 255), 
		np.random.uniform(), #0, 255), 
		np.random.uniform(), #0, 255), 
		np.random.uniform(0, img_x_lim),
		np.random.uniform(0, img_y_lim)])

	new_cluster["mean"] = mu
	new_cluster["weight"] = 1.0/num_clusters
	cluster_init[cluster_idx] = new_cluster

# each pixel is [R G B X Y]
# mean is [R G B X Y]
# covar matrix is:
# | R.R, R.G, R.B, R.X, R.Y |
# | G.R, G.G, G.B, G.X, G.Y |
# | B.R, B.G, B.B, B.X, B.Y |
# | X.R, X.G, X.B, X.X, X.Y |
# | Y.R, Y.G, Y.B, Y.X, Y.Y |
# 

# # red cluster
# red_cluster = {}
# mu = np.array([1., 0., 0., translate_20_to_1(14.), translate_20_to_1(14.)]) # good init
# # mu = np.array([1., 0., 0., translate_20_to_1(8.), translate_20_to_1(14.)]) # bad x,y init
# # mu = np.array([.5, .5, .5, translate_20_to_1(14.), translate_20_to_1(14.)]) # grey init
# # sigma = np.matrix([[1.0, 0.9, 0.9, 0.9, 0.9],
# # 				   [0.9, 1.0, 0.9, 0.9, 0.9],
# # 				   [0.9, 0.9, 1.0, 0.9, 0.9],
# # 				   [0.9, 0.9, 0.9, 4.0, 0.90],
# # 				   [0.9, 0.9, 0.9, 0.9, 4.0]])

# red_cluster["mean"] = mu
# #red_cluster["covar"] = sigma
# red_cluster["weight"] = 1.0/num_clusters
# cluster_init["red"] = red_cluster # add the red_cluster dict to our dict of clusters

cluster_white = np.matrix(
		[	[  1.01254777e-02,   1.69263376e-02 ,  1.94734238e-02,  -2.10194317e-01, -5.84128392e-02],
 			[  1.69263376e-02,   5.22945196e-02,   6.85038350e-02,  -7.07998035e-01, 2.98656366e-02],
 			[  1.94734238e-02,   6.85038350e-02,   1.11091603e-01,  -9.79523151e-01, 8.42753488e-03],
 			[ -2.10194317e-01,  -7.07998035e-01,  -9.79523151e-01,   1.40573685e+02, 1.71404103e+01],
 			[ -5.84128392e-02,   2.98656366e-02,   8.42753488e-03,   1.71404103e+01, 1.75656083e+01]])

cluster_fish = np.matrix(
 		[	[  6.68955480e-58,   1.33791090e-57,   2.00619865e-57,  -9.66895963e-55, -5.11817610e-54],
			[  1.33791090e-57,   1.35513387e-12,   4.06345758e-12,  -3.33021098e-09, -1.07102148e-08],
			[  2.00619865e-57,   4.06345758e-12,   1.53446570e-08,   4.36877803e-05, -8.68006800e-05],
			[ -9.66895963e-55,  -3.33021098e-09,   4.36877803e-05,   1.39082276e+03, -4.32053377e+00],
			[ -5.11817610e-54,  -1.07102148e-08,  -8.68006800e-05,  -4.32053377e+00, 7.72572442e+02]])

# init covar with estimate
for cluster in cluster_init:
	cluster_init[cluster]["covar"] = np.matrix(np.zeros((5,5)))
	# now calc sigma:

	for y in range(img_y_lim):
		for x in range(img_x_lim):
			# get the sample RGB and append the X, Y
			sample = np.append(img[y][x], np.array([x, y]))
			# xj - ui
			diff = sample - cluster_init[cluster]["mean"]
			# add up sigmas
			sigma_temp = np.matrix( diff * (np.reshape(diff, (5,1)) ))
			cluster_init[cluster]["covar"] += sigma_temp

	# finish sigma calc
	cluster_init[cluster]["covar"] = cluster_init[cluster]["covar"]/(num_pixels - 1)

	if cluster == 0 :
		cluster_init[cluster]["covar"][:3,:3] = cluster_white[:3,:3]
	else :
		cluster_init[cluster]["covar"][:3,:3] = cluster_fish[:3,:3]

	# # print cluster_init[cluster]["covar"]


log_likelyhood_curr = 0	# init log likelyhood
cluster_curr = deepcopy(cluster_init)	# init current cluster vals to the initial ones
p = [[{} for x in range(img_x_lim)] for y in range(img_y_lim)] # init p matrix which is a matrix of pij vals corresponding to each pixel in the image

for step in range(iterations):
	log_likelyhood_prev = log_likelyhood_curr # save the previous result for comparison later
	log_likelyhood_curr = 0.

	# save previoius itteration results
	cluster_prev = deepcopy(cluster_curr)
	p_prev = deepcopy(p)

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
				sample = np.append(img[y][x], np.array([x, y]))
				# get the probability by calculating the pdf
				prob_x_given_c[cluster] = norm_pdf_multivariate(sample, cluster_curr[cluster]["mean"], cluster_curr[cluster]["covar"])
				# while we're here, calculate the log likelyhood
				# log_likelyhood_curr += log_likelyhood(sample, cluster_curr[cluster]["mean"], cluster_curr[cluster]["covar"])

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
				sample = np.append(img[y][x], np.array([x, y]))
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
				sample = np.append(img[y][x], np.array([x, y]))
				# xj - ui
				diff = sample - cluster_curr[cluster]["mean"]
				# add up sigmas
				sigma_temp = p[y][x][cluster] * np.matrix( diff * (np.reshape(diff, (5,1)) ))
				cluster_curr[cluster]["covar"] += sigma_temp

		# finish sigma calc
		cluster_curr[cluster]["covar"][:3,:3] = cluster_curr[cluster]["covar"][:3,:3]/cluster_curr[cluster]["n"]


		cluster_curr[cluster]["covar"][:3,:3] = cluster_init[cluster]["covar"][:3,:3]

		# make sure vals don't get too small
		# set_lower_bound(cluster_curr[cluster]["covar"], 0.0001)

		# finally, update weight
		cluster_curr[cluster]["weight"] = cluster_curr[cluster]["n"] / float(num_pixels)

		print "n = %f" % cluster_curr[cluster]["n"]

		# print stuff
		# print "%s:" % cluster
		# print cluster_curr[cluster]["mean"]
		# print cluster_curr[cluster]["covar"]

	# check determinant
	for cluster in cluster_curr:
		det = np.linalg.det(cluster_curr[cluster]["covar"])
		# print "%s: %f" % (cluster, det)
		if det <= .0:
			print "%s det too small, reseting covariance" % cluster
			# cluster_curr[cluster]["mean"] = cluster_init[cluster]["mean"]
			cluster_curr[cluster]["covar"] = cluster_init[cluster]["covar"]

	# get the new log likelyhood
	# for every pixel
	for y in range(img_y_lim):
		for x in range(img_x_lim):
			l = -99999.
			for cluster in cluster_curr:
				sample_l = log_likelyhood(sample, cluster_curr[cluster]["mean"], cluster_curr[cluster]["covar"])
				if sample_l > l :
					l = sample_l
					w = cluster_curr[cluster]["weight"]

			log_likelyhood_curr += math.log(w) + l

	# check to make sure log likelyhood is still increasing
	# if log_likelyhood_curr < log_likelyhood_prev and step != 0:
	# 	print "max log likelyhood reached, stopping"
	# 	cluster_curr = cluster_prev
	# 	p = p_prev
	# 	break

	# print log likelyhood
	print "step %d log likelyhood %f" % (step + 1, log_likelyhood_curr)

	for cluster in cluster_curr:
		print cluster_curr[cluster]["covar"]
		print cluster_curr[cluster]["mean"]

	plt.figure()
	plt.subplot(121)
	imgplot = plt.imshow(img)	# create plot
	imgplot.set_interpolation('nearest') # we don't want interpolation so edges of pixels aren't "blurry"

	for cluster in cluster_init:
		x = cluster_init[cluster]["mean"][3]
		y = cluster_init[cluster]["mean"][4]
		plt.plot(x,y, marker='o', color='b', markersize=10, label="initial mean")

		# cov = cluster_init[cluster]["covar"][3:,3:]
		# plot_cov_ellipse(cov, np.array([x,y]))
		
	for cluster in cluster_curr:
		x = cluster_curr[cluster]["mean"][3]
		y = cluster_curr[cluster]["mean"][4]
		plt.plot(x,y, marker='o', color='g', markersize=10, label="final mean")

		cov = cluster_curr[cluster]["covar"][3:,3:]
		plot_cov_ellipse(cov, np.array([x,y]))

	#plt.legend(loc=4)

	# limit plot bounds
	plt.ylim([img_y_lim - 1,0])
	plt.xlim([0,img_x_lim - 1])

	# results
	img_result = deepcopy(img)

	# for every pixel
	for y in range(img_y_lim):
		for x in range(img_x_lim):
			pij_max = 0
			for cluster in cluster_curr:
				pij = p[y][x][cluster]
				if pij > pij_max:
					pij_max = pij
					new_color = cluster_curr[cluster]["mean"][:3]
			img_result[y][x] = new_color

	plt.subplot(122)
	plt.ylim([img_y_lim - 1,0])
	plt.xlim([0,img_x_lim - 1])

	imgplot_result = plt.imshow(img_result)	# create plot
	imgplot_result.set_interpolation('nearest') # we don't want interpolation so edges of pixels aren't "blurry"

	if step < iterations - 1:
		plt.show(block=False)	# show plot
	else:
		plt.show()
