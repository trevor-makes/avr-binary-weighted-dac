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

static uint8_t g_mode = 0;
static uint16_t g_delay = 0;

void attract_idle() {
  struct InitFn { IdleFn(*fn)(); uint16_t frames; };
  static const InitFn init_fns[] = {
    { init_logo, 4000 },
    { init_maze, 4000 },
    { init_doge, 2000 },
    { init_bounce, 50000 },
    { init_pepe, 2000 },
    { init_circum, 5000 },
  };
  static const uint8_t N_INIT_FNS = sizeof(init_fns) / sizeof(InitFn);

  static IdleFn delegate;
  if (g_delay-- == 0) {
    const InitFn& next = init_fns[g_mode];
    g_mode = (g_mode + 1) % N_INIT_FNS;
    delegate = next.fn();
    g_delay = next.frames;
  }
  delegate();
}

IdleFn init_attract() {
  g_mode = 0;
  g_delay = 0;
  return attract_idle;
}
