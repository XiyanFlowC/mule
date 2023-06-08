#pragma once
#ifndef LUA_DATA_HANDLER__
#define LUA_DATA_HANDLER__

#include <Data/Basic/DataHandler.h>
#include "LuaHost.h"

namespace mule
{
	namespace Lua
	{
		class LuaDataHandler : public mule::Data::Basic::DataHandler
		{
			LuaDataHandler(std::string &handlerName);

		protected:
			std::string onSrs, onSws, onSre, onSwe, onRrs, onRre, onRws, onRwe, onCr, onCw;

			virtual void OnSheetReadingStarted() override;

			virtual void OnSheetWritingStarted() override;

			virtual void OnSheetReadingEnded() override;

			virtual void OnSheetWritingEnded() override;

			virtual void OnRecordReadingStarted() override;

			virtual void OnRecordWritingStarted() override;

			virtual void OnRecordReadingEnded() override;

			virtual void OnRecordWritingEnded() override;

			virtual void OnCellRead(const mule::Data::Basic::MultiValue &value) override;

			virtual mule::Data::Basic::MultiValue OnCellWrite() override;
		};
	}
}

#endif
