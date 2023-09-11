#include "Stream.h"

bool xybase::bigEndianSystem = (*((const uint16_t *)endianTester) == 0xFF00);

xybase::Stream::~Stream()
{
}
