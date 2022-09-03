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
      //if (x0 == x1) break;
      error += dy;
      x0 += sx;
      write_x(x0);
    }
    if (e2 <= dx) {
      //if (y0 == y1) break;
      error += dx;
      y0 += sy;
      write_y(y0);
    }
  }
}

template <uint8_t Q>
void write_quad_x(int8_t xm, int8_t ym, int8_t x) {
  if (Q == 0) {
    write_x(xm - x);
  } else if (Q == 1) {
    write_y(ym - x);
  } else if (Q == 2) {
    write_x(xm + x);
  } else if (Q == 3) {
    write_y(ym + x);
  }
}

template <uint8_t Q>
void write_quad_y(int8_t xm, int8_t ym, int8_t y) {
  if (Q == 0) {
    write_y(ym + y);
  } else if (Q == 1) {
    write_x(xm - y);
  } else if (Q == 2) {
    write_y(ym - y);
  } else if (Q == 3) {
    write_x(xm + y);
  }
}

template <uint8_t Q>
void draw_quadrant(int8_t xm, int8_t ym, int8_t r) {
  // http://members.chello.at/~easyfilter/bresenham.html
  int8_t x = -r;
  int8_t y = 0;
  int8_t err = 2 - 2*r;

  write_quad_x<Q>(xm, ym, x);
  write_quad_y<Q>(xm, ym, y);
  for (;;) {
    if (x == 0) break;
    r = err;
    if (r <= y) {
      ++y;
      err += y*2 + 1;
      write_quad_y<Q>(xm, ym, y);
    }
    if (r > x || err > y) {
      ++x;
      err += x*2 + 1;
      write_quad_x<Q>(xm, ym, x);
    }
  }
}

void draw_circle(int8_t xm, int8_t ym, int8_t r) {
  draw_quadrant<0>(xm, ym, r);
  draw_quadrant<1>(xm, ym, r);
  draw_quadrant<2>(xm, ym, r);
  draw_quadrant<3>(xm, ym, r);
}

void draw_star_idle() {
  // TODO using 63 for y endpoint glitches display
  draw_line(14, 8, 32, 62);
  draw_line(32, 62, 50, 8);
  draw_line(50, 8, 3, 42);
  draw_line(3, 42, 60, 42);
  draw_line(60, 42, 14, 8);
  draw_circle(32, 32, 31);
}

void draw_star(Args) {
  idle_fn = draw_star_idle;
}
