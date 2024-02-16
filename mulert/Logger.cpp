#include "Logger.h"

#include <xystring.h>
#include "Configuration.h"

mule::LoggerConfig::LoggerConfig()
	: logLevel(0), output(stdout), errorOutput(stderr), enableColour(true)
{
}

mule::LoggerConfig &mule::LoggerConfig::GetInstance()
{
	static LoggerConfig _inst;
	return _inst;
}

void mule::LoggerConfig::LoggerInit(int p_logLevel, FILE *p_out, FILE *p_errout, bool p_enableColour)
{
	logLevel = p_logLevel;
	output = p_out;
	errorOutput = p_errout;
	enableColour = p_enableColour;
}

int mule::LoggerConfig::GetLogLevel()
{
	return Configuration::GetInstance().GetSigned(u"mule.log.level", logLevel);
}

FILE *mule::LoggerConfig::GetOutput()
{
	return output;
}

FILE *mule::LoggerConfig::GetErrorOutput()
{
	return errorOutput;
}

bool mule::LoggerConfig::IsColourationEnabled()
{
	return enableColour;
}

mule::Logger::Logger(const std::string &name, int logLevel)
	: className(xybase::string::to_wstring(name)), logLevel(logLevel)
{
}
