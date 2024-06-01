#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mandelbrot.h"

#define WIDTH 1000
#define HEIGHT 1000
#define ITERATION_STEP 5

void displayRGBPixels(uint8_t *pixelArray, int width, int height) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int pixelIdx = 3 * (row * width + col);
            al_draw_pixel(col, row, al_map_rgb(
                    pixelArray[pixelIdx],
                    pixelArray[pixelIdx + 1],
                    pixelArray[pixelIdx + 2]
            ));
        }
    }
}

void mandelbrotgenC(uint8_t *pixels, int width, int height, int max_it) {
    double min_real = -2.0;
    double max_real = 2.0;
    double min_imag = -2;
    double max_imag = 2;

    double real_factor = (max_real - min_real) / (width - 1);
    double imag_factor = (max_imag - min_imag) / (height - 1);

    for (int y = 0; y < height; y++) {
        double imag = min_imag + y * imag_factor;

        for (int x = 0; x < width; x++) {
            double real = min_real + x * real_factor;

            double zr = 0.0, zi = 0.0;
            int it;
            for (it = 0; it < max_it; it++) {
                double zr2 = zr * zr;
                double zi2 = zi * zi;
                if (zr2 + zi2 > 4.0) break;

                double temp = zr2 - zi2 + real;
                zi = 2.0 * zr * zi + imag;
                zr = temp;
            }

            int pixelIdx = 3 * (y * width + x);
            uint8_t color = (it == max_it) ? 0 : (255 * it / max_it);
            pixels[pixelIdx] = color;       // Red
            pixels[pixelIdx + 1] = color;   // Green
            pixels[pixelIdx + 2] = color;   // Blue
        }
    }
}
int main() {
    if (!al_init()) {
        fprintf(stderr, "Failed to initialize Allegro.\n");
        return -1;
    }
    if (!al_install_keyboard()) {
        fprintf(stderr, "Failed to install keyboard.\n");
        return -1;
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_DISPLAY *disp = al_create_display(WIDTH, HEIGHT);
    ALLEGRO_FONT *font = al_create_builtin_font();

    if (!timer || !queue || !disp || !font) {
        fprintf(stderr, "Failed to create Allegro resources.\n");
        return -1;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    uint8_t *pixels = (uint8_t *)malloc(WIDTH * HEIGHT * 3);
    int max_iterations = ITERATION_STEP;

    mandelbrotgen(pixels, WIDTH, HEIGHT, ITERATION_STEP);
    displayRGBPixels(pixels, WIDTH, HEIGHT);
    al_flip_display();
    al_start_timer(timer);

    while (true) {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                break;
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                if (max_iterations > ITERATION_STEP) {
                    max_iterations -= ITERATION_STEP;
                    printf("removing\n");
                }
            } else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                max_iterations += ITERATION_STEP;
                printf("adding\n");
            }
            redraw = true;

        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0)); // Clear the display

            mandelbrotgen(pixels, WIDTH, HEIGHT, max_iterations);
            displayRGBPixels(pixels, WIDTH, HEIGHT);
            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    free(pixels);
    return 0;
}
