#include "mule.h"

#include <Exception/Exception.h>

using namespace mule::Lua;
using namespace mule::Data;
using namespace mule::Data::Basic;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s [script.lua] [target.bin]", argv[0]);
		exit(-1);
	}

	LuaHost::GetInstance().LoadLuaStandardLibs();
	LuaHost::GetInstance().RunScript((Configuration::GetInstance().ScriptDir + "config.lua").c_str());
	try
	{
		LuaHost::GetInstance().RunScript(argv[1]);
	}
	catch (mule::Lua::LuaException ex)
	{
		fputs("Error when execute lua script.\n", stderr);
		fputs(ex.what(), stderr);
	}

	// Test:
	ObjectManager::GetInstance().RegisterObjectCreator(new BasicFieldCreator());
	Structure *test = ObjectManager::GetInstance().NewStructure("test");
	std::string info = "uint32";
	test->AppendField("offset", ObjectManager::GetInstance().GetOrCreateObject(info));
	test->AppendField("size", ObjectManager::GetInstance().GetOrCreateObject(info));
	Table *tbl = new Table(test, "test", 10, 0);

	xybase::Stream *stream;
	try
	{
		stream = new mule::Data::BinaryStream("test.bin");
		Mappifier m;
		tbl->Read(stream, &m);
		puts(m.GetMap().ToString().c_str());
	}
	catch (mule::Exception::Exception x)
	{
		puts( x.What() );
		abort();
	}

	return 0;
}
