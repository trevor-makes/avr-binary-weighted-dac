// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

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
  // Compute sine lookup table with domain [0, 2π) and range [0, 62]
  for (uint8_t i = 0; i < SIN_STEPS; ++i) {
    SINE_TABLE[i] = uint8_t(31. * (1. + sin(i * RAD_PER_STEP)) + 0.5);
  }

  // Set idle function to draw circle
  idle_fn = draw_circle;
}
