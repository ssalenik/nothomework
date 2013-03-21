from numpy import *
import math

# calculates the pdf of the multivariate normal distribution
# code from:
# http://stackoverflow.com/questions/11615664/multivariate-normal-density-in-python
#
def norm_pdf_multivariate(x, mu, sigma):
	size = len(x)
	shape = (size, size)
	if (size == len(mu)) and ((size, size) == sigma.shape):
		det = linalg.det(sigma)
		if det <= 0:
			raise NameError("The covariance matrix can't be singular and must be positive definite")
		norm_const = 1.0/ ( math.pow((2*math.pi),float(size)/2) * math.pow(det,1.0/2))
		x_mu = matrix(x - mu)
		inv = sigma.I        
		result = math.pow(math.e, -0.5 * (x_mu * inv * x_mu.T))
		return norm_const * result
	else:
		raise NameError("The dimensions of the input don't match")

# returns the log likelyhood of the multivariate normal distribution
# adapted from the function abvove
def log_likelyhood(x, mu, sigma):
	size = len(x)
	if size == len(mu) and (size, size) == sigma.shape:
		det = linalg.det(sigma)
		if det == 0:
			raise NameError("The covariance matrix can't be singular")
		term1 = -size/2 * math.log(2*math.pi)
		term2 = -0.5 * math.log(det)
		inv = sigma.I 
		x_mu = matrix(x - mu)
		term3 = -0.5 * (x_mu * inv * x_mu.T)
		result = term1 + term2 + term3
		return result
	else:
		raise NameError("The dimensions of the input don't match")