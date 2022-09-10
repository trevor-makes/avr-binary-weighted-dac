// Copyright (c) 2022 Trevor Makes

#pragma once

#include "core/cli.hpp"

#include <Arduino.h>

using core::serial::StreamEx;
using core::cli::IdleFn;
using core::cli::Args;
using core::cli::Command;
using CLI = core::cli::CLI<20>; // Limit line buffer to 20 bytes

extern IdleFn g_idle_fn;
extern StreamEx g_serial_ex;
extern CLI g_serial_cli;

void bitmap_idle();
void set_delay(Args);

void do_circle(Args);
void do_cross(Args);
IdleFn init_bounce();
IdleFn init_circum();

void clear_screen(Args);
void print_message(Args);
IdleFn init_logo();
IdleFn init_maze();

IdleFn init_doge();
IdleFn init_pepe();
void flip_vertical(Args);
void flip_horizontal(Args);
void export_bitmap(Args);
void import_bitmap(Args);

void init_ports();
void write_x(uint8_t x);
void write_y(uint8_t y);
