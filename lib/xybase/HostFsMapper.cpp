#include "HostFsMapper.h"
#include "BinaryStream.h"

#include <cstdio>
#include <filesystem>
#include <cstring>
#include "Exception/InvalidParameterException.h"
#include "xystring.h"

xybase::HostFsMapper::HostFsMapper(std::u16string rootPath)
{
	if (!rootPath.ends_with('/') && !rootPath.ends_with('\\')) rootPath += u'/';
	if (!std::filesystem::exists(rootPath))
		throw InvalidParameterException(L"rootPath", L"Path " + xybase::string::to_wstring(rootPath) + L" does not exist.", 0xAF10);

	this->rootPath = rootPath;
}

std::u16string xybase::HostFsMapper::GetName()
{
	return std::u16string(xybase::string::to_utf16(std::to_string((unsigned long long)this)));
}

void xybase::HostFsMapper::Flush()
{
	// intend to be blank
}

xybase::Stream *xybase::HostFsMapper::Open(std::u16string name, FileOpenMode mode)
{
	wchar_t modebuf[8];
	memset(modebuf, 0, sizeof(modebuf));
	wchar_t *ptr = modebuf;

	if (mode & FOM_READ && mode & FOM_WRITE)
	{
		if (mode & FOM_APPEND)
		{
			*ptr++ = 'r';
			*ptr++ = 'b';
			*ptr++ = '+';
		}
		if (mode & FOM_TRUNCATE)
		{
			*ptr++ = 'w';
			*ptr++ = 'b';
			*ptr++ = '+';
		}
	}
	else
	{
		if (mode & FOM_READ) *ptr++ = 'r';
		if (mode & FOM_WRITE) *ptr++ = 'w';
		*ptr++ = 'b';
		if (mode & FOM_APPEND) *ptr++ = 'a';
		else if (mode & FOM_TRUNCATE);
		else *ptr++ = '+';
	}

	if (mode & FOM_EXCLUSIVE);

	return new BinaryStream(xybase::string::to_wstring(rootPath + name), modebuf, false);
}

std::list<std::u16string> xybase::HostFsMapper::List()
{
	std::list<std::u16string> ret;

	std::filesystem::path root(rootPath);
	std::filesystem::directory_entry rootDir(root);

	for (auto const &itr : std::filesystem::recursive_directory_iterator(root))
	{
		ret.push_back(xybase::string::to_utf16(itr.path()));
	}

	return ret;
}

void xybase::HostFsMapper::MakeDir(std::u16string path)
{
	std::filesystem::path root(rootPath);
	std::filesystem::create_directory(root/path);
}

void xybase::HostFsMapper::Remove(std::u16string path, bool recursive)
{
	std::filesystem::path root(rootPath);
	if (recursive)
		std::filesystem::remove(root/path);
	else
		std::filesystem::remove_all(root/path);
}