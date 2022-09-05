// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

IdleFn idle_fn = nullptr; // Function to call while waiting for serial input
StreamEx serialEx(Serial); // Stream wrapper that supports ANSI escape codes
CLI serialCLI(serialEx); // Limit CLI to 20 byte line length

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
    // vector.cpp
    { "sines", init_sines },
    { "bounce", init_bounce },
    { "star", init_star },
    // bitmap.cpp
    { "doge", init_doge },
    { "pepe", init_pepe },
    { "fliph", flip_horizontal },
    { "flipv", flip_vertical },
    { "save", save_bitmap },
    { "load", load_bitmap },
  };

  // Prompt for a command from the list while looping over the idle function
  serialCLI.run_once(commands, idle_fn);
}
