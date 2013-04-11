import Image
import numpy as np
im = Image.open("fish_02_02.png")

# To iterate through the entire gif
print im
pix = im.convert("LA")
width, height = pix.size

for x in range(width):
	for y in range(height):
		if pix.getpixel((x,y))[0] < 210:
			#print pix.getpixel((x,y))[0]
			pix.putpixel((x,y), (0, 255))
		else :
			pix.putpixel((x,y), (255, 255))

pix.show()

pix.save("fish_04.png") 

