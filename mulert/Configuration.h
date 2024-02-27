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
	class Configuration
	{
		std::map<std::u16string, Data::Basic::MultiValue> variables;
	public:

		/**
		 * @brief 指定的配置项不存在
		*/
		class ConfigurationNotFoundException : public xybase::RuntimeException
		{
		public:
			MULERT_API ConfigurationNotFoundException(std::wstring name);

			MULERT_API ~ConfigurationNotFoundException();
		};

		/**
		 * @brief 指定的配置项中存在不符合的类型数据
		*/
		class ConfigurationTypeMismatch : public xybase::RuntimeException
		{
		public:
			MULERT_API ConfigurationTypeMismatch(std::wstring name);

			MULERT_API ~ConfigurationTypeMismatch();
		};

		MULERT_API static Configuration &GetInstance();

		MULERT_API std::u16string ResolveVariable(const std::u16string &str);

		MULERT_API void ResetVariable(const char16_t *name);

		MULERT_API void SetVariable(const char16_t *name, mule::Data::Basic::MultiValue value);

		MULERT_API void SetVariable(const char16_t *name, unsigned long long uval);

		MULERT_API void SetVariable(const char16_t *name, long long val);

		MULERT_API void SetVariable(const char16_t *name, double rval);

		MULERT_API void SetVariable(const char16_t *name, const char16_t *sval);

		MULERT_API Data::Basic::MultiValue GetVariable(const char16_t *name);

		MULERT_API const std::u16string GetString(const char16_t *name);

		MULERT_API double GetReal(const char16_t *name, double def = 0.0);

		MULERT_API long long GetSigned(const char16_t *name, long long def = 0);

		MULERT_API unsigned long long GetUnsigned(const char16_t *name, unsigned long long def = 0);

		MULERT_API bool IsExist(const char16_t *name);
	};
}
