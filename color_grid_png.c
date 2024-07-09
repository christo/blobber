#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include "lodepng.h"

typedef struct {
    uint8_t r, g, b;
} Pixel;

typedef struct {
    int x, y;
} Point;

int abs_diff(int a, int b) {
    return abs(a - b);
}

int color_distance(Pixel a, Pixel b) {
    return abs_diff(a.r, b.r) + abs_diff(a.g, b.g) + abs_diff(a.b, b.b);
}

void initialize_grid(Pixel **grid, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            grid[i][j].r = rand() % 256;
            grid[i][j].g = rand() % 256;
            grid[i][j].b = rand() % 256;
        }
    }
}

Point find_centroid(Pixel **grid, int size, Pixel ref, int threshold) {
    int sum_x = 0, sum_y = 0, count = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (color_distance(grid[i][j], ref) < threshold) {
                sum_x += i;
                sum_y += j;
                count++;
            }
        }
    }
    Point centroid = {sum_x / count, sum_y / count};
    return centroid;
}

Pixel get_closest_neighbor(Pixel **grid, int size, int x, int y, Point target) {
    Pixel closest_pixel;
    int min_distance = INT_MAX;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                int dist = abs_diff(nx, target.x) + abs_diff(ny, target.y);
                if (dist < min_distance) {
                    min_distance = dist;
                    closest_pixel = grid[nx][ny];
                }
            }
        }
    }

    return closest_pixel;
}

void swap_colors(Pixel *a, Pixel *b) {
    Pixel temp = *a;
    *a = *b;
    *b = temp;
}

void process_grid(Pixel **grid, int size, int iterations, int threshold) {
    if (iterations >= 100) {
        for (int i = 0; i < 100; ++i) {
            printf("_");
        }
        printf("\n");
    }

    int dot_interval = iterations >= 100 ? iterations / 100 : 1;

    for (int iter = 0; iter < iterations; ++iter) {
        if (iterations >= 100 && iter > 0 && iter % dot_interval == 0) {
            printf(".");
            fflush(stdout);
        }

        int x = rand() % size;
        int y = rand() % size;
        Pixel ref = grid[x][y];
        Point centroid = find_centroid(grid, size, ref, threshold);
        Pixel closest_neighbor = get_closest_neighbor(grid, size, x, y, centroid);
        swap_colors(&grid[x][y], &closest_neighbor);
    }

    if (iterations >= 100) {
        printf("\n");
    }
}

void write_png(const char *filename, Pixel **grid, int size) {
    unsigned char *image = malloc(size * size * 4);
    unsigned char *ptr = image;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            *ptr++ = grid[i][j].r;
            *ptr++ = grid[i][j].g;
            *ptr++ = grid[i][j].b;
            *ptr++ = 255;  // Alpha channel set to 255 (opaque)
        }
    }

    unsigned error = lodepng_encode32_file(filename, image, size, size);
    if (error) {
        fprintf(stderr, "Error %u: %s\n", error, lodepng_error_text(error));
    }

    free(image);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <iterations> <size>\n", argv[0]);
        return 1;
    }

    int iterations = atoi(argv[1]);
    int size = atoi(argv[2]);

    if (iterations <= 0 || size <= 0) {
        fprintf(stderr, "Both iterations and size must be positive integers.\n");
        return 1;
    }

    srand(time(NULL));

    Pixel **grid = malloc(size * sizeof(Pixel *));
    for (int i = 0; i < size; ++i) {
        grid[i] = malloc(size * sizeof(Pixel));
    }

    initialize_grid(grid, size);
    process_grid(grid, size, iterations, 50);

    char filename[64];
    snprintf(filename, sizeof(filename), "output_%dx%d_%diterations.png", size, size, iterations);
    write_png(filename, grid, size);

    for (int i = 0; i < size; ++i) {
        free(grid[i]);
    }
    free(grid);

    return 0;
}