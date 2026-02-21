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
	
	// 验证路径存在性和类型
	std::filesystem::path fsPath(rootPath);
	if (!std::filesystem::exists(fsPath))
		throw InvalidParameterException(L"rootPath", L"Path " + xybase::string::to_wstring(rootPath) + L" does not exist.", 0xAF10);
	
	if (!std::filesystem::is_directory(fsPath))
		throw InvalidParameterException(L"rootPath", L"Path " + xybase::string::to_wstring(rootPath) + L" is not a directory.", 0xAF11);

	this->rootPath = rootPath;
}

xybase::HostFsMapper::~HostFsMapper()
{
}

std::u16string xybase::HostFsMapper::GetName()
{
	return rootPath + u"{mapper}:";
}

void xybase::HostFsMapper::Flush()
{
	// intend to be blank
}

xybase::Stream *xybase::HostFsMapper::Open(std::u16string name, FileOpenMode mode)
{
	if (name.find(u"..") != std::u16string::npos)
		throw InvalidParameterException(L"name", L"Invalid path: no .. permitted.", 95964);

	// 组合出完整路径
	std::filesystem::path fullPath{rootPath + name};

	// 规范化并检查是否仍在根目录下，防止通过复杂相对路径跳出根目录
	try
	{
		fullPath = std::filesystem::weakly_canonical(fullPath);
		std::filesystem::path rootCanonical = std::filesystem::weakly_canonical(std::filesystem::path(rootPath));

		auto [rootIt, pathIt] = std::mismatch(rootCanonical.begin(), rootCanonical.end(), fullPath.begin());
		if (rootIt != rootCanonical.end())
			throw InvalidParameterException(L"name", L"Invalid path: outside root directory.", 95966);
	}
	catch (const std::filesystem::filesystem_error &)
	{
		throw InvalidParameterException(L"name", L"Invalid path: cannot be canonicalized.", 95967);
	}
	
	wchar_t modebuf[8];
	memset(modebuf, 0, sizeof(modebuf));
	wchar_t *ptr = modebuf;

	// 修复文件打开模式逻辑
	if (mode & FOM_READ && mode & FOM_WRITE)
	{
		if (mode & FOM_APPEND)
		{
			*ptr++ = 'a';
			*ptr++ = 'b';
			*ptr++ = '+';
		}
		else if (mode & FOM_TRUNCATE)  // 修复：添加 else if
		{
			*ptr++ = 'w';
			*ptr++ = 'b';
			*ptr++ = '+';
		}
		else  // 既不是 APPEND 也不是 TRUNCATE
		{
			*ptr++ = 'r';
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

	return new BinaryStream(xybase::string::to_wstring(rootPath + name), modebuf, mode & FOM_BIG_ENDIAN);
}

std::list<std::u16string> xybase::HostFsMapper::List()
{
	std::list<std::u16string> ret;

	std::filesystem::path root(rootPath);
	std::filesystem::directory_entry rootDir(root);

	// 添加错误处理和相对路径转换
	try {
		for (auto const &itr : std::filesystem::recursive_directory_iterator(
			root, 
			std::filesystem::directory_options::skip_permission_denied))  // 跳过权限拒绝的目录
		{
			// 跳过非文件项（例如目录本身）
			if (!itr.is_regular_file()) continue;
			
			// 转换为相对路径
			std::filesystem::path relativePath = std::filesystem::relative(itr.path(), root);
			ret.push_back(xybase::string::to_utf16(relativePath.generic_u16string()));
		}
	} catch (const std::filesystem::filesystem_error &e) {
		// 记录错误但不中断整个列表操作
		// 如果需要，可以在这里添加日志记录
	}

	return ret;
}

void xybase::HostFsMapper::MakeDir(std::u16string path)
{
	if (path.find(u"..") != std::u16string::npos)
		throw InvalidParameterException(L"path", L"Invalid path: no .. permitted.", 95964);

	std::filesystem::path fullPath{ rootPath + path };
	try
	{
		fullPath = std::filesystem::weakly_canonical(fullPath);
		std::filesystem::path rootCanonical = std::filesystem::weakly_canonical(std::filesystem::path(rootPath));

		auto [rootIt, pathIt] = std::mismatch(rootCanonical.begin(), rootCanonical.end(), fullPath.begin());
		if (rootIt != rootCanonical.end())
			throw InvalidParameterException(L"path", L"Invalid path: outside root directory.", 95966);
	}
	catch (const std::filesystem::filesystem_error &)
	{
		throw InvalidParameterException(L"path", L"Invalid path: cannot be canonicalized.", 95967);
	}

	std::filesystem::create_directory(fullPath);
}

void xybase::HostFsMapper::Remove(std::u16string path, bool recursive)
{
	if (path.find(u"..") != std::u16string::npos)
		throw InvalidParameterException(L"path", L"Invalid path: no .. permitted.", 95964);

	std::filesystem::path fullPath{ rootPath + path };
	try
	{
		fullPath = std::filesystem::weakly_canonical(fullPath);
		std::filesystem::path rootCanonical = std::filesystem::weakly_canonical(std::filesystem::path(rootPath));

		auto [rootIt, pathIt] = std::mismatch(rootCanonical.begin(), rootCanonical.end(), fullPath.begin());
		if (rootIt != rootCanonical.end())
			throw InvalidParameterException(L"path", L"Invalid path: outside root directory.", 95966);
	}
	catch (const std::filesystem::filesystem_error &)
	{
		throw InvalidParameterException(L"path", L"Invalid path: cannot be canonicalized.", 95967);
	}

	if (recursive)
		std::filesystem::remove_all(fullPath);
	else
		std::filesystem::remove(fullPath);
}

bool xybase::HostFsMapper::Exists(std::u16string path)
{
	return std::filesystem::exists(rootPath + path);
}
