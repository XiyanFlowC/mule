#include "InvalidOperationException.h"

using namespace mule::Exception;

InvalidOperationException::InvalidOperationException(std::string desc, const char* file, int line) : Exception(desc, file, line)
{
}
