#include "LuaHost.h"

using namespace mule::Lua;
using namespace mule::Data::Basic;

LuaHost::LuaHost()
{
	L = luaL_newstate();
}

inline LuaHost::~LuaHost()
{
	lua_close(L);
}

LuaHost& LuaHost::GetInstance()
{
	static LuaHost inst;
	return inst;
}

void LuaHost::LoadLuaStandardLibs()
{
	luaL_openlibs(L);
}

void LuaHost::RunScript(const char* path)
{
	LoadScript(path);
	int result = lua_pcall(L, 0, 0, 0);
	if (result != LUA_OK) throw LuaException(
		std::string("Run script ") + path + " failed: " + lua_tostring(L, -1),
		__FILE__, __LINE__);
}

void LuaHost::LoadScript(const char* path)
{
	int result = luaL_loadfile(L, path);
	if (result != LUA_OK) throw LuaException(std::string("Cannot load file [") + path + "], err:" + std::to_string(result), __FILE__, __LINE__);
}

void LuaHost::RegisterFunction(const std::string& name, lua_CFunction func)
{
	lua_register(L, name.c_str(), func);
}

void mule::Lua::LuaHost::RegisterLibrary(luaL_Reg *libRegTable)
{
	luaL_newlib(L, libRegTable);
}

mule::Data::Basic::MultiValue mule::Lua::LuaHost::GetGlobal(const std::string &name)
{
	lua_getglobal(L, name.c_str());
	return PopValue();
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
		ret = MultiValue(lua_tostring(L, idx));
	else if (lua_isnil(L, idx))
		ret = MultiValue();
	else if (lua_istable(L, idx))
	{
		std::map<MultiValue, MultiValue> result;

		// 遍历LuaTable
		lua_pushnil(L);  // 将nil推入栈顶作为遍历的起始点
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
	else throw LuaException("Unhandlable value.", __FILE__, __LINE__);
	return ret;
}

void mule::Lua::LuaHost::SetGlobal(const std::string &name, const mule::Data::Basic::MultiValue &value)
{
	PushValue(value);
	lua_setglobal(L, name.c_str());
}

MultiValue LuaHost::Call(const std::string name, int count, ...)
{
	lua_getglobal(L, name.c_str());

	va_list p;
	va_start(p, count);
	for (int i = 0; i < count; ++i)
	{
		MultiValue &v = va_arg(p, MultiValue);
		PushValue(v);
	}
	lua_call(L, count, 1);
	
	MultiValue ret = PopValue();

	return ret;
}

mule::Data::Basic::MultiValue mule::Lua::LuaHost::PopValue()
{
	MultiValue ret;
	GetValue(-1);
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
		lua_pushstring(L, v.value.stringValue->c_str());
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

LuaException::LuaException(std::string desc, const char* file, int line)
	: Exception(desc, file, line)
{
}
