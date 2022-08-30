// Copyright (c) 2022 Trevor Makes

#include "main.hpp"

#include "core/util.hpp"

constexpr uint8_t MAX_STEPS = 240;
constexpr uint8_t MIN_STEPS = 32;
uint8_t steps = MAX_STEPS;

// Lookup table for cached sine values
uint8_t SINE_TABLE[MAX_STEPS];

// Output sine and cosine to X and Y channels
void draw_sines() {
  for (uint8_t i = 0; i < steps; ++i) {
    // Shift phase of X by 90° (steps/4) for cosine
    write_x(SINE_TABLE[(i + (steps / 4)) % steps]);
    write_y(SINE_TABLE[i]);
  }
}

// Start drawing circle in idle loop
void init_sines(Args args) {
  if (args.has_next()) {
    steps = util::clamp<int>(atoi(args.next()), MIN_STEPS, MAX_STEPS);
    serialEx.print("Using ");
    serialEx.print(steps);
    serialEx.println(" steps in sine table");
  }
  const double RAD_PER_STEP = 2. * PI / double(steps);

  // Compute sine lookup table with domain [0, 2π) and range [0, 62]
  for (uint8_t i = 0; i < steps; ++i) {
    SINE_TABLE[i] = uint8_t(31. * (1. + sin(i * RAD_PER_STEP)) + 0.5);
  }

  // Set idle function to draw circle
  idle_fn = draw_sines;
}
