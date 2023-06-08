#include "InvalidParameterException.h"

using namespace mule::Exception;

InvalidParameterException::InvalidParameterException(std::string parameter_name, std::string description, const char* name, int line)
	: Exception(std::string("Parameter [") + parameter_name + "] is invalid because: " + description, name, line) {}
