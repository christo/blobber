#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "lodepng.h"

#define COLOR_DIFF_THRESHOLD 30

typedef struct {
    unsigned char r, g, b, a;
} Color;

typedef struct {
    int x, y;
} Point;

// Function to generate a random color
Color random_color() {
    Color color;
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = 255;  // Opaque
    return color;
}

// Function to calculate the color difference
int color_difference(Color c1, Color c2) {
    return abs(c1.r - c2.r) + abs(c1.g - c2.g) + abs(c1.b - c2.b);
}

// Function to get the neighbors of a pixel
Point get_neighbor(Point p, int direction, int grid_size) {
    Point neighbor = p;
    switch (direction) {
        case 0: neighbor.y = (p.y > 0) ? p.y - 1 : p.y; break; // Up
        case 1: neighbor.y = (p.y < grid_size - 1) ? p.y + 1 : p.y; break; // Down
        case 2: neighbor.x = (p.x > 0) ? p.x - 1 : p.x; break; // Left
        case 3: neighbor.x = (p.x < grid_size - 1) ? p.x + 1 : p.x; break; // Right
        case 4: neighbor.x = (p.x > 0 && p.y > 0) ? p.x - 1 : p.x; neighbor.y = (p.x > 0 && p.y > 0) ? p.y - 1 : p.y; break; // Up-Left
        case 5: neighbor.x = (p.x < grid_size - 1 && p.y > 0) ? p.x + 1 : p.x; neighbor.y = (p.x < grid_size - 1 && p.y > 0) ? p.y - 1 : p.y; break; // Up-Right
        case 6: neighbor.x = (p.x > 0 && p.y < grid_size - 1) ? p.x - 1 : p.x; neighbor.y = (p.x > 0 && p.y < grid_size - 1) ? p.y + 1 : p.y; break; // Down-Left
        case 7: neighbor.x = (p.x < grid_size - 1 && p.y < grid_size - 1) ? p.x + 1 : p.x; neighbor.y = (p.x < grid_size - 1 && p.y < grid_size - 1) ? p.y + 1 : p.y; break; // Down-Right
    }
    return neighbor;
}

// Function to find the centroid of a set of points
Point find_centroid(Point* points, int count) {
    int sum_x = 0, sum_y = 0;
    for (int i = 0; i < count; i++) {
        sum_x += points[i].x;
        sum_y += points[i].y;
    }
    Point centroid = {sum_x / count, sum_y / count};
    return centroid;
}

// Function to save the image as a PNG file
void save_image(Color* grid, int grid_size, int iterations, int save_interval, int save_index) {
    char filename[50];
    if (save_interval > 0) {
        snprintf(filename, sizeof(filename), "output_%05d.png", save_index);
    } else {
        snprintf(filename, sizeof(filename), "output_%dx%d_%d.png", grid_size, grid_size, iterations);
    }
    
    unsigned char* image = (unsigned char*)malloc(grid_size * grid_size * 4);
    for (int y = 0; y < grid_size; y++) {
        for (int x = 0; x < grid_size; x++) {
            int index = y * grid_size + x;
            image[4 * index + 0] = grid[index].r;
            image[4 * index + 1] = grid[index].g;
            image[4 * index + 2] = grid[index].b;
            image[4 * index + 3] = grid[index].a;
        }
    }
    unsigned error = lodepng_encode32_file(filename, image, grid_size, grid_size);
    if (error) {
        printf("Error %u: %s\n", error, lodepng_error_text(error));
    }
    free(image);
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Usage: %s <iterations> <grid_size> [-m]\n", argv[0]);
        return 1;
    }

    int iterations = atoi(argv[1]);
    int grid_size = atoi(argv[2]);
    int save_interval = (argc == 4 && strcmp(argv[3], "-m") == 0) ? 100000 : 0;
    int save_index = 0;

    srand(time(NULL));

    // Initialize the grid with random colors
    Color* grid = (Color*)malloc(grid_size * grid_size * sizeof(Color));
    for (int i = 0; i < grid_size * grid_size; i++) {
        grid[i] = random_color();
    }

    // Perform the iterations
    for (int iter = 0; iter < iterations; iter++) {
        Point a = {rand() % grid_size, rand() % grid_size};
        Color color_a = grid[a.y * grid_size + a.x];

        Point similar_points[grid_size * grid_size];
        int similar_count = 0;

        for (int y = 0; y < grid_size; y++) {
            for (int x = 0; x < grid_size; x++) {
                if (color_difference(color_a, grid[y * grid_size + x]) < COLOR_DIFF_THRESHOLD) {
                    similar_points[similar_count++] = (Point){x, y};
                }
            }
        }

        if (similar_count == 0) continue;

        Point centroid = find_centroid(similar_points, similar_count);

        // Find the neighbor of `a` that is closest to the centroid
        int best_direction = 0;
        double best_distance = hypot(a.x - centroid.x, a.y - centroid.y);

        for (int dir = 0; dir < 8; dir++) {
            Point neighbor = get_neighbor(a, dir, grid_size);
            double distance = hypot(neighbor.x - centroid.x, neighbor.y - centroid.y);
            if (distance < best_distance) {
                best_distance = distance;
                best_direction = dir;
            }
        }

        Point b = get_neighbor(a, best_direction, grid_size);

        // Swap the colors of `a` and its best neighbor
        Color temp = grid[a.y * grid_size + a.x];
        grid[a.y * grid_size + a.x] = grid[b.y * grid_size + b.x];
        grid[b.y * grid_size + b.x] = temp;

        // Save image every save_interval iterations if -m option is specified
        if (save_interval > 0 && (iter + 1) % save_interval == 0) {
            save_image(grid, grid_size, iterations, save_interval, save_index++);
        }
    }

    // Save the resulting grid as a PNG file
    if (save_interval == 0) {
        save_image(grid, grid_size, iterations, 0, 0);
    }

    // Create the shell script to compile the video if -m option is specified
    if (save_interval > 0) {
        FILE* script = fopen("compile-video.sh", "w");
        if (script) {
            fprintf(script, "#!/bin/bash\n");
            fprintf(script, "ffmpeg -framerate 1 -i output_%%05d.png -vf \"scale=1080:-1, pad=1080:1080:(ow-iw)/2:(oh-ih)/2\" -c:v libx264 -r 30 -pix_fmt yuv420p output_video.m4v\n");
            fclose(script);
            system("chmod +x compile-video.sh");
        } else {
            printf("Failed to create compile-video.sh\n");
        }
    }

    free(grid);
    return 0;
}
