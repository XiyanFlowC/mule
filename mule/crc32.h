#pragma once

#ifndef CRC32_H
#define CRC32_H

#include <cstdlib>
#include <cstdint>

uint32_t crc32_eval(const uint8_t *data, size_t size);

void crc32_init();

#endif
