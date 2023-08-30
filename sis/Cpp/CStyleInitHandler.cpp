#include "CStyleInitHandler.h"

#include <Exception/InvalidOperationException.h>

using namespace mule::Data::Basic;

int mule::Cpp::CStyleInitHandler::ident = 1, mule::Cpp::CStyleInitHandler::type = 1;

void mule::Cpp::CStyleInitHandler::OnRealmEnter(Type *realm, const std::u16string &name)
{
}

void mule::Cpp::CStyleInitHandler::OnRealmExit(Type *realm, const std::u16string &name)
{
}

void mule::Cpp::CStyleInitHandler::OnRealmEnter(Type *realm, int idx)
{
}

void mule::Cpp::CStyleInitHandler::OnRealmExit(Type *realm, int idx)
{
}

void mule::Cpp::CStyleInitHandler::OnDataRead(const MultiValue &value)
{
}

MultiValue mule::Cpp::CStyleInitHandler::OnDataWrite()
{
	return MultiValue();
}

void mule::Cpp::CStyleInitHandler::OnSheetReadStart()
{
	if (state != CSIHS_IDLE)
		throw xybase::InvalidOperationException(u"This handler is not idle for read.", 10000);
	state = CSIHS_READ;
}

void mule::Cpp::CStyleInitHandler::OnSheetReadEnd()
{
	if (state != CSIHS_IDLE)
		throw xybase::InvalidOperationException(u"This handler is not idle for write.", 10000);
	state = CSIHS_WRITE;
}
