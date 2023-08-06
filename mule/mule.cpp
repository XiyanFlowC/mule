#include "mule.h"

using namespace mule::Lua;
using namespace mule::Data;
using namespace mule::Data::Basic;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <path_to_execute_folder> <target.bin> [args...]", argv[0]);
		exit(-1);
	}

	Configuration::GetInstance().SetEnvironmentRootPath(argv[1]);
	crc32_init();

	LuaHost::GetInstance().SetGlobal("package.path", MultiValue(Configuration::GetInstance().ScriptsDir + "?.lua;" + Configuration::GetInstance().ScriptsDir + "?/init.lua"));

	LuaHost::GetInstance().LoadLuaStandardLibs();
	LuaHost::GetInstance().RunScript((Configuration::GetInstance().ScriptsDir + "config.lua").c_str());
	try
	{
		LuaHost::GetInstance().RunScript(argv[1]);
	}
	catch (mule::Lua::LuaException ex)
	{
		fputs("Error when execute lua script.\n", stderr);
		fputs(ex.what(), stderr);
	}

	TypeManager::GetInstance().RegisterObjectCreator(new BasicFieldCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new Referrence::ReferrenceCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new Array::ArrayCreator());

	ResourceManager::GetInstance().LoadDefinition("test");

	Table *tbl = new Table((Structure *)TypeManager::GetInstance().GetObject("FileEntry"), "test", 5, 0);

	mule::Xml::XmlParser<mule::Xml::XmlNode> configParser;

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
		puts("Re-parsed:");
		gen.indent = 0;
		mule::Xml::XmlParser<mule::Xml::MvXmlNode> parser;
		puts(gen.ToXml(mule::Xml::MvXmlNode(std::string("root"), parser.Parse(xml).mv)).c_str());
		puts(parser.error.c_str());
	}
	catch (mule::Exception::Exception x)
	{
		puts( x.What() );
		abort();
	}

	return 0;
}
