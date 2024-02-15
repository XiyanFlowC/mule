#include "SheetManager.h"

using namespace mule;

SheetManager &mule::SheetManager::GetInstance()
{
	static SheetManager _inst;
	return _inst;
}
