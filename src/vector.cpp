// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

void draw_line(int8_t x0, int8_t y0, int8_t x1, int8_t y1) {
  // https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
  int8_t dx = x0 < x1 ? x1 - x0 : x0 - x1;
  int8_t sx = x0 < x1 ? 1 : -1;
  int8_t dy = y0 < y1 ? y0 - y1 : y1 - y0; // should be negative
  int8_t sy = y0 < y1 ? 1 : -1;
  int8_t error = dx + dy;

  write_x(x0);
  write_y(y0);
  for (;;) {
    if (x0 == x1 && y0 == y1) break;
    int8_t e2 = 2 * error;
    if (e2 >= dy) {
      if (x0 == x1) break;
      error += dy;
      x0 += sx;
      write_x(x0);
    }
    if (e2 <= dx) {
      if (y0 == y1) break;
      error += dx;
      y0 += sy;
      write_y(y0);
    }
  }
}

void draw_star_idle() {
  draw_line(16, 8, 32, 56);
  draw_line(32, 56, 48, 8);
  draw_line(48, 8, 8, 40);
  draw_line(8, 40, 56, 40);
  draw_line(56, 40, 16, 8);
}

void draw_star(Args) {
  idle_fn = draw_star_idle;
}
