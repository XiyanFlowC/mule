#include "DataManager.h"

#include <xystring.h>
#include <xyutils.h>
#include <Exception/IOException.h>
#include "../Configuration.h"

using namespace mule::Storage;

int DataManager::CreateDirectoryRecursively(std::string path, size_t index)
{
	size_t endIndex = path.find_first_of('/', index);

	if (endIndex == std::string::npos)
	{
		if (xybase::io::access(path.c_str(), xybase::io::PM_READ | xybase::io::PM_WRITE))
		{
			return xybase::io::mkdir(path.c_str());
		}
	}

	std::string curPath = path.substr(0, endIndex);
	if (xybase::io::access(curPath.c_str(), xybase::io::PM_READ | xybase::io::PM_WRITE))
	{
		xybase::io::mkdir(curPath.c_str());
	}
	return CreateDirectoryRecursively(path, endIndex + 1);
}

DataManager &mule::Storage::DataManager::GetInstance()
{
	static DataManager _inst{};
	return _inst;
}

mule::Storage::DataManager::~DataManager()
{
	auto f = OpenRaw(0, true);
	fprintf(f, "%u\n", currentId);
	fclose(f);
}

void mule::Storage::DataManager::Initialise(const std::u16string &datadir)
{
	if (xybase::io::access(xybase::string::to_string(datadir).c_str(), xybase::io::PM_READ | xybase::io::PM_WRITE))
		throw xybase::InvalidParameterException(L"datadir", L"Inaccessible path.", 100);
	dataPath = datadir;

	auto data = OpenRaw(0);
	if (data == nullptr) return;

	int ret = fscanf(data, "%u", &currentId);
	// 读取其他数据（目前不处理）
	fclose(data);
}

BinaryData DataManager::LoadData(std::string name)
{
	auto &&it = fileInfos.find(name);
	if (it == fileInfos.end())
	{
		throw xybase::RuntimeException(L"Unkown id for data " + xybase::string::to_wstring(name), __LINE__);
	}
	return LoadData(it->second.id);
}

BinaryData DataManager::LoadData(unsigned int id)
{
	char path[32];
	sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);

	FILE *f = fopen((xybase::string::to_string(dataPath) + path).c_str(), "rb");
	if (f == NULL) throw xybase::IOException(xybase::string::to_wstring(path), L"Unable to open data file.");

	fseek(f, 0, SEEK_END);
	size_t length = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buffer = new char[length];
	fread(buffer, length, 1, f);
	fclose(f);

	return BinaryData(buffer, length, false);
}

FILE *mule::Storage::DataManager::OpenRaw(unsigned int id, bool create)
{
	char path[32];
	sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
	std::string pp = xybase::string::to_string(dataPath) + path;
	if (xybase::io::access(pp.substr(0, pp.find_last_of('/')).c_str(), xybase::io::PM_READ | xybase::io::PM_WRITE)) CreateDirectoryRecursively(pp.substr(0, pp.find_last_of('/')));

	FILE *f = fopen(pp.c_str(), create ? "wb" : "rb");

	return f;
}

mule::Storage::DataManager::DataManager()
{
	Initialise(Configuration::GetInstance().GetString(u"mule.data.basedir"));
}

bool DataManager::IsExist(unsigned int id)
{
	char path[32];
	sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);

	FILE *f = fopen((xybase::string::to_string(dataPath) + path).c_str(), "rb");
	if (f != NULL)
	{
		fclose(f);
		return true;
	}
	return false;
}

unsigned int DataManager::SaveData(const BinaryData &data, unsigned int id)
{
	char path[32];

	sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
	std::string pp = xybase::string::to_string(dataPath) + path;
	if (xybase::io::access(pp.substr(0, pp.find_last_of('/')).c_str(), xybase::io::PM_READ | xybase::io::PM_WRITE)) CreateDirectoryRecursively(pp.substr(0, pp.find_last_of('/')));
	FILE *f = fopen(pp.c_str(), "wb");
	if (f == NULL)
	{
		throw xybase::IOException(xybase::string::to_wstring(path), L"Unable to open file to write ");
	}

	fwrite(data.GetData(), data.GetLength(), 1, f);
	fclose(f);

	return id;
}

unsigned int mule::Storage::DataManager::SaveData(const BinaryData &data)
{
	while (IsExist(currentId)) currentId++;

	return SaveData(data, currentId++);
}
