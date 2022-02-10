// Copyright (c) 2022 Trevor Makes

#include "uIO.hpp"
#include "uCLI.hpp"

#include <Arduino.h>

#include "main.hpp"

// Function to call while waiting for serial input
uCLI::IdleFn idle_fn = nullptr;

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
  static const uCLI::Command commands[] = {
    { "logo", init_logo },
    { "clear", clear_screen },
    { "print", print_message },
    { "circle", init_circle },
    { "doge", init_doge },
    { "pepe", init_pepe },
  };

  // Prompt for a command from the list while looping over the idle function
  uCLI::run_command(Serial, commands, idle_fn);
}
