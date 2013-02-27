
from mdp import *

print """
part I
Q1
---------------
"""

world = GridMDP([[-0.04, -0.04, None, -0.04],
				[-0.04, -0.04, -0.04, -0.04],
            	[-0.04, None,  -0.04, -1],
               	[-0.04, -0.04, -0.04, +1]],
               	terminals=[(3, 0), (3, 1)],
               	init=(0,0),
               	gamma = 0.9)

print "value iteration"
print "---------------"
print "state utilities:"
util = value_iteration(world, 0.0001);
print_table(world.to_grid(util))
print ""
print "policy:"
pi_val = best_policy(world, util)
print_table(world.to_arrows(pi_val))

print ""
print "policy iteration"
print "----------------"
print "state utilities:"
pi_poli, util = policy_iteration(world)
print_table(world.to_grid(util))
print ""
print "policy:"
print_table(world.to_arrows(pi_val))
print "---------------"

