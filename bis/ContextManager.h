#pragma once

#ifndef CONTEXT_MANAGER_H__
#define CONTEXT_MANAGER_H__

#include <map>
#include <string>

#include "Data/Basic/MultiValue.h"

namespace mule
{
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

#endif
