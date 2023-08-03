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
	ObjectManager::GetInstance().RegisterObjectCreator(new Referrence::ReferrenceObjectCreator());
	mule::Cpp::StructureBuilder *sb = new mule::Cpp::StructureBuilder("test");
	sb->AppendField("uint32", "offset");
	sb->AppendField("uint32", "size");
	sb->AppendField("string*", "name");
	sb->AppendField("uint32", "crc32");
	Structure *test = sb->Build();
	Table *tbl = new Table(test, "test", 5, 0);

	xybase::Stream *stream;
	try
	{
		stream = new mule::Data::BinaryStream("test.bin");
		Mappifier m;
		tbl->Read(stream, &m);
		puts("Read:");
		puts(m.GetMap().ToString().c_str());
		mule::Xml::XmlGenerator<mule::Xml::MvXmlNode> gen;
		gen.indent = 2;
		std::string xml = gen.ToXml(mule::Xml::MvXmlNode(std::string("root"), m.GetMap()));
		puts("Xml:");
		puts(xml.c_str());
		mule::Xml::XmlParser<mule::Xml::MvXmlNode> parser;
		puts("Re-parsed:");
		puts(parser.Parse(xml).mv.ToString().c_str());
		puts(parser.error.c_str());
	}
	catch (mule::Exception::Exception x)
	{
		puts( x.What() );
		abort();
	}

	return 0;
}
