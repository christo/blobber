#!/bin/zsh

#gcc -Ofast -march=native color_grid_ppm.c -o color_grid_ppm -lm
gcc -Ofast -flto -march=native color_grid_ppm.c -o color_grid_ppm -lm
gcc -Ofast -flto -march=native color_grid_png.c lodepng.c -o color_grid_png -lm
