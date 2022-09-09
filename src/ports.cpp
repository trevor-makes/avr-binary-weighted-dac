// Copyright (c) 2022 Trevor Makes

#include "core/io.hpp"

#include "main.hpp"

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
  CORE_PORT(B)
  CORE_PORT(C)

  // For ATmega 328p boards, the two highest bits of ports B and C are
  // unavailable, so we set a mask to prevent writing to these bits.
  // This allows values from 0 to 63 to be written to each port.
  //         x |         | C5 - RΩ
  //         x |         | C4 - 2RΩ
  //         x |         | C3 - 4RΩ
  //         x | Arduino | C2 - 8RΩ
  // 32RΩ - B0 |   NANO  | C1 - 16RΩ
  // 16RΩ - B1 |         | C0 - 32RΩ
  //  8RΩ - B2 |   ___   | x
  //  4RΩ - B3 |  |USB|  | x
  //  2RΩ - B4 |__|___|__| B5 - RΩ
  // NOTE there is no harm in writing the unused upper bits of each port,
  // and removing the mask significantly speeds up vector drawing
  using XAxis = PortB;//::Mask<0x3F>;
  using YAxis = PortC;//::Mask<0x3F>;
#else
  #error The I/O port mapping has not been defined for the target platform
#endif

void init_ports() {
  XAxis::config_output();
  YAxis::config_output();
}

void write_x(uint8_t x) {
  XAxis::write(x);
}

void write_y(uint8_t y) {
  YAxis::write(y);
}
