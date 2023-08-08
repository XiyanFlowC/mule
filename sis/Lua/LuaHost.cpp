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
	while (libRegTable->func != NULL)
	{
		RegisterFunction(libRegTable->name, libRegTable->func);
		++libRegTable;
	}
}

mule::Data::Basic::MultiValue mule::Lua::LuaHost::GetGlobal(const std::string &name)
{
	int top = GetStackTop();
	// ���Ǳ�
	size_t dotIndex = name.find_first_of('.'), startIndex = 0;
	if (dotIndex != std::string::npos)
	{
		lua_getglobal(L, name.substr(startIndex, dotIndex - startIndex).c_str());
		if (lua_isnil(L, -1)) throw LuaException("Invalid table " + name.substr(startIndex, dotIndex - startIndex), __FILE__, __LINE__);
		startIndex = dotIndex + 1;
		dotIndex = name.find_first_of('.', startIndex);
		while (dotIndex != std::string::npos)
		{
			lua_getfield(L, -1, name.substr(startIndex, dotIndex - startIndex).c_str());
			if (lua_isnil(L, -1)) throw LuaException("Invalid table " + name.substr(startIndex, dotIndex - startIndex), __FILE__, __LINE__);
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
		ret = MultiValue(lua_tostring(L, idx));
	else if (lua_isnil(L, idx))
		ret = MultiValue();
	else if (lua_istable(L, idx))
	{
		std::map<MultiValue, MultiValue> result;

		// ����LuaTable
		lua_pushnil(L);  // ��nil����ջ����Ϊ��������ʼ��
		while (lua_next(L, idx) != 0) {
			// ��ȡkey��value
			const MultiValue key = GetValue(-2);
			const MultiValue value = GetValue(-1);

			// ��key��value����std::map
			result[key] = value;

			// ����value������key������һ�ε���
			lua_pop(L, 1);
		}

		ret.SetValue(result);
	}
	else throw LuaException("Unhandlable value.", __FILE__, __LINE__);
	return ret;
}

void mule::Lua::LuaHost::SetGlobal(const std::string &name, const mule::Data::Basic::MultiValue &value)
{
	int top = GetStackTop();
	// ���Ǳ�
	size_t dotIndex = name.find_first_of('.'), startIndex = 0;
	if (dotIndex != std::string::npos)
	{
		lua_getglobal(L, name.substr(startIndex, dotIndex - startIndex).c_str());
		if (lua_isnil(L, -1)) throw LuaException("Invalid table " + name.substr(startIndex, dotIndex - startIndex), __FILE__, __LINE__);
		startIndex = dotIndex + 1;
		dotIndex = name.find_first_of('.', startIndex);
		while (dotIndex != std::string::npos)
		{
			lua_getfield(L, -1, name.substr(startIndex, dotIndex - startIndex).c_str());
			if (lua_isnil(L, -1)) throw LuaException("Invalid table " + name.substr(startIndex, dotIndex - startIndex), __FILE__, __LINE__);
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
