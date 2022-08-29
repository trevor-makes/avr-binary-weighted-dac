// Copyright (c) 2022 Trevor Makes

#include "core/io.hpp"

#include "main.hpp"

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
  CORE_PORT(B)
  CORE_PORT(C)

  // For ATmega 328p boards, the two highest bits of ports B and C are
  // unavailable, so we set a mask to prevent writing to these bits.
  // This allows values from 0 to 63 to be written to each port.
  //         x |         | C5 - RΩ
  //         x |         | C4 - 2RΩ
  //         x |         | C3 - 4RΩ
  //         x | Arduino | C2 - 8RΩ
  // 32RΩ - B0 |   NANO  | C1 - 16RΩ
  // 16RΩ - B1 |         | C0 - 32RΩ
  //  8RΩ - B2 |   ___   | x
  //  4RΩ - B3 |  |USB|  | x
  //  2RΩ - B4 |__|___|__| B5 - RΩ
  using XAxis = PortB::Mask<0x3F>;
  using YAxis = PortC::Mask<0x3F>;
#else
  #error The I/O port mapping has not been defined for the target platform
#endif

void init_ports() {
  XAxis::config_output();
  YAxis::config_output();
}

void write_x(uint8_t x) {
  XAxis::write(x);
}

void write_y(uint8_t y) {
  YAxis::write(y);
}

void write_bits(uint8_t x, uint8_t y, uint8_t bits) {
  // Skip blank scanlines
  if (bits == 0)
    return;

  // Write Y only if we find a non-blank scanline
  write_y(y);

  // Write X for each set bit
  do {
    if (bits >= 0x80) { // if high bit is set...
      write_x(x);
    }
    bits <<= 1;
    ++x;
  } while (bits > 0);
}