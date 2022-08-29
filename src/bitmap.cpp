// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

extern const uint8_t DOGE_ROM[] PROGMEM;
extern const uint8_t PEPE_ROM[] PROGMEM;

constexpr uint8_t BITMAP_ROWS = 64;
constexpr uint8_t BITMAP_COLS = 8;

const uint8_t* bitmap_ptr;

// Trace set bitmap pixels with X and Y
void draw_bitmap() {
  for (uint8_t row = 0; row < BITMAP_ROWS; ++row) {
    for (uint8_t col_byte = 0; col_byte < BITMAP_COLS; ++col_byte) {
      // Read byte at current bitmap index
      uint16_t bitmap_index = row * BITMAP_COLS + col_byte;
      uint8_t scan_data = pgm_read_byte(&bitmap_ptr[bitmap_index]);

      // Write X, Y for set bits
      uint8_t col_bit = col_byte * BITS_PER_BYTE;
      write_bits(col_bit, row, scan_data);
    }
  }
}

// Start drawing Doge bitmap in idle loop
void init_doge(Args) {
  bitmap_ptr = DOGE_ROM;
  idle_fn = draw_bitmap;
}

// Start drawing Pepe bitmap in idle loop
void init_pepe(Args) {
  bitmap_ptr = PEPE_ROM;
  idle_fn = draw_bitmap;
}

// 64x64 1-bit Doge bitmap
const uint8_t DOGE_ROM[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x18, 0x00,
  0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0x7C, 0x00,
  0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0xD4, 0x00,
  0x00, 0x00, 0x3D, 0x80, 0x00, 0x01, 0xAA, 0x00,
  0x00, 0x00, 0x1E, 0x80, 0x00, 0x03, 0xEA, 0x00,
  0x00, 0x00, 0x1B, 0xC0, 0x00, 0x0E, 0xB6, 0x00,
  0x00, 0x00, 0x3D, 0x40, 0x00, 0x0F, 0xAA, 0x00,
  0x00, 0x00, 0x3A, 0xF0, 0x00, 0x3A, 0xD5, 0x00,
  0x00, 0x00, 0x3F, 0xFF, 0x64, 0x3A, 0xAB, 0x00,
  0x00, 0x00, 0x2F, 0xFF, 0xFF, 0xD6, 0xAB, 0x00,
  0x00, 0x00, 0x7F, 0xFF, 0xFF, 0x75, 0x55, 0x00,
  0x00, 0x00, 0xFF, 0xEF, 0xFE, 0x95, 0x15, 0x00,
  0x00, 0x01, 0xFF, 0xEB, 0xFF, 0xD6, 0x8B, 0x00,
  0x00, 0x03, 0xFF, 0xBF, 0xFF, 0xAA, 0x36, 0x00,
  0x00, 0x0F, 0xFF, 0xAA, 0xDF, 0xD5, 0x16, 0x00,
  0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xEA, 0x5A, 0x00,
  0x00, 0x3F, 0xFF, 0x5F, 0xF7, 0xF4, 0x2E, 0x00,
  0x00, 0x7F, 0xFF, 0xFF, 0xDF, 0xFA, 0xD2, 0x00,
  0x00, 0x7F, 0xFF, 0x5F, 0xEB, 0xFF, 0x5E, 0x00,
  0x00, 0xFC, 0x3D, 0xBF, 0xFF, 0xFE, 0xEB, 0x00,
  0x01, 0xFF, 0x1F, 0xBF, 0xF5, 0xFF, 0xAB, 0x80,
  0x01, 0xFC, 0x7F, 0xFF, 0xFE, 0xFF, 0xD5, 0x80,
  0x01, 0xFE, 0x1F, 0xFE, 0x2F, 0xFF, 0xF5, 0x80,
  0x03, 0xFA, 0x7F, 0xF5, 0x97, 0xFF, 0xEB, 0x80,
  0x03, 0xFF, 0x7F, 0xD1, 0x03, 0xFF, 0xF5, 0xC0,
  0x03, 0xFF, 0xFF, 0xD2, 0x0F, 0xFF, 0xFB, 0xC0,
  0x07, 0xFF, 0xFF, 0xD9, 0x17, 0xFF, 0xFD, 0xC0,
  0x07, 0xFF, 0xFF, 0xEA, 0xBF, 0xFF, 0xFB, 0xC0,
  0x07, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFB, 0xC0,
  0x07, 0xF5, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0,
  0x07, 0xD5, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0,
  0x0F, 0xC0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
  0x0F, 0xC0, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
  0x07, 0xC0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
  0x0F, 0xC0, 0x0F, 0xFF, 0xFF, 0xFF, 0xFB, 0xF0,
  0x0F, 0xC0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFD, 0xF0,
  0x0F, 0xE0, 0xAF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF0,
  0x0F, 0x90, 0x5F, 0xFF, 0xFF, 0xFF, 0xF7, 0xF0,
  0x0F, 0xAA, 0x2B, 0xFF, 0xFF, 0xFF, 0xF5, 0x78,
  0x0F, 0xC1, 0x5E, 0xFF, 0xFF, 0xFF, 0xEF, 0xF8,
  0x07, 0xE0, 0x2B, 0xD7, 0xFF, 0xFF, 0xFA, 0xBC,
  0x0F, 0xD0, 0x1E, 0xAF, 0xFF, 0xFF, 0xEF, 0xF8,
  0x07, 0xE8, 0x04, 0x17, 0xFF, 0xFF, 0xF5, 0x7C,
  0x07, 0xF5, 0x42, 0xDF, 0xFF, 0xFF, 0xEF, 0xBC,
  0x07, 0xFD, 0x75, 0x5F, 0xFF, 0xFF, 0xFA, 0xDE,
  0x03, 0xFA, 0xAE, 0xFF, 0xFF, 0xFF, 0xAD, 0x7E,
  0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xAE,
  0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5A, 0xFE,
  0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDA, 0xAE,
  0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xED, 0xFE,
  0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xB5, 0x5E,
  0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDB, 0xFE,
  0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x6D, 0xFE,
  0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xDB, 0xFE,
  0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFE,
  0x01, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFE,
  0x03, 0xFF, 0xFF, 0x55, 0xBF, 0xFF, 0xBF, 0xFE,
  0x03, 0xFF, 0xAA, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE,
  0x03, 0xFF, 0xFF, 0x57, 0xFF, 0xFF, 0xFF, 0xFE,
  0x03, 0xFF, 0xEB, 0xBF, 0xFF, 0xFF, 0xFF, 0xFE,
  0x03, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
};

