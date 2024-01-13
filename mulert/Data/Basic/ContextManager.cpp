#include "ContextManager.h"

using namespace mule::Data::Basic;

ContextManager &ContextManager::GetInstance()
{
	static ContextManager _inst;
	return _inst;
}

void ContextManager::SetVariable(const std::u16string &name, const mule::Data::Basic::MultiValue &value)
{
	variables[name] = value;
}

const mule::Data::Basic::MultiValue &ContextManager::GetVariable(const std::u16string &name)
{
	return variables[name];
}
