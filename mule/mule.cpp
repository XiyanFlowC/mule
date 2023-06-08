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
	ObjectManager::GetInstance().RegisterFieldCreator(new BasicFieldCreator());
	Structure *test = ObjectManager::GetInstance().NewStructure("test");
	FieldCreator::FieldCreatingInfo info;
	info.typeDescriptor = "uint32";
	test->AppendField("offset", ObjectManager::GetInstance().CreateField(info));
	test->AppendField("size", ObjectManager::GetInstance().CreateField(info));

	xybase::Stream *stream = new mule::Data::BinaryStream("test.bin");

	Table *tbl = new Table(test, "table", 10, 0);

	MultiValue *mv = new MultiValue();
	tbl->Read(stream, new Stringfier(mv));
	puts(mv->ToString().c_str());


	return 0;
}
