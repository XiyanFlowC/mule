#include "NotImplementedException.h"

xybase::NotImplementedException::NotImplementedException()
	: Exception(u"Not implemented yet.", 90002) {}
