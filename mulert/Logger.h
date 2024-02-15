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
	class MULERT_API LoggerConfig
	{
		/**
		 * @brief 日志等级
		*/
		int logLevel;

		FILE *output;

		FILE *errorOutput;

		bool enableColour;

		LoggerConfig();
	public:
		
		/**
		 * @brief 获取单例实例。
		 * @return 实例。
		*/
		static LoggerConfig &GetInstance();

		/**
		 * @brief 设定初始化
		 * @param logLevel 日志等级
		 * @param out 输出流
		 * @param errorOutput 错误流
		 * @param enableColour 是否允许着色
		*/
		void LoggerInit(int logLevel, FILE *out, FILE *errorOutput, bool enableColour);

		/**
		 * @brief 获取日志等级
		 * @return 日志等级
		*/
		int GetLogLevel();

		/**
		 * @brief 获取输出流
		 * @return 普通日志输出流
		*/
		FILE *GetOutput();

		/**
		 * @brief 获取设定的错误输出流
		 * @return 错误日志输出流
		*/
		FILE *GetErrorOutput();

		/**
		 * @brief 是否允许着色。
		 * @return true 允许，false 禁止。
		*/
		bool IsColourationEnabled();
	};

	/**
	 * @brief 日志记录器类。
	*/
	class MULERT_API Logger
	{
		std::wstring className;

		int logLevel;

		template<typename... Args>
		void Output(FILE * dest, const std::wstring &name, const std::wstring &fmt, Args...args) const
		{
			std::wstring vfmt;
			if (LoggerConfig::GetInstance().IsColourationEnabled())
				vfmt = std::format(L"[{}\033[0m] \033[94m{}\033[0m - {}", name, className, fmt);
			else
				vfmt = std::format(L"[{0}] {2} ({1})", name, className, fmt);

			fputws(std::vformat(vfmt, std::make_wformat_args(args...)).c_str(), dest);
			fputws(L"\n", dest);
		}
	public:

		Logger(const std::string &name, int logLevel);

		template<typename T>
		static Logger GetLogger(int logLevel = -1)
		{
			return Logger(typeid(T).name(), logLevel == -1 ? LoggerConfig::GetInstance().GetLogLevel() : logLevel);
		}
#ifdef DEBUG
		template<typename... Args>
		void Debug(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 0) return;
			if (LoggerConfig::GetInstance().IsColourationEnabled())
				Output(LoggerConfig::GetInstance().GetOutput(), L"\033[96mDebug", fmt, args...);
			else
				Output(LoggerConfig::GetInstance().GetOutput(), L"\033[96mDebug", fmt, args...);
		}
#else
		template<typename... Args>
		void Debug(const std::wstring &fmt, Args...args) const
		{}
#endif
		template<typename... Args>
		void Info(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 1) return;
			Output(LoggerConfig::GetInstance().GetOutput(), L"Info", fmt, args...);
		}

		template<typename... Args>
		void Note(const std::wstring &fmt, Args... args) const
		{
			if (logLevel > 2) return;
			Output(LoggerConfig::GetInstance().GetErrorOutput(), L"Note", fmt, args...);
		}

		template<typename... Args>
		void Warn(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 2) return;
			if (LoggerConfig::GetInstance().IsColourationEnabled())
				Output(LoggerConfig::GetInstance().GetErrorOutput(), L"\033[93mWarn", fmt, args...);
			else
				Output(LoggerConfig::GetInstance().GetOutput(), L"Warn", fmt, args...);
		}

		template<typename... Args>
		void Error(const std::wstring &fmt, Args...args) const
		{
			if (logLevel > 3) return;
			if (LoggerConfig::GetInstance().IsColourationEnabled())
				Output(LoggerConfig::GetInstance().GetErrorOutput(), L"\033[31mError", fmt, args...);
			else
				Output(LoggerConfig::GetInstance().GetErrorOutput(), L"Error", fmt, args...);
		}

		template<typename... Args>
		void Fatal(const std::wstring &fmt, Args...args) const
		{
			if (LoggerConfig::GetInstance().IsColourationEnabled())
				Output(LoggerConfig::GetInstance().GetErrorOutput(), L"\033[91;43mFatal", fmt, args...);
			else
				Output(LoggerConfig::GetInstance().GetErrorOutput(), L"Fatal", fmt, args...);
		}
	};
}

#endif // !LOGGER_H__
