// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

#include "core/util.hpp"

void draw_line(int8_t x0, int8_t y0, int8_t x1, int8_t y1) {
  // https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
  // https://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
  int8_t dx = x0 < x1 ? x1 - x0 : x0 - x1;
  int8_t sx = x0 < x1 ? 1 : -1;
  int8_t dy = y0 < y1 ? y1 - y0 : y0 - y1;
  int8_t sy = y0 < y1 ? 1 : -1;
  int8_t error = (dx > dy ? dx : -dy) / 2;

  DAC::X::write(x0);
  DAC::Y::write(y0);
  while (x0 != x1 || y0 != y1) {
    int8_t e2 = error;
    if (e2 > -dx) {
      error -= dy;
      x0 += sx;
      DAC::X::write(x0);
    }
    if (e2 < dy) {
      error += dx;
      y0 += sy;
      DAC::Y::write(y0);
    }
  }
}

template <uint8_t Q>
void write_quad_x(int8_t xm, int8_t ym, int8_t x) {
  if (Q == 0) {
    DAC::X::write(xm - x);
  } else if (Q == 1) {
    DAC::Y::write(ym - x);
  } else if (Q == 2) {
    DAC::X::write(xm + x);
  } else if (Q == 3) {
    DAC::Y::write(ym + x);
  }
}

