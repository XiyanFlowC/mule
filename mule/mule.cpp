#include "mule.h"
#include "version.h"

using namespace mule::Lua;
using namespace mule::Data;
using namespace mule::Data::Basic;

void InitialiseLuaEnvironment();

int main(int argc, char **argv)
{
	if (argc <= 3)
	{
		std::cerr << "Mule by xiyan" << std::endl;
		std::cerr << "Ver." << MULE_MAJOR_VERSION << "." << MULE_MINOR_VERSION << " (Compile time: " << MULE_BUILD_TIME << ")" << std::endl;
		std::cerr << "Usage: " << argv[0] << " <path_to_execute_folder> <path_to_target_file> <action> [options]" << std::endl;
		std::cerr
			<< "Options:" << std::endl 
			<< "-l                  less log." << std::endl
			<< "-d<name>=<value>    define a variable." << std::endl;
		exit(-1);
	}
	// 设定为用户偏好的语言环境
	setlocale(LC_ALL, "");

	fwide(stderr, 1);
	fwide(stdin, 1);
	fwide(stdout, 1);

	// Configurate
	auto &&conf = mule::Configuration::GetInstance();
	conf.SetVariable(u"mule.target", xybase::string::to_utf16(argv[2]));
	auto basedir = xybase::string::to_utf16(argv[1]);
	if (!basedir.ends_with('/') || !basedir.ends_with('\\')) basedir += '/';
	conf.SetVariable(u"mule.basedir", basedir);
	conf.SetVariable(u"mule.data.basedir", conf.GetString(u"mule.basedir") + u"data/");
	conf.SetVariable(u"mule.resource.basedir", conf.GetString(u"mule.basedir") + u"resources/");
	conf.SetVariable(u"mule.sheet.basedir", conf.GetString(u"mule.basedir") + u"sheets/");
	conf.SetVariable(u"mule.script.basedir", conf.GetString(u"mule.resource.basedir") + u"scripts/");

	if (argc >= 5)
	{
		for (int i = 4; i < argc; ++i)
		{
			if (0 == strcmp("-l", argv[i]))
				mule::LoggerConfig::GetInstance().LoggerInit(2, stdout, stderr, true);
			if (0 == memcmp("-d", argv[i], 2))
			{
				std::string def(argv[i] + 2);
				auto defname = def.substr(0, def.find_first_of('='));
				auto defval = def.substr(def.find_first_of('=') + 1);
				mule::Configuration::GetInstance()
					.SetVariable(
						xybase::string::to_utf16(defname).c_str(),
						MultiValue::Parse(xybase::string::to_utf16(defval)));
			}
		}
	}

	// main 用 logger
	mule::Logger logger{ "<mule>", -1 };

	// Initialisation
	crc32_init();

	// Lua environment initialisation
	auto scriptBaseDir = conf.GetString(u"mule.script.basedir");
	LuaHost::GetInstance().LoadLuaStandardLibs();
	LuaHost::GetInstance().SetGlobal("package.path", scriptBaseDir + u"?.lua;" + scriptBaseDir + u"?/init.lua");
	LuaHost::GetInstance().SetGlobal("package.cpath", scriptBaseDir + u"?.dll;" + scriptBaseDir + u"dll/?.dll");

	// Set variable
	LuaHost::GetInstance().SetGlobal("mule", MultiValue{MultiValue::MVT_MAP});
	LuaHost::GetInstance().SetGlobal("mule.os", MultiValue{
#ifdef WIN32
		u"windows"
#else
		u"linux"
#endif // WIN32
		});

	mule::Lua::Api::RegisterContainerOperationFunctions();
	mule::Lua::Api::RegisterStreamOperationFunctions();
	mule::Lua::Api::RegisterSystemOperations();
	InitialiseLuaEnvironment();

	mule::Mule::GetInstance().LoadDescription(&mule::Cpp::bisDesc);

	mule::Lua::LuaEnvironment::GetInstance().SetStream(new xybase::BinaryStream(xybase::string::to_wstring(conf.GetString(u"mule.target"))));

	try
	{
		LuaHost::GetInstance().RunScript((xybase::string::to_string(conf.GetString(u"mule.script.basedir")) + "config.lua").c_str());
	}
	catch (LuaException &ex)
	{
		logger.Error(L"Failed when config system.");
		logger.Fatal(L"Lua config execute failed!");
		logger.Fatal(L"[{}] {}", ex.GetErrorCode(), ex.GetMessage());
		abort();
	}

	// Execution
	try
	{
		if (argv[3] == std::string{"interact"})
		{
			std::wcout << L"=====================================" << std::endl;
			std::wcout << L"Multiple-purpose User-configurable Large-file Editor" << std::endl;
			std::wcout << L"=====================================" << std::endl;
			std::wcout << "Ver." << MULE_MAJOR_VERSION << "." << MULE_MINOR_VERSION << " (Compile time: " << MULE_BUILD_TIME << ")" << std::endl;
			std::wcout << L"Interaction Mode[Lua based]" << std::endl;

			bool exList = false;
			int off = 7;
			while (true)
			{
				std::wcout << L"Lua> ";
				static char buffer[4096] {'r', 'e', 't', 'u', 'r', 'n', ' '};
				std::cin.getline(buffer + 7, 4096 - 8);
				if (strcmp(buffer + off, "quit") == 0) break;
				if (strcmp(buffer + off, "exlist") == 0)
				{
					exList ^= true;
					std::wcout << L"Extract list : " << (exList ? L"Enable" : L"Disable") << std::endl;
					continue;
				}
				try
				{
					auto && ret = LuaHost::GetInstance().RunString(buffer);
					if (ret.GetType() == MultiValue::MVT_ARRAY)
					{
						for (size_t i = 0; i < ret.GetLength(); ++i)
						{
							std::wcout << L" " << i << " => " << xybase::string::to_wstring(ret.value.arrayValue[i].ToString()).c_str() << std::endl;
						}
					}
					else if (exList)
					{
						if (ret.GetType() == MultiValue::MVT_MAP)
						{
							auto &&it = ret.value.mapValue->find(1);
							if (it != ret.value.mapValue->end())
							{
								std::wcout << L"  => " << xybase::string::to_wstring(it->second.ToString()) << std::endl;
								for (size_t i = 2; i <= ret.value.mapValue->size(); ++i)
								{
									it = ret.value.mapValue->find((int)i);
									if (it == ret.value.mapValue->end()) break;
									std::wcout << L"  -> " << xybase::string::to_wstring(it->second.ToString()) << std::endl;
								}
								continue;
							}
						}
						std::wcout << "  => " << xybase::string::to_wstring(ret.ToString()) << std::endl;
					}
					else
						std::wcout << "  => " << xybase::string::to_wstring(ret.ToString()) << std::endl;
				}
				catch (LuaException &ex)
				{
					std::wcerr << L"Lua Error: " << ex.what() << std::endl << std::endl;
				}
				catch (xybase::Exception &ex)
				{
					std::wcerr << L"Error when executing " << buffer << L":\n" << ex.what() << L"\n\n";
				}
				LuaHost::GetInstance().SetStackTop(0);
			}
			puts("Bye!");
		}
		else
		{
			LuaHost::GetInstance().RunScript((xybase::string::to_string(conf.GetString(u"mule.script.basedir")) + argv[3] + ".lua").c_str());
		}
	}
	catch (mule::Lua::LuaException &ex)
	{
		logger.Error(L"Error when execute lua script. {}", ex.GetMessage());
		return -1;
	}

	return 0;
}
