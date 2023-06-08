#include "RestrictionViolationException.h"

using namespace mule::Exception;

RestrictionViolationException::RestrictionViolationException(std::string name, std::string value, std::string actualValue, const char* file, int line) : Exception(name + " should be " + value + " but got " + actualValue, file, line)
{
}
