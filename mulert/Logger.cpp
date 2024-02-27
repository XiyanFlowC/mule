#include "Logger.h"

#include <Exception/IOException.h>
#include <xystring.h>
#include "Configuration.h"

mule::LoggerBase::LoggerBase()
	: logLevel(0), output(stdout), errorOutput(stderr), enableColour(true)
{
}

mule::LoggerBase &mule::LoggerBase::GetInstance()
{
	static LoggerBase _inst;
	return _inst;
}

void mule::LoggerBase::LoggerInit(int logLevel, std::string outfile, std::string errfile)
{
	this->logLevel = logLevel;
	SetStandardOutput(outfile);
	SetErrorOutput(errfile);
	enableColour = false;
}

mule::Logger::Logger(const std::string &name)
	: className(xybase::string::to_wstring(name))
{
}

mule::Logger::~Logger()
{
}

void mule::LoggerBase::LoggerInit(int logLevel, bool enableColor)
{
	output = stdout;
	errorOutput = stderr;
	enableColour = enableColor;
}

void mule::LoggerBase::SetStandardOutput(std::string filename)
{
	if (output != stdout) fclose(output);
	output = fopen(filename.c_str(), "w");
	if (output == nullptr)
	{
		output = stdout;
		throw xybase::IOException(xybase::string::to_wstring(filename), L"Cannot open file to write");
	}
	enableColour = false;
}

void mule::LoggerBase::SetErrorOutput(std::string filename)
{
	if (errorOutput != stderr) fclose(output);
	errorOutput = fopen(filename.c_str(), "w");
	if (errorOutput == nullptr)
	{
		errorOutput = stderr;
		throw xybase::IOException(xybase::string::to_wstring(filename), L"Cannot open file to write");
	}
	enableColour = false;
}

void mule::LoggerBase::SetBothOutput(std::string filename)
{
	if (output != stdout) fclose(output);
	if (errorOutput != stderr) fclose(output);
	errorOutput = fopen(filename.c_str(), "w");
	if (errorOutput == nullptr)
	{
		errorOutput = stderr;
		throw xybase::IOException(xybase::string::to_wstring(filename), L"Cannot open file to write");
	}
	output = errorOutput;
	enableColour = false;
}

void mule::LoggerBase::StandardOutput(std::wstring message)
{
	fputws(message.c_str(), output);
	fputwc(L'\n', output);
}

void mule::LoggerBase::ErrorOutput(std::wstring message)
{
	fputws(message.c_str(), errorOutput);
	fputwc(L'\n', errorOutput);
}

void mule::LoggerBase::Info(std::wstring message, std::wstring logger)
{
	if (logLevel > 0) return;
	StandardOutput(enableColour ?
		std::format(L"[Info] \033[94m{}\033[0m: {}", logger, message)
		: std::format(L"[Info] {} - {}", message, logger)
	);
}

void mule::LoggerBase::Warn(std::wstring message, std::wstring logger)
{
	if (logLevel > 1) return;
	ErrorOutput(enableColour ?
		std::format(L"[\033[93mWarn\033[0m] \033[94m{}\033[0m: {}", logger, message)
		: std::format(L"[Warn] {} - {}", message, logger)
	);
}

void mule::LoggerBase::Note(std::wstring message, std::wstring logger)
{
	if (logLevel > 1) return;
	ErrorOutput(enableColour ?
		std::format(L"[Note] {} - {}", logger, message)
		: std::format(L"[Note] {} - {}", message, logger)
	);
}

MULERT_API void mule::LoggerBase::Error(std::wstring message, std::wstring logger)
{
	if (logLevel > 2) return;
	ErrorOutput(enableColour ?
		std::format(L"[\033[31mError\033[0m] \033[94m{}\033[0m: \033[31m{}\033[0m", logger, message)
		: std::format(L"[Error] {} - {}", message, logger)
	);
}

MULERT_API void mule::LoggerBase::Fatal(std::wstring message, std::wstring logger)
{
	if (logLevel > 3) return;
	ErrorOutput(enableColour ?
		std::format(L"[\033[91;43mFatal\033[0m] \033[94m{}\033[0m: \033[31m{}\033[0m", logger, message)
		: std::format(L"[Fatal] {} - {}", message, logger)
	);
}

void mule::LoggerBase::Debug(std::wstring message, std::wstring logger)
{
	if (logLevel > 0) return;
	StandardOutput(enableColour ?
		std::format(L"[\033[96mDebug\033[0m] \033[94m{}\033[0m: {}", logger, message)
		: std::format(L"[Info] {} - {}", message, logger)
	);
}
