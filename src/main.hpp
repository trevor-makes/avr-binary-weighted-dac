// Copyright (c) 2022 Trevor Makes

#pragma once

#include "uIO.hpp"
#include "uCLI.hpp"

#include <Arduino.h>

using uCLI::StreamEx;
using uCLI::IdleFn;
using uCLI::Tokens;
using uCLI::CLI;

extern IdleFn idle_fn;
extern StreamEx serialEx;

void init_sines(Tokens);

void clear_screen(Tokens);
void init_logo(Tokens);
void print_message(Tokens);

void init_doge(Tokens);
void init_pepe(Tokens);

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
  // For ATmega 328p boards, the two highest bits of ports B and C are
  // unavailable, so we set a mask to prevent writing to these bits.
  // This allows values from 0 to 63 to be written to each port.
  using PortB = uIO::PortB::Mask<0x3F>;
  using PortC = uIO::PortC::Mask<0x3F>;
#else
  #error The I/O port mapping has not been defined for the target platform
#endif

// Option to invert axis in case oscilloscope does not support this
constexpr bool INVERT_X = true;
constexpr bool INVERT_Y = true;

inline void init_ports() {
  PortB::config_output();
  PortC::config_output();
}

inline void write_x(uint8_t x) {
  if (INVERT_X) {
    PortB::write(63 - x);
  } else {
    PortB::write(x);
  }
}

inline void write_y(uint8_t y) {
  if (INVERT_Y) {
    PortC::write(63 - y);
  } else {
    PortC::write(y);
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
