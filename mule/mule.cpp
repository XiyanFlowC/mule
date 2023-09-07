#include "mule.h"

using namespace mule::Lua;
using namespace mule::Data;
using namespace mule::Data::Basic;

void InitialiseLuaEnvironment(xybase::Stream *);

int main(int argc, char **argv)
{
	if (argc <= 3)
	{
		fprintf(stderr, "Usage: %s <path_to_execute_folder> <path_to_target_file(s)> <action> [options...]", argv[0]);
		exit(-1);
	}

	// Initialisation
	Configuration::GetInstance().SetEnvironmentRootPath(argv[1]);
	Configuration::GetInstance().TargetFile = argv[2];
	crc32_init();

	// Lua environment initialisation
	LuaHost::GetInstance().LoadLuaStandardLibs();
	LuaHost::GetInstance().SetGlobal("package.path", MultiValue(xybase::string::to_utf16(Configuration::GetInstance().ScriptsDir + "?.lua;" + Configuration::GetInstance().ScriptsDir + "?/init.lua")));
	LuaHost::GetInstance().SetGlobal("package.cpath", MultiValue(xybase::string::to_utf16(Configuration::GetInstance().ScriptsDir + "?.dll;" + Configuration::GetInstance().ScriptsDir + "dll/?.dll")));

	LuaHost::GetInstance().RunScript((Configuration::GetInstance().ScriptsDir + "config.lua").c_str());

	TypeManager::GetInstance().RegisterObjectCreator(new ShiftableReferrence::ShiftableReferrenceCreator());

	mule::Cpp::Environment::GetInstance().LoadDescription(&mule::Cpp::bisDesc);

	mule::Xml::XmlParser<mule::Xml::XmlNode> xmlParser;

	InitialiseLuaEnvironment(new xybase::BinaryStream(xybase::string::to_utf16(Configuration::GetInstance().TargetFile.c_str())));

	// setlocale(LC_ALL, "ja-JP");
	// setlocale(LC_ALL, "zh-CN");
	// Execution
	try
	{
		if (argv[3] == std::string{"interact"})
		{
			puts("=====================================");
			//puts("MULE who f**ked ArTonelico");
			//puts("MULE User-configurable Large-file Editor");
			//puts("MULE is a Utility for Limited Editing");
			puts("Multiple-purpose User-configurable Large-file Editor");
			puts("=====================================");
			puts("Interaction Mode [Lua based]");

			bool exList = false;
			int off = 7;
			while (true)
			{
				printf("Lua> ");
				static char buffer[4096] {'r', 'e', 't', 'u', 'r', 'n', ' '};
				scanf("%[^\n]", buffer + off);
				getchar();
				if (strcmp(buffer + off, "quit") == 0) break;
				if (strcmp(buffer + off, "exlist") == 0)
				{
					exList ^= true;
					std::cout << "Extract list : " << (exList ? "Enable" : "Disable") << std::endl;
					continue;
				}
				try
				{
					auto && ret = LuaHost::GetInstance().RunString(buffer);
					if (ret.GetType() == MultiValue::MVT_ARRAY)
					{
						for (size_t i = 0; i < ret.GetLength(); ++i)
						{
							printf(" %zu => %s\n", i, xybase::string::to_string(ret.value.arrayValue[i].ToString()).c_str());
						}
					}
					else if (exList)
					{
						if (ret.GetType() == MultiValue::MVT_MAP)
						{
							auto &&it = ret.value.mapValue->find(1);
							if (it != ret.value.mapValue->end())
							{
								std::cout << "  => " << xybase::string::to_string(it->second.ToString()) << std::endl;
								for (size_t i = 2; i <= ret.value.mapValue->size(); ++i)
								{
									it = ret.value.mapValue->find((int)i);
									if (it == ret.value.mapValue->end()) break;
									std::cout << "  -> " << xybase::string::to_string(it->second.ToString()) << std::endl;
								}
								continue;
							}
						}
						std::cout << "  => " << xybase::string::to_string(ret.ToString()) << std::endl;
					}
					else
						std::cout << "  => " << xybase::string::to_string(ret.ToString()) << std::endl;
				}
				catch (LuaException &ex)
				{
					fprintf(stderr, "Lua Error: %s\n\n", ex.what());
				}
				catch (xybase::Exception &ex)
				{
					fprintf(stderr, "Error when executing %s:\n%s\n\n", buffer, ex.what());
				}
				LuaHost::GetInstance().SetStackTop(0);
			}
			puts("Bye!");
		}
		else
		{
			LuaHost::GetInstance().RunScript((Configuration::GetInstance().ScriptsDir + argv[3] + ".lua").c_str());
		}
	}
	catch (mule::Lua::LuaException &ex)
	{
		fputs("Error when execute lua script.\n", stderr);
		fputs(ex.what(), stderr);
	}

	return 0;
}
