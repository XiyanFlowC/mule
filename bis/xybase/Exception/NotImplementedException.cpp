#include "NotImplementedException.h"

xybase::NotImplementedException::NotImplementedException()
	: RuntimeException("Not implemented yet.", 90002) {}