#pragma once
#ifndef LUA_DATA_HANDLER__
#define LUA_DATA_HANDLER__

#include <Data/Basic/Object.h>
#include "LuaHost.h"

namespace mule
{
	namespace Lua
	{
		class LuaDataHandler : public mule::Data::Basic::Object::DataHandler
		{
		public:
			virtual void OnRealmEnter(mule::Data::Basic::Object *realm, std::string name) override;
			virtual void OnRealmExit(mule::Data::Basic::Object *realm, std::string name) override;
			virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
			virtual mule::Data::Basic::MultiValue OnDataWrite() override;
			virtual void OnRealmEnter(mule::Data::Basic::Object *realm, int idx) override;
			virtual void OnRealmExit(mule::Data::Basic::Object *realm, int idx) override;
		protected:
		};
	}
}

#endif
