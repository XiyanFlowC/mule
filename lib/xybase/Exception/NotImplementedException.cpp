#include "NotImplementedException.h"

xybase::NotImplementedException::NotImplementedException()
	: Exception(L"Not implemented yet.", 90002) {}

xybase::NotImplementedException::~NotImplementedException()
{
}
