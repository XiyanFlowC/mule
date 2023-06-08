#include "Exception.h"

using namespace mule::Exception;

Exception::Exception(std::string description, const char* file, int line) noexcept
{
	this->description = "Exception message: " + description + "\n\t" + file + ":" + std::to_string(line) + "\n";
}

const char *Exception::what() const
{
	return description.c_str();
}

const char *Exception::What() const
{
	return what();
}
