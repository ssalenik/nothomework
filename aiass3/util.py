import numpy as np

def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)

def translate_20_to_1(value):
	return translate(value, 0., 20., 0., 1.)

def translate_1_to_20(value):
	return translate(value, 0., 1., 0., 20.)

def set_lower_bound(matrix, bound):
	for e in np.nditer(matrix, op_flags=['readwrite']):
		if e < 0. and -e < bound :
			e[...] = -bound
		elif e > 0. and e < bound:
			e[...] = bound