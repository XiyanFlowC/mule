#include "LuaHost.h"

#include <xystring.h>

using namespace mule::Lua;
using namespace mule::Data::Basic;

LuaHost::LuaHost()
{
	L = luaL_newstate();
	disposable = true;
}

LuaHost::~LuaHost()
{
	if (disposable)
		lua_close(L);
	disposable = false;
}

LuaHost& LuaHost::GetInstance()
{
	static LuaHost inst;
	return inst;
}

mule::Lua::LuaHost::LuaHost(lua_State *L)
{
	this->L = L;
}

static const luaL_Reg selectedLibs[] = {
  {LUA_GNAME, luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_UTF8LIBNAME, luaopen_utf8},
  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};

void LuaHost::LoadLuaStandardLibs()
{
	// luaL_openlibs(L);
	// Codes from luaL_openlibs to register only selected libs.
	for (const luaL_Reg *lib = selectedLibs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);  /* remove lib */
	}
}

void LuaHost::RunScript(const char* path)
{
	LoadScript(path);
	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result != LUA_OK) throw LuaException(
		L"Run script " + xybase::string::to_wstring(path) + L" failed: " + xybase::string::to_wstring(lua_tostring(L, -1)),
		__LINE__);
}

MultiValue mule::Lua::LuaHost::RunString(const char *str)
{
	if (luaL_loadstring(L, str) != LUA_OK)
	{
		throw LuaException(L"Load error: " + xybase::string::to_wstring(lua_tostring(L, -1)), __LINE__);
	}
	
	if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
	{
		throw LuaException(L"Failed to execute: " + xybase::string::to_wstring(lua_tostring(L, -1)), __LINE__);
	}

	int num = lua_gettop(L);
	if (num == 1)
	{
		return PopValue();
	}

	MultiValue ret{ MultiValue::MVT_ARRAY, num };
	for (int i = 0; i < num; ++i)
	{
		ret.value.arrayValue[i] = PopValue();
	}
	return ret;
}

void LuaHost::LoadScript(const char* path)
{
	int result = luaL_loadfile(L, path);
	if (result != LUA_OK)
		throw LuaException(L"Cannot load file [" + xybase::string::to_wstring(path) + L"], err:" + xybase::string::to_wstring(std::to_string(result)), __LINE__);
}

void LuaHost::RegisterFunction(const std::string& name, lua_CFunction func)
{
	lua_register(L, name.c_str(), func);
}

mule::Data::Basic::MultiValue mule::Lua::LuaHost::GetGlobal(const std::string &name)
{
	int top = GetStackTop();
	// 若是表
	size_t dotIndex = name.find_first_of('.'), startIndex = 0;
	if (dotIndex != std::string::npos)
	{
		lua_getglobal(L, name.substr(startIndex, dotIndex - startIndex).c_str());
		if (lua_isnil(L, -1)) throw LuaException(L"Invalid table " + xybase::string::to_wstring(name.substr(startIndex, dotIndex - startIndex)), __LINE__);
		startIndex = dotIndex + 1;
		dotIndex = name.find_first_of('.', startIndex);
		while (dotIndex != std::string::npos)
		{
			lua_getfield(L, -1, name.substr(startIndex, dotIndex - startIndex).c_str());
			if (lua_isnil(L, -1)) throw LuaException(L"Invalid table " + xybase::string::to_wstring(name.substr(startIndex, dotIndex - startIndex)), __LINE__);
			startIndex = dotIndex + 1;
			dotIndex = name.find_first_of('.', startIndex);
		}
		lua_getfield(L, -1, name.substr(startIndex).c_str());
		MultiValue ret = GetValue(-1);
		SetStackTop(top);
		return ret;
	}
	else
	{
		lua_getglobal(L, name.c_str());
		return PopValue();
	}
	SetStackTop(top);
}

mule::Data::Basic::MultiValue mule::Lua::LuaHost::GetValue(int idx)
{
	MultiValue ret;
	if (lua_isboolean(L, idx))
		ret = MultiValue((int64_t)lua_toboolean(L, idx));
	else if (lua_isinteger(L, idx))
		ret = MultiValue((int64_t)lua_tointeger(L, idx));
	else if (lua_isnumber(L, idx))
		ret = MultiValue(lua_tonumber(L, idx));
	else if (lua_isstring(L, idx))
		ret = MultiValue(xybase::string::to_utf16(lua_tostring(L, idx)));
	else if (lua_isnil(L, idx))
		ret = MultiValue();
	else if (lua_istable(L, idx))
	{
		std::map<MultiValue, MultiValue> result;

		// 遍历LuaTable
		lua_pushnil(L);  // 将nil推入栈顶作为遍历的起始点

		// 入栈的key使要求的值离栈顶更远了
		if (idx < 0)
			--idx;
		while (lua_next(L, idx) != 0) {
			// 获取key和value
			const MultiValue key = GetValue(-2);
			const MultiValue value = GetValue(-1);

			// 将key和value插入std::map
			result[key] = value;

			// 弹出value，保留key用于下一次迭代
			lua_pop(L, 1);
		}

		ret.SetValue(result);
	}
	else throw LuaException(L"Unhandlable value.", __LINE__);
	return ret;
}

