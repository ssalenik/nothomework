# ECSE 526
# Assignment 2
# Stepan Salenikovich
# Feb 27, 2013

import numpy as np
from mdp import *

print """

Q5
---------------
"""
increment = 0.5	# the amount by which r will be incrmented

# init r with the given increment
r = np.arange(-4.0, 0, increment)

# init world with different values of r
world = []
for i in range(len(r)):
	world.append(GridMDP([[r[i], r[i], None, r[i]],
					[r[i], r[i], r[i], r[i]],
	            	[r[i], None,  r[i], -1],
	               	[r[i], r[i], r[i], +1]],
	               	terminals=[(3, 0), (3, 1)],
	               	init=(0,0),
	               	gamma = 0.9))

# evaluate
u = []
for i in range(len(r)):
	u.append(value_iteration(world[i], 0.0001))

print "optimal policy for r = -3.0"
i = 0
while r[i] < -3.0:
	i += 1
pi_val = best_policy(world[i], u[i])
print_table(world[i].to_arrows(pi_val))

print ""
print "optimal policy for r = -1.0"
i = 0
while r[i] < -1.0:
	i += 1
pi_val = best_policy(world[i], u[i])
print_table(world[i].to_arrows(pi_val))

print "---------------"

# state (3,2), (2,1), (4,4)
y = [[],[],[]]
for i in range(len(r)):
	y[0].append(u[i][(2,1)]) #(3,2)
	y[1].append(u[i][(1,0)]) #(2,1)
	y[2].append(u[i][(3,3)]) #(4,4)

# plot utilites of state (3,2), (2,1), and (4,4) v.s. r
import matplotlib.pyplot as plt
plt.plot(r, y[0], marker='o', linestyle='-', label = 'U(3,2)')
plt.plot(r, y[1], marker='x', linestyle='-', label = 'U(2,1)')
plt.plot(r, y[2], marker='v', linestyle='-', label = 'U(4,4)')
plt.legend(loc=4)
plt.ylabel("state utility")
plt.xlabel("value of r")
plt.show()



