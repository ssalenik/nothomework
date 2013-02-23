
from mdp import *


"""Part I

	question 1
"""

Q1_1 = GridMDP([[-0.04, -0.04, -0.04, -0.04],
				[-0.04, -0.04, -0.04, -0.04],
            	[-0.04, None,  -0.04, -1],
               	[-0.04, -0.04, -0.04, +1]],
               	terminals=[(3, 0), (3, 1)],
               	init=(0,0),
               	gamma = 0.9)

print "value iteration"
print "---------------"
print "state utilities:"
util = value_iteration(Q1_1, 0.0001);
for val in util:
	# increment each index, since we index from 0 in the program
	print "(%i,%i): %r" % (val[0] + 1, val[1] + 1, util[val])
print "policy:"
pi_val = best_policy(Q1_1, util)
print_table(Q1_1.to_arrows(pi_val))

print ""
print "policy iteration"
print "----------------"
print "policy:"
pi_poli = policy_iteration(Q1_1)
print_table(Q1_1.to_arrows(pi_val))


