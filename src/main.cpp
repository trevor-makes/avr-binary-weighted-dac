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
    { F("attract"), DoIdle<init_attract> },
    // text.cpp
    { F("logo"), DoIdle<init_logo> },
    { F("clear"), clear_screen },
    { F("print"), print_message },
    { F("maze"), DoIdle<init_maze> },
    // vector.cpp
    { F("circle"), do_circle },
    { F("cross"), do_cross },
    { F("bounce"), DoIdle<init_bounce> },
    { F("circum"), DoIdle<init_circum> },
    { F("lissajous"), custom_lissajous },
    // bitmap.cpp
    { F("doge"), DoIdle<init_doge> },
    { F("pepe"), DoIdle<init_pepe> },
    { F("reee"), DoIdle<init_reee> },
    { F("wojak"), DoIdle<init_wojak> },
    { F("fliph"), flip_horizontal },
    { F("flipv"), flip_vertical },
    { F("export"), export_bitmap },
    { F("import"), import_bitmap },
    { F("save"), save_bitmap },
    { F("load"), load_bitmap },
    { F("delay"), set_delay },
  };

  // Prompt for a command from the list while looping over the idle function
  g_serial_cli.prompt(commands, g_idle_fn);
}

static uint8_t g_mode;
static uint16_t g_countdown;
static decltype(millis()) g_lastMillis;

void attract_idle() {
  using InitFn = IdleFn(*)();
  struct Entry { InitFn init_fn; uint16_t delay_ms; };
  static const Entry entries[] = {
    { init_logo, 10000 },
    { init_maze, 15000 },
    { init_lj_11, 15000 },
    { init_doge, 10000 },
    { init_bounce, 15000 },
    { init_lj_12, 15000 },
    { init_pepe, 10000 },
    { init_circum, 15000 },
    { init_lj_56, 15000 },
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
