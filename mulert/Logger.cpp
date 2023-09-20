#include "Logger.h"

#include <xystring.h>

int mule::LoggerConfig::logLevel = 0;

FILE *mule::LoggerConfig::output = stdout;

FILE *mule::LoggerConfig::errout = stderr;

void mule::LoggerConfig::LoggerInit(int p_logLevel, FILE *p_out, FILE *p_errout)
{
	logLevel = p_logLevel;
	output = p_out;
	errout = p_errout;
}

int mule::LoggerConfig::GetLogLevel()
{
	return logLevel;
}

FILE *mule::LoggerConfig::GetOutput()
{
	return output;
}

FILE *mule::LoggerConfig::GetErrorOutput()
{
	return errout;
}

mule::Logger::Logger(const std::string &name, int logLevel)
	: className(xybase::string::to_wstring(name)), logLevel(logLevel)
{
}
