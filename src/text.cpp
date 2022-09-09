// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

#include "core/util.hpp"

extern const uint8_t CHAR_ROM[] PROGMEM;

constexpr uint8_t SCREEN_COLS = 8;
constexpr uint8_t SCREEN_ROWS = 8;
constexpr uint8_t COLS_PER_CHAR = 8;
constexpr uint8_t ROWS_PER_CHAR = 8;
constexpr uint8_t BITMAP_ROWS = SCREEN_ROWS * ROWS_PER_CHAR;
constexpr char FIRST_CHAR = '!';
constexpr char LAST_CHAR = '~';

extern uint8_t BITMAP_RAM[];

void draw_string(uint8_t row, const char* str) {
  uint8_t* col_ptr = BITMAP_RAM + row * SCREEN_COLS;
  uint8_t rows = util::min(ROWS_PER_CHAR, uint8_t(BITMAP_ROWS - util::min(BITMAP_ROWS, row)));

  // Clear line
  memset(col_ptr, 0, SCREEN_COLS * rows);

  for (uint8_t col = 0; col < SCREEN_COLS; ++col) {
    // Read ASCII code for current character
    char c = *str++;

    // End early if we find end of string
    if (c == '\0')
      break;

    // Skip non-printable characters
    if (c < FIRST_CHAR || c > LAST_CHAR)
      continue;

    // Trace each character fully before advancing to next character
    uint16_t char_index = (c - FIRST_CHAR) * ROWS_PER_CHAR;
    const uint8_t* char_ptr = &CHAR_ROM[char_index];
    for (uint8_t offset = col; offset < SCREEN_COLS * rows; offset += SCREEN_COLS) {
      col_ptr[offset] = pgm_read_byte(char_ptr++);
    }
  }
}

void clear_bitmap() {
  memset(BITMAP_RAM, 0, SCREEN_COLS * BITMAP_ROWS);
}

// Clear each row of screen buffer
void clear_screen(Args) {
  clear_bitmap();
  g_idle_fn = bitmap_idle;
}

// Copy logo to screen buffer
void init_logo(Args) {
  clear_bitmap();
  draw_string(2 * ROWS_PER_CHAR, "````````");
  draw_string(3 * ROWS_PER_CHAR, "Trevor  ");
  draw_string(4 * ROWS_PER_CHAR, "  Makes!");
  draw_string(5 * ROWS_PER_CHAR, "````````");
  g_idle_fn = bitmap_idle;
}

// Scroll screen buffer and print message to bottom row
void print_message(Args args) {
  const char* message = args.next();

  // Scroll rows up from the bottom
  memmove(BITMAP_RAM, BITMAP_RAM + SCREEN_COLS * ROWS_PER_CHAR, (SCREEN_ROWS - 1) * SCREEN_COLS * ROWS_PER_CHAR);

  // Copy message into now vacant line at bottom
  draw_string((SCREEN_ROWS - 1) * ROWS_PER_CHAR, message);

  // Set idle function to draw screen buffer
  g_idle_fn = bitmap_idle;
}

static uint8_t g_idle_count = 0;
static uint8_t g_scroll_count = 0;

void maze_idle() {
  // Limit scrolling to 1/8 framerate
  if ((g_idle_count++ & 0x07) == 0) {
    static char maze_chars[SCREEN_COLS];
    // Refresh random chars when scrolling a new row
    if (g_scroll_count++ == 0) {
      for (uint8_t i = 0; i < SCREEN_COLS; ++i) {
        maze_chars[i]  = random(2) ? '/' : '\\';
      }
    }
    // Scroll line up by one pixel
    memmove(BITMAP_RAM, BITMAP_RAM + SCREEN_COLS, (BITMAP_ROWS - 1) * SCREEN_COLS);
    draw_string(BITMAP_ROWS - g_scroll_count, maze_chars);
    if (g_scroll_count == ROWS_PER_CHAR) g_scroll_count = 0;
  }
  // Delegate to bitmap idle function
  bitmap_idle();
}

void init_maze(Args) {
  g_idle_count = 0;
  g_scroll_count = 0;
  g_idle_fn = maze_idle;
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
  0x03, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xC0, // ╱
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
  0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03, // ╲
  0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00, // ]
  0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, // ↑ (replaces ^)
  0x00, 0x10, 0x30, 0x7F, 0x7F, 0x30, 0x10, 0x00, // ← (replaces _)
  0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, // ━ (replaces `)
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
  0x1C, 0x30, 0x30, 0x60, 0x30, 0x30, 0x1C, 0x00, // {
  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, // |
  0x38, 0x0C, 0x0C, 0x06, 0x0C, 0x0C, 0x38, 0x00, // }
  0x22, 0x77, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00, // ♥ (replaces ~)
};
