import argparse
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np

defaultImageLocation = "images/image1_a.png"

parser = argparse.ArgumentParser()
parser.add_argument("image", nargs='?', default=defaultImageLocation, help="location of the image; default image used otherwise")
imageLocation = parser.parse_args().image

print "using image: %s" % imageLocation

img = mpimg.imread(imageLocation)

# print img

imgplot = plt.imshow(img)

plt.show()