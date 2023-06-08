#include "LuaDataHandler.h"

using namespace mule::Lua;
using namespace mule::Data::Basic;

LuaDataHandler::LuaDataHandler(std::string &handlerName)
{
	onSrs = handlerName + ".onSheetReadStarted";
	onSre = handlerName + ".onSheetReadEnded";
	onSwe = handlerName + ".onSheetWriteEnded";
	onSws = handlerName + ".onSheetWriteStarted";
	onRrs = handlerName + ".onRecordReadStarted";
	onRre = handlerName + ".onRecordReadEnded";
	onRws = handlerName + ".onRecordWriteStarted";
	onRwe = handlerName + ".onRecordWriteEnded";
	onCr = handlerName + ".onCellRead";
	onCw = handlerName + ".onCellWrite";
}

void LuaDataHandler::OnSheetReadingStarted()
{
	LuaHost::GetInstance().Call(onSrs, 1, MultiValue(tableName));
}

void LuaDataHandler::OnSheetWritingStarted()
{
	LuaHost::GetInstance().Call(onSws, 0);
}

void LuaDataHandler::OnSheetReadingEnded()
{
	LuaHost::GetInstance().Call(onSre, 0);
}

void LuaDataHandler::OnSheetWritingEnded()
{
	LuaHost::GetInstance().Call(onSwe, 0);
}

void LuaDataHandler::OnRecordReadingStarted()
{
	LuaHost::GetInstance().Call(onRrs, 0);
}

void LuaDataHandler::OnRecordWritingStarted()
{
	LuaHost::GetInstance().Call(onRws, 0);
}

void LuaDataHandler::OnRecordReadingEnded()
{
	LuaHost::GetInstance().Call(onRre, 0);
}

void LuaDataHandler::OnRecordWritingEnded()
{
	LuaHost::GetInstance().Call(onRwe, 0);
}

void LuaDataHandler::OnCellRead(const MultiValue &value)
{
	LuaHost::GetInstance().Call(onCr, 1, value);
}

MultiValue LuaDataHandler::OnCellWrite()
{
	return LuaHost::GetInstance().Call(onCw, 0);
}
