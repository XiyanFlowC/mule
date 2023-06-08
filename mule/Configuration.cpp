#include "Configuration.h"

Configuration& Configuration::GetInstance()
{
	static Configuration inst;
	return inst;
}
