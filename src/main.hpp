// Copyright (c) 2022 Trevor Makes

#pragma once

#include "core/io.hpp"
#include "core/cli.hpp"

#include <Arduino.h>

using core::serial::StreamEx;
using core::cli::IdleFn;
using core::cli::Args;
using core::cli::Command;
using core::cli::CLI;

extern IdleFn idle_fn;
extern StreamEx serialEx;

void init_sines(Args);

void clear_screen(Args);
void init_logo(Args);
void print_message(Args);

void init_doge(Args);
void init_pepe(Args);

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

// Option to invert axis in case oscilloscope does not support this
constexpr bool INVERT_X = true;
constexpr bool INVERT_Y = true;

inline void init_ports() {
  XAxis::config_output();
  YAxis::config_output();
}

inline void write_x(uint8_t x) {
  if (INVERT_X) {
    XAxis::write(63 - x);
  } else {
    XAxis::write(x);
  }
}

inline void write_y(uint8_t y) {
  if (INVERT_Y) {
    YAxis::write(63 - y);
  } else {
    YAxis::write(y);
  }
}

constexpr uint8_t BITS_PER_BYTE = 8;
constexpr uint16_t US_PER_PIXEL = 5;

inline void write_bits(uint8_t x, uint8_t y, uint8_t bits) {
  // Skip blank scanlines
  if (bits == 0)
    return;

  // Write Y only if we find a non-blank scanline
  write_y(y);

  // Write X for each set bit
  for (uint8_t i = 0; i < BITS_PER_BYTE; ++i, bits <<= 1) {
    if (bits >= 0x80) { // if high bit is set...
      write_x(x + i);

      // Hold position briefly to normalize the intensity of each pixel
      delayMicroseconds(US_PER_PIXEL);
    }
  }
}
