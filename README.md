# blobber

Experiments in monte carlo simulations of pixels in a grid, based on colour. 

![](https://youtu.be/CjznlQxLma0)

# how it works

An initial grid is filled with random colours. At each step, a random pixel is chosen and it swaps with the one of its immediate neighbours who is closest to the centroid of of all the other pixels closer in colour to that pixel than some arbitrary threshold. Then another random pixel is chosen and the process repeats.

After enough iterations, colours will be observed to group together although random jittering in position continues. Depending on the threshold, various shapes like oil paint in water can be seen swirling in the timelapse video above. 

Includes [lodepng](https://github.com/lvandeve/lodepng) as source, license for that is in the source and at that repo.


