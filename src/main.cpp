// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

IdleFn g_idle_fn = nullptr; // Function to call while waiting for serial input
StreamEx g_serial_ex(Serial); // Stream wrapper that supports ANSI escape codes
CLI g_serial_cli(g_serial_ex); // Command line interface manager

IdleFn init_attract();

void setup() {
  // Configure I/O ports for output
  DAC::config();

  // Begin in attract mode
  g_idle_fn = init_attract();

  // Establish serial connection with computer
  Serial.begin(9600);
  while (!Serial) {}
}

template <IdleFn (*Fn)()>
void DoIdle(Args) {
  g_idle_fn = Fn();
}

void loop() {
  static const Command commands[] = {
    { "attract", DoIdle<init_attract> },
    // text.cpp
    { "logo", DoIdle<init_logo> },
    { "clear", clear_screen },
    { "print", print_message },
    { "maze", DoIdle<init_maze> },
    // vector.cpp
    { "circle", do_circle },
    { "cross", do_cross },
    { "bounce", DoIdle<init_bounce> },
    { "circum", DoIdle<init_circum> },
    // bitmap.cpp
    { "doge", DoIdle<init_doge> },
    { "pepe", DoIdle<init_pepe> },
    { "fliph", flip_horizontal },
    { "flipv", flip_vertical },
    { "export", export_bitmap },
    { "import", import_bitmap },
    { "save", save_bitmap },
    { "load", load_bitmap },
    { "delay", set_delay },
  };

  // Prompt for a command from the list while looping over the idle function
  g_serial_cli.run_once(commands, g_idle_fn);
}

static uint8_t g_mode;
static uint16_t g_countdown;
static decltype(millis()) g_lastMillis;

void attract_idle() {
  using InitFn = IdleFn(*)();
  struct Entry { InitFn init_fn; uint16_t delay_ms; };
  static const Entry entries[] = {
    { init_logo, 10000 },
    { init_maze, 20000 },
    { init_doge, 10000 },
    { init_bounce, 20000 },
    { init_pepe, 10000 },
    { init_circum, 20000 },
  };
  static const uint8_t N_ENTRIES = sizeof(entries) / sizeof(Entry);
  static IdleFn delegate = nullptr;

  // Get elapsed time since last frame
  auto nowMillis = millis();
  auto elapsed = nowMillis - g_lastMillis;
  g_lastMillis = nowMillis;

  // Load next mode when countdown reaches zero
  if (elapsed >= g_countdown) {
    const Entry& next = entries[g_mode];
    g_mode = (g_mode + 1) % N_ENTRIES;
    g_countdown = next.delay_ms;
    delegate = next.init_fn();
  } else {
    g_countdown -= elapsed;
  }

  if (delegate) delegate();
}

IdleFn init_attract() {
  g_mode = 0;
  g_countdown = 0;
  g_lastMillis = millis();
  return attract_idle;
}
