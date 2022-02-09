// Copyright (c) 2022 Trevor Makes

#include "uIO.hpp"
#include "uCLI.hpp"

#include <Arduino.h>

#include "rom_data.hpp"

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

// Print string at (X, Y).
// This needs to be called in a loop to persist on oscilliscope display.
void print(uint8_t x, uint8_t y, const char* str) {
  for (uint8_t row = 0; row < 8; ++row) {
    // Repeat each row twice (double pixels vertically)
    for (uint8_t dup = 0; dup < 2; ++dup) {
      for (uint8_t col_byte = 0; col_byte < 8; ++col_byte) {
        // Read ASCII code for current character
        char c = str[col_byte];

        // End row early if we find end of string (null terminator)
        if (c == '\0')
          break;

        // Skip non-printable characters
        if (c < ' ' || c > 'z')
          continue;

        // Look-up scanline for character at current row
        uint8_t scan_byte = pgm_read_byte(&CHAR_ROM[(c - 0x20) * 8 + row]);

        // Skip blank scanlines
        if (scan_byte == 0)
          continue;

        // Write Y only if we find a non-blank scanline
        write_y((y + row) * 2 + dup);

        // Write X for each set pixel
        for (uint8_t col_bit = 0; col_bit < 8; ++col_bit, scan_byte <<= 1) {
          if (scan_byte >= 0x80) {
            write_x(x + col_byte * 8 + col_bit);
          }
        }
      }
    }
  }
}

// Character buffer for text messages
char SCREEN_RAM[4][8];

// Write screen buffer to display
void print_screen() {
  for (uint8_t row = 0; row < 4; ++row) {
    print(0, row*8, SCREEN_RAM[row]);
  }
}

// Function to call while waiting for serial input
uCLI::IdleFn idle_fn = print_screen;

// Clear each row of screen buffer
void clear_screen(uCLI::Args) {
  SCREEN_RAM[0][0] = '\0';
  SCREEN_RAM[1][0] = '\0';
  SCREEN_RAM[2][0] = '\0';
  SCREEN_RAM[3][0] = '\0';
  idle_fn = print_screen;
}

// Copy logo to screen buffer
void init_logo(uCLI::Args) {
  strncpy(SCREEN_RAM[0], "````````", 8);
  strncpy(SCREEN_RAM[1], "Trevor", 8);
  strncpy(SCREEN_RAM[2], "  Makes!", 8);
  strncpy(SCREEN_RAM[3], "````````", 8);
  idle_fn = print_screen;
}

// Scroll screen buffer and add message at bottom
void write_msg(uCLI::Args args) {
  char* message = args.remainder();
  strncpy(SCREEN_RAM[0], SCREEN_RAM[1], 8);
  strncpy(SCREEN_RAM[1], SCREEN_RAM[2], 8);
  strncpy(SCREEN_RAM[2], SCREEN_RAM[3], 8);
  strncpy(SCREEN_RAM[3], message, 8);
  idle_fn = print_screen;
}

// Lookup table for cached sine values
uint8_t SINE_TABLE[256];

// Output sine and cosine to X and Y channels
void draw_circle() {
  uint8_t i = 0;
  do {
    // Shift phase of second channel by 90° (64/256) so we have sine and cosine
    PortB::write(SINE_TABLE[i]);
    PortC::write(SINE_TABLE[uint8_t(i + 64)]);
  } while (++i != 0); // Loop until i wraps from 255 back to 0
}

// Start drawing circle in idle loop
void init_circle(uCLI::Args) {
  // Compute sine lookup table
  uint8_t i = 0;
  do {
    // Domain: 0 to 2π in 256 steps, Range: 0 to 62
    SINE_TABLE[i] = uint8_t(31.f * (1.f + sin(i * (PI / 128.f))));
  } while (++i != 0); // Loop until i wraps from 255 back to 0

  // Switch idle function to draw circle
  idle_fn = draw_circle;
}

const uint8_t* bitmap_ptr;

void draw_bitmap() {
  for (uint8_t row = 0; row < 64; ++row) {
    for (uint8_t col_byte = 0; col_byte < 8; ++col_byte) {
      // Read next byte of current scanline
      uint8_t scan_byte = pgm_read_byte(&bitmap_ptr[row * 8 + col_byte]);

      // Skip blank scanlines
      if (scan_byte == 0)
        continue;

      // Write Y only if we find a non-blank scanline
      write_y(row);

      // Write X for each set pixel
      for (uint8_t col_bit = 0; col_bit < 8; ++col_bit, scan_byte <<= 1) {
        if (scan_byte >= 0x80) {
          write_x(col_byte * 8 + col_bit);
        }
      }
    }
  }
}

void init_doge(uCLI::Args) {
  bitmap_ptr = DOGE_ROM;
  idle_fn = draw_bitmap;
}

void init_pepe(uCLI::Args) {
  bitmap_ptr = PEPE_ROM;
  idle_fn = draw_bitmap;
}

void setup() {
  PortB::config_output();
  PortC::config_output();

  // Call init_logo with empty uCLI::Args
  init_logo({});

  // Establish serial connection with computer
  Serial.begin(9600);
  while (!Serial) {}
}

void loop() {
  static const uCLI::Command commands[] = {
    { "logo", init_logo },
    { "clear", clear_screen },
    { "write", write_msg },
    { "circle", init_circle },
    { "doge", init_doge },
    { "pepe", init_pepe },
  };

  // Prompt for a command from the list while looping over the idle function
  uCLI::run_command(Serial, commands, idle_fn);
}
