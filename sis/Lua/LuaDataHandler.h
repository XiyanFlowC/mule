#pragma once
#ifndef LUA_DATA_HANDLER__
#define LUA_DATA_HANDLER__

#include <Data/Basic/Type.h>
#include "LuaHost.h"

namespace mule
{
	namespace Lua
	{
		class LuaDataHandler : public mule::Data::Basic::Type::DataHandler
		{
		public:
			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;
		protected:
		};
	}
}

#endif
