#include "NotImplementedException.h"

mule::Exception::NotImplementedException::NotImplementedException(const char *name, int line)
	: Exception("Feature has not been implemented yet.", name, line)
{
}
