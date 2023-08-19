#include "ContextManager.h"

using namespace mule;

ContextManager &mule::ContextManager::GetInstance()
{
	static ContextManager _inst;
	return _inst;
}

void mule::ContextManager::SetVariable(const std::u16string &name, const mule::Data::Basic::MultiValue &value)
{
	variables[name] = value;
}

const mule::Data::Basic::MultiValue &mule::ContextManager::GetVariable(const std::u16string &name)
{
	return variables[name];
}
