// Copyright (c) 2022 Trevor Makes

#include <Arduino.h>

#include "main.hpp"

extern const uint8_t CHAR_ROM[] PROGMEM;

constexpr uint8_t SCREEN_COLS = 8;
constexpr uint8_t SCREEN_ROWS = 4;
constexpr uint8_t ROWS_PER_CHAR = 8;
constexpr uint8_t PIXELS_PER_ROW = 2;
constexpr char FIRST_CHAR = ' ';
constexpr char LAST_CHAR = 'z';

void draw_string(uint8_t x, uint8_t y, const char* str) {
  for (uint8_t col = 0; col < SCREEN_COLS; ++col) {
    // Read ASCII code for current character
    char c = str[col];

    // End early if we find end of string
    if (c == '\0')
      break;

    // Skip non-printable characters
    if (c < FIRST_CHAR || c > LAST_CHAR)
      continue;

    // Trace each character fully before advancing to next character
    uint16_t offset = (c - FIRST_CHAR) * ROWS_PER_CHAR;
    for (uint8_t row = 0; row < ROWS_PER_CHAR; ++row) {
      // Look-up scan data for character at current row
      uint8_t char_data = pgm_read_byte(&CHAR_ROM[offset + row]);

      uint8_t char_x = x + col * BITS_PER_BYTE;
      uint8_t char_y = (y + row) * PIXELS_PER_ROW;

      // Repeat each row to double pixels vertically
      for (uint8_t row_pixel = 0; row_pixel < PIXELS_PER_ROW; ++row_pixel) {
        write_bits(char_x, char_y + row_pixel, char_data);
      }
    }
  }
}

// Character buffer for text messages
char SCREEN_RAM[SCREEN_ROWS][SCREEN_COLS];

// Draw each row of screen buffer
void draw_screen() {
  for (uint8_t row = 0; row < SCREEN_ROWS; ++row) {
    draw_string(0, row * ROWS_PER_CHAR, SCREEN_RAM[row]);
  }
}

// Clear each row of screen buffer
void clear_screen(StreamEx&, Tokens) {
  for (uint8_t row = 0; row < SCREEN_ROWS; ++row) {
    SCREEN_RAM[row][0] = '\0';
  }
  idle_fn = draw_screen;
}

// Copy logo to screen buffer
void init_logo(StreamEx&, Tokens) {
  strncpy(SCREEN_RAM[0], "````````", SCREEN_COLS);
  strncpy(SCREEN_RAM[1], "Trevor  ", SCREEN_COLS);
  strncpy(SCREEN_RAM[2], "  Makes!", SCREEN_COLS);
  strncpy(SCREEN_RAM[3], "````````", SCREEN_COLS);
  idle_fn = draw_screen;
}

// Scroll screen buffer and print message to bottom row
void print_message(StreamEx&, Tokens args) {
  const char* message = args.next();

  // Copy line [1] to [0], line [2] to [1], and so on to scroll up
  for (uint8_t row = 1; row < SCREEN_ROWS; ++row) {
    strncpy(SCREEN_RAM[row - 1], SCREEN_RAM[row], SCREEN_COLS);
  }

  // Copy message into now vacant line at bottom
  strncpy(SCREEN_RAM[SCREEN_ROWS - 1], message, SCREEN_COLS);

  // Set idle function to draw screen buffer
  idle_fn = draw_screen;
}

// Commodore 64 font extracted from VICE
// The 8x8 character data is stored in read-only program memory to save RAM.
// The characters are in ASCII order starting at 0x20 (space), with each
// character occupying 8 bytes. Each byte represents a row of pixels and each
// bit represents a pixel in that row, as in the character 'A':
// ...##... 0x18
// ..####.. 0x3C
// .##..##. 0x66
// .######. 0x7E
// .##..##. 0x66
// .##..##. 0x66
// .##..##. 0x66
// ........ 0x00
const uint8_t CHAR_ROM[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // space
  0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00, // !
  0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, // "
  0x66, 0x66, 0xFF, 0x66, 0xFF, 0x66, 0x66, 0x00, // #
  0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00, // $
  0x62, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x46, 0x00, // %
  0x3C, 0x66, 0x3C, 0x38, 0x67, 0x66, 0x3F, 0x00, // &
  0x06, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, // '
  0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00, // (
  0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00, // )
  0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, // *
  0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, // +
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, // ,
  0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, // -
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, // .
  0x00, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00, // /
  0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00, // 0
  0x18, 0x18, 0x38, 0x18, 0x18, 0x18, 0x7E, 0x00, // 1
  0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E, 0x00, // 2
  0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00, // 3
  0x06, 0x0E, 0x1E, 0x66, 0x7F, 0x06, 0x06, 0x00, // 4
  0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00, // 5
  0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00, // 6
  0x7E, 0x66, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00, // 7
  0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00, // 8
  0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00, // 9
  0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, // :
  0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30, // ;
  0x0E, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0E, 0x00, // <
  0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, // =
  0x70, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x70, 0x00, // >
  0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00, // ?
  0x3C, 0x66, 0x6E, 0x6E, 0x60, 0x62, 0x3C, 0x00, // @
  0x18, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, // A
  0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00, // B
  0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00, // C
  0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00, // D
  0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7E, 0x00, // E
  0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x00, // F
  0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00, // G
  0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, // H
  0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, // I
  0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x66, 0x00, // J
  0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00, // K
  0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00, // L
  0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00, // M
  0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00, // N
  0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, // O
  0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00, // P
  0x3C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0E, 0x00, // Q
  0x7C, 0x66, 0x66, 0x7C, 0x78, 0x6C, 0x66, 0x00, // R
  0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00, // S
  0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, // T
  0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, // U
  0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, // V
  0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00, // W
  0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00, // X
  0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00, // Y
  0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00, // Z
  0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00, // [
  0x0C, 0x12, 0x30, 0x7C, 0x30, 0x62, 0xFC, 0x00, // \ (lira/pound)
  0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00, // ]
  0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, // ^ (up arrow)
  0x00, 0x10, 0x30, 0x7F, 0x7F, 0x30, 0x10, 0x00, // _ (left arrow)
  0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, // ` (bold -)
  0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, // a
  0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C, 0x00, // b
  0x00, 0x00, 0x3C, 0x60, 0x60, 0x60, 0x3C, 0x00, // c
  0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E, 0x00, // d
  0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, // e
  0x00, 0x0E, 0x18, 0x3E, 0x18, 0x18, 0x18, 0x00, // f
  0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x7C, // g
  0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x00, // h
  0x00, 0x18, 0x00, 0x38, 0x18, 0x18, 0x3C, 0x00, // i
  0x00, 0x06, 0x00, 0x06, 0x06, 0x06, 0x06, 0x3C, // j
  0x00, 0x60, 0x60, 0x6C, 0x78, 0x6C, 0x66, 0x00, // k
  0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, // l
  0x00, 0x00, 0x66, 0x7F, 0x7F, 0x6B, 0x63, 0x00, // m
  0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00, // n
  0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00, // o
  0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, // p
  0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06, // q
  0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00, // r
  0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00, // s
  0x00, 0x18, 0x7E, 0x18, 0x18, 0x18, 0x0E, 0x00, // t
  0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, // u
  0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, // v
  0x00, 0x00, 0x63, 0x6B, 0x7F, 0x3E, 0x36, 0x00, // w
  0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00, // x
  0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x0C, 0x78, // y
  0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00, // z
  // ASCII characters {|}~ are not present
};
