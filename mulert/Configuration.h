#pragma once

#include "mulert_api.h"
#include "Data/Basic/MultiValue.h"
#include <map>
#include <Exception/RuntimeException.h>

namespace mule
{
	/**
	 * @brief 配置提供类
	*/
	class MULERT_API Configuration
	{
		std::map<std::u16string, Data::Basic::MultiValue> variables;
	public:

		/**
		 * @brief 指定的配置项不存在
		*/
		class ConfigurationNotFoundException : public xybase::RuntimeException
		{
		public:
			ConfigurationNotFoundException(std::wstring name);
		};

		/**
		 * @brief 指定的配置项中存在不符合的类型数据
		*/
		class ConfigurationTypeMismatch : public xybase::RuntimeException
		{
		public:
			ConfigurationTypeMismatch(std::wstring name);
		};

		static Configuration &GetInstance();

		void SetVariable(const char16_t *name, mule::Data::Basic::MultiValue value);

		void SetVariable(const char16_t *name, unsigned long long uval);

		void SetVariable(const char16_t *name, long long val);

		void SetVariable(const char16_t *name, double rval);

		void SetVariable(const char16_t *name, const char16_t *sval);

		Data::Basic::MultiValue GetVariable(const char16_t *name);

		const std::u16string GetString(const char16_t *name);

		double GetReal(const char16_t *name, double def = 0.0);

		long long GetSigned(const char16_t *name, long long def = 0);

		unsigned long long GetUnsigned(const char16_t *name, unsigned long long def = 0);
	};
}