// 64x64 1-bit Pepe bitmap
const uint8_t PEPE_ROM[] PROGMEM = {    
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x7F, 0xF8, 0x00, 0x40, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0xFE, 0x05, 0xF8, 0x00, 0x00,
  0x00, 0x01, 0xFF, 0xD6, 0x0F, 0xFE, 0x00, 0x00,
  0x00, 0x03, 0xFC, 0x00, 0x1F, 0xFC, 0x00, 0x00,
  0x00, 0x07, 0xE0, 0xFF, 0x0F, 0xE0, 0x00, 0x00,
  0x00, 0x0F, 0x8F, 0xFF, 0xF1, 0xFF, 0xF0, 0x00,
  0x00, 0x1F, 0x3F, 0xFF, 0x00, 0xFF, 0xF8, 0x00,
  0x00, 0x3F, 0xFF, 0x80, 0xFC, 0x3E, 0x00, 0x00,
  0x00, 0x3F, 0xFE, 0x00, 0x7E, 0x00, 0xFF, 0xF0,
  0x00, 0x7F, 0xF8, 0x07, 0xFF, 0x00, 0x1F, 0x98,
  0x00, 0x7F, 0xF8, 0x3F, 0xFF, 0x87, 0xFF, 0xF0,
  0x00, 0x7F, 0xF0, 0xFF, 0x87, 0xCF, 0xF8, 0xF8,
  0x00, 0x7F, 0xCF, 0xFF, 0x03, 0xEF, 0xF0, 0x7C,
  0x00, 0xFF, 0xDF, 0xFE, 0x03, 0xEF, 0xE0, 0x3C,
  0x00, 0xFF, 0xCF, 0xFE, 0x01, 0xE7, 0xE0, 0x3C,
  0x00, 0xFF, 0xC7, 0xFE, 0x31, 0xE7, 0xE0, 0x3C,
  0x00, 0xFF, 0xF3, 0xFC, 0x39, 0xE7, 0xE3, 0x3C,
  0x03, 0xFF, 0xF9, 0xFE, 0x31, 0xE7, 0xE2, 0x3C,
  0x07, 0xFF, 0xF8, 0xFE, 0x03, 0xE7, 0xF0, 0x7C,
  0x0F, 0xFF, 0xF8, 0x7F, 0x07, 0xE1, 0xFF, 0xFC,
  0x0F, 0xFF, 0xFC, 0x1F, 0xFF, 0xC4, 0x7F, 0xF8,
  0x1F, 0xFF, 0xFE, 0x07, 0xFF, 0x83, 0x03, 0xC0,
  0x1F, 0xFF, 0xFF, 0x00, 0x7C, 0x19, 0x0F, 0xF0,
  0x3F, 0xFF, 0xFF, 0xE3, 0xFE, 0x3C, 0xF0, 0x00,
  0x3F, 0xFF, 0xF1, 0xF8, 0x50, 0x7E, 0x7C, 0x00,
  0x3F, 0xFF, 0xF8, 0x3F, 0x01, 0xFE, 0x00, 0x00,
  0x3F, 0xFF, 0xFD, 0x02, 0x03, 0xFF, 0x05, 0x40,
  0x3F, 0xFF, 0xFF, 0x80, 0x5F, 0xFF, 0x9F, 0xE0,
  0x3F, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xE0,
  0x3F, 0xEE, 0x3F, 0xFF, 0xFF, 0xFF, 0xDF, 0xF0,
  0x3F, 0xC8, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0x3F, 0xD9, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0x3F, 0xF3, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xC8,
  0x3F, 0xF3, 0xFF, 0x81, 0xFF, 0xFF, 0xFF, 0x3C,
  0x3F, 0xF3, 0x87, 0xF8, 0x0F, 0xFF, 0xE0, 0xF8,
  0x3F, 0xF9, 0x80, 0xFF, 0x80, 0x00, 0x07, 0xC0,
  0x3F, 0xFC, 0xC4, 0x1F, 0xFF, 0xFC, 0x00, 0x00,
  0x3F, 0xFC, 0xC7, 0x40, 0xFF, 0xFE, 0x00, 0x00,
  0x3F, 0xFF, 0x71, 0xE0, 0x00, 0x00, 0x70, 0x80,
  0x3F, 0xFF, 0x30, 0x60, 0x00, 0x01, 0xF9, 0xC0,
  0x1F, 0xFF, 0x9C, 0x00, 0x1F, 0xA1, 0xFC, 0xE0,
  0x1F, 0xFF, 0xCF, 0x0C, 0x0F, 0xF9, 0xFE, 0x60,
  0x0F, 0xFF, 0xE3, 0x9F, 0x83, 0xF8, 0xFF, 0x30,
  0x07, 0xFF, 0xF1, 0xE7, 0xE1, 0xF8, 0xFF, 0x30,
  0x03, 0xFF, 0xFC, 0xF0, 0xF8, 0x51, 0xFE, 0x70,
  0x01, 0xFF, 0xFE, 0x3C, 0x1C, 0x03, 0xF0, 0xE0,
  0x00, 0x7F, 0xFF, 0x87, 0x80, 0x07, 0x03, 0xC0,
  0x00, 0x3F, 0xFF, 0xC1, 0xFF, 0xFE, 0x07, 0x00,
  0x00, 0x0F, 0xFF, 0xF0, 0x7F, 0xF8, 0x0E, 0x00,
  0x00, 0x07, 0xFF, 0xFE, 0x0F, 0x83, 0xFE, 0x00,
  0x00, 0x01, 0xFF, 0xFF, 0xC0, 0x3F, 0xFE, 0x00,
  0x00, 0x00, 0x7F, 0xFF, 0xFB, 0x7F, 0xF8, 0x00,
  0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xE0, 0x00,
  0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0x80, 0x00,
  0x00, 0x00, 0x01, 0xFF, 0xFF, 0xF0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
