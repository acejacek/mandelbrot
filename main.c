#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define return_defer(value) do { result = (value); goto close_file; } while (0)

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define X_MIN_RANGE -2.0f
#define X_MAX_RANGE 1.2f
#define Y_MIN_RANGE -1.2f
#define Y_MAX_RANGE 1.2f
#define MAX_ITERATIONS 300
#define FILENAME "mandelbrot.ppm"

typedef struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;

typedef int Errno;

Pixel* screen;

static inline float fmap(long x, long in_min, long in_max, float out_min, float out_max)
{
    return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
static inline long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void render(void)
{
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {

            float a = fmap(x, 0, SCREEN_WIDTH, X_MIN_RANGE, X_MAX_RANGE);
            float b = fmap(y, 0, SCREEN_HEIGHT, Y_MIN_RANGE, Y_MAX_RANGE);

            float ca = a;
            float cb = b;

            int n = 0;
            while (++n < MAX_ITERATIONS)
            {
                float aa = a * a - b * b;
                float bb = 2.0f * a * b;

                a = aa + ca;
                b = bb + cb;

                if (a + b > 16) break;
            }

            uint8_t gray = 0;

            if (n < MAX_ITERATIONS)
                gray = (uint8_t) map(n, 0, MAX_ITERATIONS, 0, 255);

            Pixel draw = (Pixel) {gray, gray, gray};

            screen[y * SCREEN_WIDTH + x] = draw;
        }
    }
}

Errno saveToFile()
{
    Errno result = 0;
    FILE* f = NULL;

    f = fopen(FILENAME, "wb");
    if (!f) return_defer(errno);

    fprintf(f, "P6\n%d %d\n255\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (ferror(f)) return_defer(errno);

    fwrite(screen, sizeof *screen, SCREEN_WIDTH * SCREEN_HEIGHT, f);   // save full screen memory
    if (ferror(f)) return_defer(errno);

 close_file:
    if (f) fclose(f);
    return result;
}

int main(void) {

    Errno err = 0;

    screen = malloc(sizeof *screen * SCREEN_WIDTH * SCREEN_HEIGHT);
    assert(screen);

    render();

    err = saveToFile();
    if (err)
        fprintf(stderr, "Error: could not write to file %s: %s\n", FILENAME, strerror(err));

    free(screen);
    return err;
}