template <uint8_t Q>
void write_quad_y(int8_t xm, int8_t ym, int8_t y) {
  if (Q == 0) {
    DAC::Y::write(ym + y);
  } else if (Q == 1) {
    DAC::X::write(xm - y);
  } else if (Q == 2) {
    DAC::Y::write(ym - y);
  } else if (Q == 3) {
    DAC::X::write(xm + y);
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
  while (x != 0) {
    r = err;
    if (r <= y) {
      ++y;
      err += y*2 + 1;
      write_quad_y<Q>(xm, ym, y);
    }
    if (r > x || err > y) {
      ++x;
      write_quad_x<Q>(xm, ym, x);
      err += x*2 + 1;
    }
  }
}

void draw_circle(int8_t xm, int8_t ym, int8_t r) {
  draw_quadrant<0>(xm, ym, r);
  draw_quadrant<1>(xm, ym, r);
  draw_quadrant<2>(xm, ym, r);
  draw_quadrant<3>(xm, ym, r);
}

void circle_idle() {
  // Cosine in X, sine in Y
  constexpr uint8_t CENTER_X = DAC::X::RESOLUTION / 2;
  constexpr uint8_t CENTER_Y = DAC::Y::RESOLUTION / 2;
  constexpr uint8_t RADIUS = util::min(CENTER_X, CENTER_Y) - 1;
  draw_circle(CENTER_X, CENTER_Y, RADIUS);
}

void do_circle(Args) {
  g_idle_fn = circle_idle;
}

void cross_idle() {
  // Triangle wave in X, sawtooth wave in Y
  constexpr uint8_t MAX_X = DAC::X::RESOLUTION - 1;
  constexpr uint8_t MAX_Y = DAC::Y::RESOLUTION - 1;
  draw_line(0, 0, MAX_X, MAX_Y);
  draw_line(MAX_X, 0, 0, MAX_Y);
}

void do_cross(Args) {
  g_idle_fn = cross_idle;
}

void bounce_idle() {
  // Ball radius and top/bottom inset
  constexpr uint8_t RADIUS = 4;
  constexpr uint8_t INSET = 4;
  constexpr int8_t DX = 5, DY = 3;
  constexpr uint8_t MIN_X = 0;
  constexpr uint8_t MAX_X = DAC::X::RESOLUTION - 1;
  constexpr uint8_t MIN_Y = INSET;
  constexpr uint8_t MAX_Y = DAC::Y::RESOLUTION - INSET - 1;
  // 8.8 bit position and .8 bit velocity
  static uint16_t x = (MIN_X + RADIUS) * 256;
  static uint16_t y = (MIN_Y + RADIUS) * 256;
  static int8_t dx = DX, dy = DY;
  // Draw ball and borders
  draw_circle(x >> 8, y >> 8, RADIUS);
  draw_line(MIN_X, MIN_Y, MAX_X, MIN_Y);
  draw_line(MAX_X, MIN_Y, MAX_X, MAX_Y);
  draw_line(MAX_X, MAX_Y, MIN_X, MAX_Y);
  draw_line(MIN_X, MAX_Y, MIN_X, MIN_Y);
  // Bounce ball off borders
  if (x < (MIN_X + RADIUS) * 256 + DX) dx = DX;
  if (x > (MAX_X - RADIUS) * 256 - DX) dx = -DX;
  if (y < (MIN_Y + RADIUS) * 256 + DY) dy = DY;
  if (y > (MAX_Y - RADIUS) * 256 - DY) dy = -DY;
  // Update ball position
  x += dx;
  y += dy;
}

IdleFn init_bounce() {
  return bounce_idle;
}

struct Point { uint8_t x; uint8_t y; };

// Compute random point on circumference
void random_point(Point& point) {
  float angle = radians(random(0, 359));
  point.x = (cos(angle) + 1) * 0.5 * (DAC::X::RESOLUTION - 1);
  point.y = (sin(angle) + 1) * 0.5 * (DAC::Y::RESOLUTION - 1);
}

struct Triangle { Point a; Point b; Point c; };

// Compute random circumscribed triangle
void random_triangle(Triangle& triangle) {
  random_point(triangle.a);
  random_point(triangle.b);
  random_point(triangle.c);
}

constexpr uint8_t MAX_TRIS = 8;
static uint8_t g_num_tris = 0;

void circum_idle() {
  static Triangle buffer[MAX_TRIS];
  // Add new triangle every 1/16 frames
  static uint8_t delay = 0;
  if ((delay++ & 0x0F) == 0) {
    if (g_num_tris < MAX_TRIS) {
      ++g_num_tris;
    } else {
      memmove(buffer, buffer + 1, (MAX_TRIS - 1) * sizeof(Triangle));
    }
    random_triangle(buffer[g_num_tris - 1]);
  }
  // Trace triangles
  for (uint8_t i = 0; i < g_num_tris; ++i) {
    Triangle& tri = buffer[i];
    // Repeat recently inserted triangles so they appear brighter
    for (uint8_t j = 0; j <= i; ++j) {
      draw_line(tri.a.x, tri.a.y, tri.b.x, tri.b.y);
      draw_line(tri.b.x, tri.b.y, tri.c.x, tri.c.y);
      draw_line(tri.c.x, tri.c.y, tri.a.x, tri.a.y);
    }
  }
}

IdleFn init_circum() {
  g_num_tris = 0;
  return circum_idle;
}

// Compute sine LUT at compile time, stored in Flash memory
static constexpr uint8_t RESOLUTION = util::min(DAC::X::RESOLUTION, DAC::Y::RESOLUTION);
#define SINE_STEP(i) uint8_t((sin(i * M_PI / 8) + 1.f) * 0.5f * (RESOLUTION - 1))
static uint8_t const SINE_LUT[16] PROGMEM = {
  SINE_STEP(0),  SINE_STEP(1),  SINE_STEP(2),  SINE_STEP(3),
  SINE_STEP(4),  SINE_STEP(5),  SINE_STEP(6),  SINE_STEP(7),
  SINE_STEP(8),  SINE_STEP(9),  SINE_STEP(10), SINE_STEP(11),
  SINE_STEP(12), SINE_STEP(13), SINE_STEP(14), SINE_STEP(15),
};
#undef SINE_STEP

// Approximate sine with LUT and lerp
// Could be optimized, but way faster than float
uint8_t uint_sine(uint8_t x) {
  uint8_t i_a = (x >> 4) & 15;
  uint8_t i_b = (i_a + 1) & 15;
  uint8_t a = pgm_read_byte(&SINE_LUT[i_a]);
  uint8_t b = pgm_read_byte(&SINE_LUT[i_b]);
  uint8_t t = x & 15;
  return (a * (16 - t) + b * t) >> 4;
}

uint8_t g_lj_ax, g_lj_ay;
uint8_t g_lj_dx, g_lj_dy;
uint8_t g_lj_delay;

void lissajous_idle() {
  static uint8_t x0 = 0, y0 = 0;
  uint8_t x = uint_sine(g_lj_ax);
  uint8_t y = uint_sine(g_lj_ay);
  draw_line(x0, y0, x, y);
  g_lj_ax += g_lj_dx;
  g_lj_ay += g_lj_dy;
  x0 = x;
  y0 = y;

  // Optionally animate the phase difference over time
  if (g_lj_delay) {
    static unsigned long last_millis = millis();
    unsigned long now_millis = millis();
    if (now_millis - last_millis > g_lj_delay) {
      last_millis = now_millis;
      g_lj_ax += 1;
    }
  }
}

void custom_lissajous(Args args) {
  g_lj_dx = args.has_next() ? atoi(args.next()) : 1;
  g_lj_dy = args.has_next() ? atoi(args.next()) : 1;
  g_lj_ax = args.has_next() ? atoi(args.next()) : 64; // Phase, 64 = pi/2
  g_lj_ay = 0;
  g_lj_delay = args.has_next() ? atoi(args.next()) : 0;
  g_idle_fn = lissajous_idle;
}

IdleFn init_lj_11() {
  g_lj_dx = 1;
  g_lj_dy = 1;
  g_lj_delay = 10;
  return lissajous_idle;
}

IdleFn init_lj_12() {
  g_lj_dx = 1;
  g_lj_dy = 2;
  g_lj_delay = 25;
  return lissajous_idle;
}

IdleFn init_lj_56() {
  g_lj_dx = 5;
  g_lj_dy = 6;
  g_lj_delay = 35;
  return lissajous_idle;
}
