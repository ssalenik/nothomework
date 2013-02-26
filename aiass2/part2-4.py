from utils import *
from mdp import *

# extending the MDP class to solve the kangaroo world problem
class KangarooMDP(MDP):
    """A two-dimensional MDP, where the first dimension represents the position in
    the world and the second the jump vector used to get to that position.
    All you have to do is specify the grid as a list of lists of rewards;
    use None for an obstacle (unreachable state).  Also, you should specify the
    terminal states.
    An action is a jump vector - an integer in the range of -2 to 2"""

    def __init__(self, grid, terminals, init=(0, 0), gamma=.9):
        #grid.reverse() ## because we want row 0 on bottom, not on top

        # possible jump vectors based on previous
        # such that the law of inertia is respected
        jump_vectors = {
            -2:[-2, -1],
            -1:[-2, -1, 0],
             0:[-1, 0, 1],
             1:[0, 1, 2],
             2:[1, 2]
        }

        # mapping of row # to jump vector
        self.row_to_jv = {
            0:-2, 1:-1, 2:0, 3:1, 4:2
        }

        # maping of jump vector to row
        self.jv_to_row = {
            -2:0, -1:1, 0:2, 1:3, 2:4
        }

        MDP.__init__(self, init, actlist=jump_vectors,
                     terminals=terminals, gamma=gamma)
        update(self, grid=grid, rows=len(grid), cols=len(grid[0]))
        for x in range(self.rows):
            for y in range(self.cols):
                self.reward[x, y] = grid[x][y]
                if grid[x][y] is not None:
                    self.states.add((x, y))

    def T(self, state, action):
        if action is None:
            return [(0.0, state)]
        else:
            # the action is determenistic, so return with probability of 1
            return [(1.0, self.go(state, action))]

    def actions(self, state):
        """Set of actions that can be performed in this state.  The actions
        depend on the state due to the law of inertia."""
        if state in self.terminals:
            return [None]
        else:
            # possible actions depend ont the jump vector
            # the row # determines the jump vector
            y = state[1]
            return self.actlist[self.row_to_jv[y]]

    def go(self, state, direction):
        "Return the state that results from this jump vector."
        position = state[0]
        vector = direction

        # result of jump
        position = position + vector
        
        # check if we bounced off the left wall
        if position < 0:
            position = -(position + 1)
            vector = -vector
        # check if we bounced off the right wall
        elif position >= self.rows:
            position = (self.rows - 1) - (position - self.rows)
            vector = -vector

        return (position, self.jv_to_row[vector])

    def to_grid(self, mapping):
        """Convert a mapping from (x, y) to v into a [[..., v, ...]] grid."""
        return list(reversed([[mapping.get((x,y), None)
                               for x in range(self.rows)]
                              for y in range(self.cols)]))

#______________________________________________________________________________

r = [-0.04, -0.5, -1]

# init the world with the different r vals
world = []
for i in range(len(r)):
    world.append(KangarooMDP([  # -2    -1    0      1      2
                                [r[i], r[i], r[i], r[i], r[i]],     # 1
                                [r[i], r[i], r[i], r[i], r[i]],     # 2
                                [-1,   -1,   -1,   -1,   -1  ],     # 3
                                [r[i], r[i], r[i], r[i], r[i]],     # 4
                                [r[i], r[i], +1,   r[i], r[i]],     # 5
                                [r[i], r[i], r[i], r[i], r[i]],     # 6
                                [-1,   -1,   -1,   -1,   -1  ],     # 7
                                [r[i], r[i], r[i], r[i], r[i]],     # 8
                                [r[i], r[i], r[i], r[i], r[i]]],    # 9

                                terminals=[ (2, 0), (6, 0), (4, 2),
                                            (2, 1), (6, 1),
                                            (2, 2), (6, 2),
                                            (2, 3), (6, 3),   
                                            (2, 4), (6, 4)],
                                init=(0,2),
                                gamma = 0.9))
#______________________________________________________________________________

for i in range(len(r)):
    print "r = %r" % r[i]
    print "value iteration"
    print "---------------"
    print "state utilities:"
    util = value_iteration(world[i], 0.0001);
    print_table(world[i].to_grid(util))

    print ""
    print "policy:"
    pi_val = best_policy(world[i], util)

    print_table(world[i].to_grid(pi_val))

    print ""
    print "policy iteration"
    print "----------------"
    print "state utilities:"
    pi_poli, util = policy_iteration(world[i])
    util = value_iteration(world[i], 0.0001);
    print_table(world[i].to_grid(util))
    print ""
    print "policy:"
    print_table(world[i].to_grid(pi_poli))
    print "--------------------------------\n"

