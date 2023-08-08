/* FIXME: Reimplement all of these functions with luahost!!! */
/* This file is for test only!!! */
#include "mule.h"

using namespace mule::Data::Basic;
using namespace mule::Data;

std::map<int, xybase::Stream *> streams;
int streamd = 0;
std::map<int, MultiValue> values;
int valued = 0;
std::stack<xybase::FileContainer *> containerStack;
std::list<Table *> tables;

void setRootStream(xybase::Stream *stream)
{
	streams[streamd++] = stream;
}

int applyContainer(lua_State *L)
{
	if (!lua_isstring(L, -1) || !lua_isinteger(L, -2))
	{
		lua_pushinteger(L, -1);
		return 1;
	}
	std::string x {lua_tostring(L, -1)};
	int id = lua_tointeger(L, -2);
	lua_pop(L, 2);

	auto &&stream = streams.find(id);
	if (stream == streams.end())
	{
		lua_pushinteger(L, -2);
		return 1;
	}

	if (x == "iso")
	{
		containerStack.push(new IsoContainer(stream->second));
	}
	else
	{
		lua_pushinteger(L, -3);
		return 1;
	}

	lua_pushinteger(L, 0);
	return 1;
}

int popContainer(lua_State *L)
{
	lua_pushinteger(L, containerStack.empty() ? -1 : 0);
	if (!containerStack.empty()) containerStack.pop();
	return 1;
}

int openFile(lua_State *L)
{
	if (containerStack.empty() || !lua_isstring(L, -1))
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	std::string name {lua_tostring(L, -1)};

	lua_pop(L, 1);

	auto ret = containerStack.top()->Open(name, xybase::FileContainer::FOM_READ_WRITE);
	if (ret == nullptr)
	{
		lua_pushinteger(L, -1);
		return 1;
	}
	streams[streamd] = ret;

	lua_pushinteger(L, streamd++);
	return 1;
}

int closeFile(lua_State *L)
{
	if (containerStack.empty())
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	int fd = lua_tointeger(L, -1);

	lua_pop(L, 1);

	auto &&itr = streams.find(fd);
	if (itr == streams.end())
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	itr->second->Close();
	streams.erase(itr);

	lua_pushinteger(L, 0);
	return 1;
}

int loadDefine(lua_State *L)
{
	if (!lua_isstring(L, -1))
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	std::string define {lua_tostring(L, -1)};

	lua_pop(L, 1);

	try
	{
		ResourceManager::GetInstance().LoadDefinition(define);
	}
	catch (mule::Exception::Exception &ex)
	{
		fprintf(stderr, "[EX] loadDefine: %s\n", ex.what());
		lua_pushinteger(L, -2);
		return 1;
	}

	lua_pushinteger(L, 0);
	return 1;
}

int applyTable(lua_State *L)
{
	// int - fd, string - typename, int - offset, int - size
	if (!lua_isinteger(L, -1) || !lua_isinteger(L, -2) || !lua_isstring(L, -3) || !lua_isinteger(L, -4))
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	int size = lua_tointeger(L, -1);
	size_t offset = lua_tointeger(L, -2);
	std::string name(lua_tostring(L, -3));
	int fd = lua_tointeger(L, -4);

	lua_pop(L, 4);

	auto &&itr = streams.find(fd);
	if (itr == streams.end())
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	auto structure = TypeManager::GetInstance().GetObject(name);
	if (structure == nullptr || dynamic_cast<Structure *>(structure) == nullptr)
	{
		lua_pushinteger(L, -2);
		return 1;
	}

	Table tbl(dynamic_cast<Structure *>(structure), "", size, offset);
	Mappifier *mp = new Mappifier();
	tbl.Read(itr->second, mp);

	values[valued] = mp->GetMap();

	delete mp;
	lua_pushinteger(L, valued++);
	return 1;
}

mule::Xml::XmlGenerator<mule::Xml::MvXmlNode> generator;

int saveSheet(lua_State *L)
{
	if (!lua_isstring(L, -1) || !lua_isinteger(L, -2))
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	int vd = lua_tointeger(L, -2);
	std::string sheetName{lua_tostring(L, -1)};

	lua_pop(L, 2);

	auto &&itr = values.find(vd);
	if (itr == values.end())
	{
		lua_pushinteger(L, -2);
		return 1;
	}
	std::string xml = generator.ToXml(mule::Xml::MvXmlNode(sheetName, itr->second));
	ResourceManager::GetInstance().SaveSheet(sheetName, xml);

	lua_pushinteger(L, 0);
	return 1;
}

mule::Xml::XmlParser<mule::Xml::MvXmlNode> sheetParser;

int loadSheet(lua_State *L)
{
	if (!lua_isstring(L, -1))
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	std::string name {lua_tostring(L, -1)};

	lua_pop(L, 1);

	auto ret = ResourceManager::GetInstance().LoadSheet(name);
	values[valued] = sheetParser.Parse(ret).mv;
	if (!sheetParser.error.empty())
	{
		fputs(sheetParser.error.c_str(), stderr);
	}

	lua_pushinteger(L, valued++);
	return 1;
}

int getSheet(lua_State *L)
{
	if (!lua_isinteger(L, -1))
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	int vd = lua_tointeger(L, -1);

	lua_pop(L, 1);

	auto &&itr = values.find(vd);
	if (itr == values.end())
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	mule::Lua::LuaHost::GetInstance().PushValue(itr->second);

	//lua_pushinteger(L, 0);
	return 1;
}

int list(lua_State *L)
{
	auto &&lst = containerStack.top()->List();

	MultiValue mv { MultiValue::MVT_MAP };

	int i = 1;
	for (auto &&item : lst)
	{
		(*mv.value.mapValue)[MultiValue((long long)(i++))] = MultiValue(item);
	}

	mule::Lua::LuaHost::GetInstance().PushValue(mv);

	//lua_pushinteger(L, 0);
	return 1;
}

int applyStream(lua_State *L)
{
	if (!lua_isstring(L, -1) || !lua_isinteger(L, -2))
	{
		lua_pushinteger(L, -1);
		return 1;
	}
	std::string x {lua_tostring(L, -1)};
	int id = lua_tointeger(L, -2);
	lua_pop(L, 2);

	auto &&stream = streams.find(id);
	if (stream == streams.end())
	{
		lua_pushinteger(L, -2);
		return 1;
	}

	if (x == "elf")
	{
		streams[streamd] = new ElfStream(stream->second);
		lua_pushinteger(L, streamd++);
	}
	else
	{
		lua_pushinteger(L, -3);
	}
	return 1;
}

luaL_Reg mulefuncs[] = {
	{"applyfs", applyContainer},
	{"popfs", popContainer},
	{"open", openFile},
	{"close", closeFile},
	{"loaddef", loadDefine},
	{"applytbl", applyTable},
	{"savesheet", saveSheet},
	{"loadsheet", loadSheet},
	{"getsheet", getSheet},
	{"ls", list},
	{"applyfile", applyStream},
	{NULL, NULL},
};