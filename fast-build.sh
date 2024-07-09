#!/bin/zsh

gcc -Ofast -flto -march=native color_grid_png.c lodepng.c -o color_grid_png -lm
