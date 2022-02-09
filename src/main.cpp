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

constexpr uint8_t BITS_PER_BYTE = 8;

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
    }
  }
}

constexpr uint8_t SCREEN_COLS = 8;
constexpr uint8_t SCREEN_ROWS = 4;
constexpr uint8_t ROWS_PER_CHAR = 8;
constexpr uint8_t PIX_PER_ROW = 2;
constexpr char FIRST_CHAR = ' ';
constexpr char LAST_CHAR = 'z';

// Print string at (X, Y).
// This needs to be called in a loop to persist on oscilliscope display.
void print(uint8_t x, uint8_t y, const char* str) {
  for (uint8_t row = 0; row < ROWS_PER_CHAR; ++row) {
    // Repeat each row twice (double pixels vertically)
    for (uint8_t row_pix = 0; row_pix < PIX_PER_ROW; ++row_pix) {
      for (uint8_t col_byte = 0; col_byte < SCREEN_COLS; ++col_byte) {
        // Read ASCII code for current character
        char c = str[col_byte];

        // End row early if we find end of string (null terminator)
        if (c == '\0')
          break;

        // Skip non-printable characters
        if (c < FIRST_CHAR || c > LAST_CHAR)
          continue;

        // Look-up scanline for character at current row
        uint16_t scan_index = (c - FIRST_CHAR) * ROWS_PER_CHAR + row;
        uint8_t scan_data = pgm_read_byte(&CHAR_ROM[scan_index]);

        // Write X, Y for set bits
        uint8_t scan_x = x + col_byte * BITS_PER_BYTE;
        uint8_t scan_y = (y + row) * PIX_PER_ROW + row_pix;
        write_bits(scan_x, scan_y, scan_data);
      }
    }
  }
}

// Character buffer for text messages
char SCREEN_RAM[SCREEN_ROWS][SCREEN_COLS];

// Write screen buffer to display
void print_screen() {
  for (uint8_t row = 0; row < SCREEN_ROWS; ++row) {
    print(0, row * ROWS_PER_CHAR, SCREEN_RAM[row]);
  }
}

// Function to call while waiting for serial input
uCLI::IdleFn idle_fn = print_screen;

// Clear each row of screen buffer
void clear_screen(uCLI::Args) {
  for (uint8_t row = 0; row < SCREEN_ROWS; ++row) {
    SCREEN_RAM[row][0] = '\0';
  }
  idle_fn = print_screen;
}

// Copy logo to screen buffer
void init_logo(uCLI::Args) {
  strncpy(SCREEN_RAM[0], "````````", SCREEN_COLS);
  strncpy(SCREEN_RAM[1], "Trevor  ", SCREEN_COLS);
  strncpy(SCREEN_RAM[2], "  Makes!", SCREEN_COLS);
  strncpy(SCREEN_RAM[3], "````````", SCREEN_COLS);
  idle_fn = print_screen;
}

// Scroll screen buffer and add message at bottom
void write_msg(uCLI::Args args) {
  char* message = args.remainder();
  // Copy line [1] to [0], line [2] to [1], and so on to scroll up
  for (uint8_t row = 1; row < SCREEN_ROWS; ++row) {
    strncpy(SCREEN_RAM[row - 1], SCREEN_RAM[row], SCREEN_COLS);
  }
  // Copy message into now vacant line at bottom
  strncpy(SCREEN_RAM[SCREEN_ROWS - 1], message, SCREEN_COLS);
  idle_fn = print_screen;
}

constexpr uint8_t SIN_STEPS = 128;
constexpr double RAD_PER_STEP = 2. * PI / double(SIN_STEPS);

// Lookup table for cached sine values
uint8_t SINE_TABLE[SIN_STEPS];

// Output sine and cosine to X and Y channels
void draw_circle() {
  for (uint8_t i = 0; i < SIN_STEPS; ++i) {
    // Shift phase of second channel by 90° (steps/4) so we have sine and cosine
    PortB::write(SINE_TABLE[i]);
    PortC::write(SINE_TABLE[(i + (SIN_STEPS / 4)) % SIN_STEPS]);
  }
}

// Start drawing circle in idle loop
void init_circle(uCLI::Args) {
  // Compute sine lookup table
  for (uint8_t i = 0; i < SIN_STEPS; ++i) {
    // Domain: 0 to 2π, Range: 0 to 62
    SINE_TABLE[i] = uint8_t(31. * (1. + sin(i * RAD_PER_STEP)) + 0.5);
  }

  // Switch idle function to draw circle
  idle_fn = draw_circle;
}

constexpr uint8_t BITMAP_ROWS = 64;
constexpr uint8_t BITMAP_COLS = 8;

const uint8_t* bitmap_ptr;

void draw_bitmap() {
  for (uint8_t row = 0; row < BITMAP_ROWS; ++row) {
    for (uint8_t col_byte = 0; col_byte < BITMAP_COLS; ++col_byte) {
      // Read byte at current bitmap index
      uint16_t bitmap_index = row * BITMAP_COLS + col_byte;
      uint8_t scan_data = pgm_read_byte(&bitmap_ptr[bitmap_index]);

      // Write X, Y for set bits
      uint8_t col = col_byte * BITS_PER_BYTE;
      write_bits(col, row, scan_data);
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