void mule::Lua::LuaHost::SetGlobal(const std::string &name, const mule::Data::Basic::MultiValue &value)
{
	int top = GetStackTop();
	// 若是表
	size_t dotIndex = name.find_first_of('.'), startIndex = 0;
	if (dotIndex != std::string::npos)
	{
		lua_getglobal(L, name.substr(startIndex, dotIndex - startIndex).c_str());
		if (lua_isnil(L, -1)) throw LuaException(L"Invalid table " + xybase::string::to_wstring(name.substr(startIndex, dotIndex - startIndex)), __LINE__);
		startIndex = dotIndex + 1;
		dotIndex = name.find_first_of('.', startIndex);
		while (dotIndex != std::string::npos)
		{
			lua_getfield(L, -1, name.substr(startIndex, dotIndex - startIndex).c_str());
			if (lua_isnil(L, -1)) throw LuaException(L"Invalid table " + xybase::string::to_wstring(name.substr(startIndex, dotIndex - startIndex)), __LINE__);
			startIndex = dotIndex + 1;
			dotIndex = name.find_first_of('.', startIndex);
		}
		PushValue(value);
		lua_setfield(L, -2, name.substr(startIndex).c_str());
	}
	else
	{
		PushValue(value);
		lua_setglobal(L, name.c_str());
	}
	SetStackTop(top);
}

MultiValue LuaHost::Call(const std::string name, int count, ...)
{
	int top = GetStackTop();
	MultiValue ret{};
	// 若是表
	size_t dotIndex = name.find_first_of('.'), startIndex = 0;
	if (dotIndex != std::string::npos)
	{
		lua_getglobal(L, name.substr(startIndex, dotIndex - startIndex).c_str());
		if (lua_isnil(L, -1)) throw LuaException(L"Invalid table " + xybase::string::to_wstring(name.substr(startIndex, dotIndex - startIndex)), __LINE__);
		startIndex = dotIndex + 1;
		dotIndex = name.find_first_of('.', startIndex);
		while (dotIndex != std::string::npos)
		{
			lua_getfield(L, -1, name.substr(startIndex, dotIndex - startIndex).c_str());
			if (lua_isnil(L, -1)) throw LuaException(L"Invalid table " + xybase::string::to_wstring(name.substr(startIndex, dotIndex - startIndex)), __LINE__);
			startIndex = dotIndex + 1;
			dotIndex = name.find_first_of('.', startIndex);
		}
		lua_setfield(L, -2, name.substr(startIndex).c_str());
	}
	else
	{
		lua_getglobal(L, name.c_str());
	}

	va_list p;
	va_start(p, count);
	for (int i = 0; i < count; ++i)
	{
		MultiValue *v = va_arg(p, MultiValue*);
		PushValue(*v);
	}
	lua_call(L, count, 1);

	ret = PopValue();
	SetStackTop(top);
	return ret;
}

mule::Data::Basic::MultiValue mule::Lua::LuaHost::PopValue()
{
	MultiValue ret = GetValue(-1);
	lua_pop(L, 1);
	return ret;
}

void mule::Lua::LuaHost::PushValue(const mule::Data::Basic::MultiValue &v)
{
	switch (v.type)
	{
	case MultiValue::ValueType::MVT_NULL:
		lua_pushnil(L);
		break;
	case MultiValue::ValueType::MVT_INT:
		lua_pushinteger(L, v.value.signedValue);
		break;
	case MultiValue::ValueType::MVT_UINT:
		lua_pushinteger(L, v.value.unsignedValue);
		break;
	case MultiValue::ValueType::MVT_REAL:
		lua_pushnumber(L, v.value.realValue);
		break;
	case MultiValue::ValueType::MVT_STRING:
		lua_pushstring(L, xybase::string::to_string(*v.value.stringValue).c_str());
		break;
	case MultiValue::ValueType::MVT_MAP:
		lua_newtable(L);

		for (const auto &entry : *v.value.mapValue)
		{
			PushValue(entry.first);
			PushValue(entry.second);

			lua_settable(L, -3);
		}
		break;
	case MultiValue::MVT_ARRAY:
		lua_newtable(L);

		for (size_t i = 0; i < v.GetLength(); ++i)
		{
			lua_pushinteger(L, i);
			PushValue(v.value.arrayValue[i]);

			lua_settable(L, -3);
		}
		break;
	}
}

void LuaHost::ResetStack()
{
	lua_settop(L, 0);
}

int mule::Lua::LuaHost::GetStackTop()
{
	return lua_gettop(L);
}

void mule::Lua::LuaHost::SetStackTop(int idx)
{
	lua_settop(L, idx);
}

LuaException::LuaException(std::wstring desc, int line)
	: Exception(desc, line)
{
}
