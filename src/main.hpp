// Copyright (c) 2022 Trevor Makes

#pragma once

#include "core/arduino.hpp"
#include "core/io.hpp"
#include "core/cli.hpp"

using core::serial::StreamEx;
using core::cli::IdleFn;
using core::cli::Args;
using core::cli::Command;
using CLI = core::cli::CLI<20>; // Limit line buffer to 20 bytes

extern IdleFn g_idle_fn;
extern StreamEx g_serial_ex;
extern CLI g_serial_cli;

void bitmap_idle();
void set_delay(Args);

void do_circle(Args);
void do_cross(Args);
IdleFn init_bounce();
IdleFn init_circum();
void custom_lissajous(Args);
IdleFn init_lj_11();
IdleFn init_lj_12();
IdleFn init_lj_56();

void clear_screen(Args);
void print_message(Args);
IdleFn init_logo();
IdleFn init_maze();

IdleFn init_doge();
IdleFn init_pepe();
void flip_vertical(Args);
void flip_horizontal(Args);
void export_bitmap(Args);
void import_bitmap(Args);
void save_bitmap(Args);
void load_bitmap(Args);

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
  // For Uno/Nano boards, the two highest bits of ports B and C are unavailable
  // (used for oscillator and reset). The upper bits could be masked off using
  // `::Mask<0x3F>`, but not doing so significantly speeds up vector drawing.
  // This leaves us with 6 bits in each, values 0 to 63.
  //         x |         | C5 -  1RΩ
  //         x |         | C4 -  2RΩ
  //         x |         | C3 -  4RΩ
  //         x | Arduino | C2 -  8RΩ
  // 32RΩ - B0 |   NANO  | C1 - 16RΩ
  // 16RΩ - B1 |         | C0 - 32RΩ
  //  8RΩ - B2 |   ___   | x
  //  4RΩ - B3 |  |USB|  | x
  //  2RΩ - B4 |__|___|__| B5 -  1RΩ
  struct DAC {
  protected:
    CORE_PORT(B)
    CORE_PORT(C)

  public:
    struct X : public PortB {
      static constexpr uint8_t RESOLUTION = 64;
    };

    struct Y : public PortC {
      static constexpr uint8_t RESOLUTION = 64;
    };

    static void config() {
      X::config_output();
      Y::config_output();
    }
  };
#else
  #error The I/O port mapping has not been defined for the target platform
#endif
