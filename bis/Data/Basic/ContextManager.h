#pragma once

#ifndef CONTEXT_MANAGER_H__
#define CONTEXT_MANAGER_H__

#include <map>
#include <string>

#include <Data/Basic/MultiValue.h>

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			/**
			 * @brief 为各个类型提供上下文保存能力：各类型自行决定是否保存该信息，或是否能够保存该信息。
			 * Provides context for types. Each type decides if context information should be saved.
			*/
			class ContextManager
			{
			protected:
				std::map<std::u16string, mule::Data::Basic::MultiValue> variables;
			public:

				static ContextManager &GetInstance();

				void SetVariable(const std::u16string &name, const mule::Data::Basic::MultiValue &value);

				const mule::Data::Basic::MultiValue &GetVariable(const std::u16string &name);
			};
		}
	}
}

#endif
