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
	/**
	 * @brief 单例类。日志记录器配置用类
	*/
	class LoggerBase
	{
		/**
		 * @brief 日志等级
		*/
		int logLevel;

		FILE *output;

		FILE *errorOutput;

		bool enableColour;

		LoggerBase();
	public:
		
		/**
		 * @brief 获取单例实例。
		 * @return 实例。
		*/
		MULERT_API static LoggerBase &GetInstance();

		/**
		 * @brief Logger init. Set log level and set the output to files.
		 * @param logLevel Log level threshold.
		 * @param outfile Normal output file name.
		 * @param errfile Error output file name.
		*/
		MULERT_API void LoggerInit(int logLevel, std::string outfile, std::string errfile);

		/**
		 * @brief Logger init. Set log level and set the output to stdout & stderr.
		 * @param logLevel Log level threshold.
		*/
		MULERT_API void LoggerInit(int logLevel, bool enableColor);

		MULERT_API void SetStandardOutput(std::string filename);

		MULERT_API void SetErrorOutput(std::string filename);

		MULERT_API void SetBothOutput(std::string filename);

		MULERT_API void StandardOutput(std::wstring message);

		MULERT_API void ErrorOutput(std::wstring message);

		MULERT_API void Info(std::wstring message, std::wstring logger = L"<Global>");

		MULERT_API void Warn(std::wstring message, std::wstring logger = L"<Global>");

		MULERT_API void Note(std::wstring message, std::wstring logger = L"<Global>");

		MULERT_API void Error(std::wstring message, std::wstring logger = L"<Global>");

		MULERT_API void Fatal(std::wstring message, std::wstring logger = L"<Global>");

		MULERT_API void Debug(std::wstring message, std::wstring logger = L"<Global>");
	};

	/**
	 * @brief 日志记录器类。
	*/
	class Logger
	{
		std::wstring className;
	public:

		MULERT_API Logger(const std::string &name);

		MULERT_API virtual ~Logger();

		template<typename T>
		static Logger GetLogger(int logLevel = -1)
		{
			return Logger(typeid(T).name());
		}
#ifdef NDEBUG
		template<typename... Args>
		void Debug(const std::wstring &fmt, Args...args) const
		{}
#else
		template<typename... Args>
		void Debug(const std::wstring &fmt, Args...args) const
		{
			LoggerBase::GetInstance().Debug(std::vformat(fmt, std::make_wformat_args(args...)), className);
		}
#endif
		template<typename... Args>
		void Info(const std::wstring &fmt, Args...args) const
		{
			LoggerBase::GetInstance().Info(std::vformat(fmt, std::make_wformat_args(args...)), className);
		}

		template<typename... Args>
		void Note(const std::wstring &fmt, Args... args) const
		{
			LoggerBase::GetInstance().Note(std::vformat(fmt, std::make_wformat_args(args...)), className);
		}

		template<typename... Args>
		void Warn(const std::wstring &fmt, Args...args) const
		{
			LoggerBase::GetInstance().Warn(std::vformat(fmt, std::make_wformat_args(args...)), className);
		}

		template<typename... Args>
		void Error(const std::wstring &fmt, Args...args) const
		{
			LoggerBase::GetInstance().Error(std::vformat(fmt, std::make_wformat_args(args...)), className);
		}

		template<typename... Args>
		void Fatal(const std::wstring &fmt, Args...args) const
		{
			LoggerBase::GetInstance().Fatal(std::vformat(fmt, std::make_wformat_args(args...)), className);
		}
	};
}

#endif // !LOGGER_H__
