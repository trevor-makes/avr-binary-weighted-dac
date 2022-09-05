// Copyright (c) 2022 Trevor Makes

#pragma once

#include "core/cli.hpp"

#include <Arduino.h>

using core::serial::StreamEx;
using core::cli::IdleFn;
using core::cli::Args;
using core::cli::Command;
using CLI = core::cli::CLI<20>;

extern IdleFn idle_fn;
extern StreamEx serialEx;
extern CLI serialCLI;

void update_bitmap();

void init_circle(Args);
void init_cross(Args);
void init_bounce(Args);

void clear_screen(Args);
void init_logo(Args);
void print_message(Args);

void init_doge(Args);
void init_pepe(Args);
void flip_vertical(Args);
void flip_horizontal(Args);
void save_bitmap(Args);
void load_bitmap(Args);

void init_ports();
void write_x(uint8_t x);
void write_y(uint8_t y);
