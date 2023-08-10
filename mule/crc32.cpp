#include "crc32.h"

uint32_t crctbl[256];

uint32_t crc32_eval(const uint8_t *data, size_t size)
{
    uint32_t crc = 0xffffffff;
    for (size_t i = 0; i < size; ++i)
    {
        crc = crctbl[(crc ^ data[i]) & 0xff] ^ (crc >> 8);
    }
    return crc ^ 0xffffffff;
}

void crc32_init()
{
    for (int i = 0; i < 256; ++i)
    {
        uint32_t crc = (uint32_t)i;
        for (int j = 0; j < 8; ++j)
        {
            crc = crc & 1 ? 0xedb88320 ^ (crc >> 1) : crc >> 1;
        }
        crctbl[i] = crc;
    }
}
