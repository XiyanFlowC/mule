#pragma once

#include "Data/Sheet.h"
#include "mulert_api.h"
#include <map>
#include <list>

namespace mule
{
	class MULERT_API SheetManager
	{
		std::map<std::u16string, std::list<Data::Sheet *>> streamSheets;
	public:
		static SheetManager &GetInstance();


	};
}
