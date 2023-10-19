#pragma once

#ifndef LOGGER_H__
#define LOGGER_H__

#include <cstdio>
#include <string>
#include <typeinfo>
#include <format>
#include <cwchar>
#include "mulert_api.h"

namespace mule
{
	class MULERT_API LoggerConfig
	{
		static int logLevel;

		static FILE *output;

		static FILE *errout;
	public:

		static void LoggerInit(int logLevel, FILE *out, FILE *errout);

		static int GetLogLevel();

		static FILE *GetOutput();

		static FILE *GetErrorOutput();
	};

	class MULERT_API Logger
	{
		std::wstring className;

		int logLevel;

		template<typename... Args>
		void Output(FILE * dest, const std::wstring &name, const std::wstring &fmt, Args...args) const
		{
#ifndef DISABLE_TERMINAL_ANSI_ESCAPE
			std::wstring vfmt = std::format(L"[{}\033[0m] \033[94m{}\033[0m - {}\n", name, className, fmt);
#else
			std::wstring vfmt = std::format(L"[{0}] {2} ({1})\n", name, className, fmt);
#endif
			fputws(std::vformat(vfmt, std::make_wformat_args(args...)).c_str(), dest);
		}
	public:

		Logger(const std::string &name, int logLevel);

		template<typename T>
		static Logger GetLogger(int logLevel = -1)
		{
			return Logger(typeid(T).name(), logLevel == -1 ? LoggerConfig::GetLogLevel() : logLevel);
		}

		template<typename... Args>
		void Debug(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 0) return;
#ifndef DISABLE_TERMINAL_ANSI_ESCAPE
			Output(LoggerConfig::GetOutput(), L"\033[96mDebug", fmt, args...);
#else
			Output(LoggerConfig::GetOutput(), L"\033[96mDebug", fmt, args...);
#endif
		}

		template<typename... Args>
		void Info(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 1) return;
			Output(LoggerConfig::GetOutput(), L"Info", fmt, args...);
		}

		template<typename... Args>
		void Note(const std::wstring &fmt, Args... args) const
		{
			if (logLevel > 2) return;
			Output(LoggerConfig::GetErrorOutput(), L"Note", fmt, args...);
		}

		template<typename... Args>
		void Warn(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 2) return;
#ifndef DISABLE_TERMINAL_ANSI_ESCAPE
			Output(LoggerConfig::GetOutput(), L"\033[93mWarn", fmt, args...);
#else
			Output(LoggerConfig::GetOutput(), L"Warn", fmt, args...);
#endif
		}

		template<typename... Args>
		void Error(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 3) return;
#ifndef DISABLE_TERMINAL_ANSI_ESCAPE
			Output(LoggerConfig::GetErrorOutput(), L"\033[31mError", fmt, args...);
#else
			Output(LoggerConfig::GetErrorOutput(), L"Error", fmt, args...);
#endif
		}

		template<typename... Args>
		void Fatal(const std::wstring &fmt, Args...args) const
		{
#ifndef DISABLE_TERMINAL_ANSI_ESCAPE
			Output(LoggerConfig::GetErrorOutput(), L"\033[91;43mFatal", fmt, args...);
#else
			Output(LoggerConfig::GetErrorOutput(), L"Fatal", fmt, args...);
#endif
		}
	};
}

#endif // !LOGGER_H__
