#include "mule.h"

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
	mule::Cpp::StructureBuilder *sb = new mule::Cpp::StructureBuilder("test");
	sb->AppendField("uint32", "offset");
	sb->AppendField("uint32", "size");
	Structure *test = sb->Build();
	Table *tbl = new Table(test, "test", 10, 0);

	xybase::Stream *stream;
	try
	{
		stream = new mule::Data::BinaryStream("test.bin");
		Mappifier m;
		tbl->Read(stream, &m);
		puts(m.GetMap().ToString().c_str());
		mule::Xml::XmlGenerator gen;
		puts(gen.ToXml(m.GetMap()).c_str());
	}
	catch (mule::Exception::Exception x)
	{
		puts( x.What() );
		abort();
	}

	return 0;
}
