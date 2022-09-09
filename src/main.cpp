// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

IdleFn g_idle_fn = nullptr; // Function to call while waiting for serial input
StreamEx g_serial_ex(Serial); // Stream wrapper that supports ANSI escape codes
CLI g_serial_cli(g_serial_ex); // Command line interface manager

void setup() {
  // Configure I/O ports for output
  init_ports();

  // Call init_logo with empty uCLI::Args
  init_logo({});

  // Establish serial connection with computer
  Serial.begin(9600);
  while (!Serial) {}
}

void loop() {
  static const Command commands[] = {
    // text.cpp
    { "logo", init_logo },
    { "clear", clear_screen },
    { "print", print_message },
    { "maze", init_maze },
    // vector.cpp
    { "circle", init_circle },
    { "cross", init_cross },
    { "bounce", init_bounce },
    // bitmap.cpp
    { "doge", init_doge },
    { "pepe", init_pepe },
    { "fliph", flip_horizontal },
    { "flipv", flip_vertical },
    { "export", export_bitmap },
    { "import", import_bitmap },
    { "delay", set_delay },
  };

  // Prompt for a command from the list while looping over the idle function
  g_serial_cli.run_once(commands, g_idle_fn);
}
