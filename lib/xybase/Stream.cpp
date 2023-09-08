#include "Stream.h"

bool xybase::Stream::bigEndianSystem = (*((const uint16_t *)endianTester) == 0xFF00);

xybase::Stream::~Stream()
{
}
