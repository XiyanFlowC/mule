#include <format>

#include "FileContainerBasic.h"
#include "xystring.h"
#include "xyutils.h"

xybase::FileContainerBasic::InnerStream::InnerStream(FileContainerBasic *host, unsigned long long fileHandle)
{
	this->host = host;
	this->fileHandle = fileHandle;
}

xybase::FileContainerBasic::InnerStream::~InnerStream()
{
}

void xybase::FileContainerBasic::InnerStream::Flush()
{
	host->Flush();
}

std::u16string xybase::FileContainerBasic::InnerStream::GetName() const
{
	return host->GetName() + host->openedFiles[fileHandle].baseEntry->path;
}

void xybase::FileContainerBasic::InnerStream::ReadBytes(char *buffer, size_t limit)
{
	host->ReadBytes(fileHandle, buffer, limit);
}

size_t xybase::FileContainerBasic::InnerStream::Tell() const
{
	return host->Tell(fileHandle);
}

void xybase::FileContainerBasic::InnerStream::Seek(long long offset, SeekMode mode)
{
	host->Seek(fileHandle, offset, mode);
}

void xybase::FileContainerBasic::InnerStream::Write(const char *buffer, size_t limit)
{
	host->Write(fileHandle, buffer, limit);
}

void xybase::FileContainerBasic::InnerStream::Close()
{
	host->Close(fileHandle);
}

bool xybase::FileContainerBasic::InnerStream::IsEof() const noexcept
{
	return host->openedFiles[fileHandle].cursor == host->openedFiles[fileHandle].size;
}

xybase::FileContainerBasic::FileContainerBasic(xybase::Stream *stream)
	: infraStream(stream), currentHandle(0), mode(-1)
{
}

xybase::FileContainerBasic::~FileContainerBasic()
{
	for (auto &&item : files)
	{
		delete item.second;
	}
}

std::u16string xybase::FileContainerBasic::GetName()
{
	return infraStream->GetName() + u":";
}

void xybase::FileContainerBasic::Flush()
{
	infraStream->Flush();
}

xybase::Stream *xybase::FileContainerBasic::Open(std::u16string name, FileOpenMode mode)
{
	if (!Exists(name))
	{
		if (!(mode & FOM_WRITE))
			throw InvalidOperationException(L"Specified file " + xybase::string::to_wstring(name) + L" does not exist, cannot open.", 102001);

		Fragment::Fragment frag{0, 0};
		try
		{
			 frag = freeSpaces.AllocMaximumFragment();
		}
		catch (xybase::InvalidOperationException &ex)
		{
			if (ex.GetErrorCode() == 32751)
				throw InvalidOperationException(std::format(L"Cannot create specified file [{}], fragments allocation failed.", xybase::string::to_wstring(name)), 102098);
			throw ex;
		}

		if (frag.GetSize() == 0)
			throw InvalidOperationException(L"Cannot create specified file since the free space was ran out.", 102099);

		unsigned long long handle = currentHandle++;

		FileEntry *fe = new FileEntry
		{
			.offset = frag.GetBeginning(),
			.size = frag.GetSize(),
			.path = name,
			.occupied = true,
		};
		files[name] = fe;

		auto ret = new InnerStream(this, handle);
		OpenedFileInformation stub
		{
			.cursor = 0,
			.size = 0,
			.capacity = fe->size,
			.stream = ret,
			.baseEntry = fe,
			.readable = false,
			.writable = true,
		};
		openedFiles[handle] = stub;

		return ret;
	}

	FileEntry *fe = files[name];

	if (fe->occupied) throw InvalidOperationException(L"File occupied.", 102299);

	unsigned long long handle = currentHandle++;
	auto ret = new InnerStream(this, handle);
	OpenedFileInformation stub
	{
		.cursor = 0,
		.size = fe->size,
		.capacity = XY_ALIGN(fe->size, align),
		.stream = ret,
		.baseEntry = fe,
		.readable = false,
		.writable = false,
	};

	if (mode & FOM_WRITE)
	{
		if (mode & FOM_TRUNCATE)
		{
			stub.size = 0;
		}
		if (mode & FOM_APPEND)
		{
			stub.cursor = stub.size;
		}
		stub.writable = true;
	}
	if (mode & FOM_READ)
	{
		stub.readable = true;
	}
	if (mode & FOM_EXCLUSIVE)
	{
		fe->occupied = true;
	}
	openedFiles[handle] = stub;

	return ret;
}

std::list<std::u16string> xybase::FileContainerBasic::List()
{
	std::list<std::u16string> ret;

	for (auto &&fe : files)
	{
		ret.push_back(fe.second->path);
	}

	return ret;
}

void xybase::FileContainerBasic::MakeDir(std::u16string path)
{
	// 留空不处理，文件包只处理文件单位
}

void xybase::FileContainerBasic::Remove(const std::u16string &path)
{
	auto const &itr = files.find(path);
	if (itr == files.end()) return;

	// TODO: 检查是否有文件访问正在进行中

	freeSpaces.RegisterFragment(itr->second->offset, itr->second->size);
	delete itr->second;
	files.erase(itr);
}

void xybase::FileContainerBasic::Remove(std::u16string path, bool recursive)
{
	if (recursive)
	{
		if (!path.ends_with(u'/'))
		{
			path += u'/';
		}

		for (auto &&pair : files)
		{
			if (pair.second->path.starts_with(path))
			{
				auto ptr = pair.second;
				Remove(pair.second->path);
			}
		}
	}
	else
	{
		Remove(path);
	}
}

void xybase::FileContainerBasic::Write(unsigned long long handle, const char *buffer, size_t limit)
{
	if (!openedFiles[handle].writable) throw xybase::InvalidOperationException(L"This inner stream cannot write.", 102501);
	/* 读写交织避免，Seek重设流状态 */
	if (!mode)
	{
		mode = 1;
		infraStream->Seek(0, Stream::SM_CURRENT);
		infraStream->Flush();
	}

	auto &target = openedFiles[handle];
	if (target.cursor + limit > target.capacity)
		throw InvalidOperationException(L"Too large to write. Exceeded the file capacity: " + xybase::string::itos<wchar_t>(target.capacity), 100000);

	if (infraStream->Tell() != target.cursor + target.baseEntry->offset)
		infraStream->Seek(target.cursor + target.baseEntry->offset, Stream::SM_BEGIN);
	infraStream->Write(buffer, limit);

	target.cursor += limit;
	target.size = XY_MAX(target.size, target.cursor);
}

void xybase::FileContainerBasic::ReadBytes(unsigned long long handle, char *buffer, size_t length)
{
	if (!openedFiles[handle].readable) throw xybase::InvalidOperationException(L"This inner stream cannot read.", 102500);
	/* 读写交织避免，Seek重设流状态 */
	if (mode)
	{
		mode = 0;
		infraStream->Seek(0, Stream::SM_CURRENT);
		infraStream->Flush();
	}

	auto &target = openedFiles[handle];
	if (target.cursor + length > target.size)
		throw InvalidOperationException(L"Too large to read. Exceeded the file size: " + xybase::string::itos<wchar_t>(target.capacity), 100001);

	if (infraStream->Tell() != target.cursor + target.baseEntry->offset)
		infraStream->Seek(target.cursor + target.baseEntry->offset, Stream::SM_BEGIN);
	infraStream->ReadBytes(buffer, length);

	target.cursor += length;
	target.size = XY_MAX(target.size, target.cursor);
}

void xybase::FileContainerBasic::Seek(unsigned long long handle, long long offset, Stream::SeekMode mode)
{
	auto &target = openedFiles[handle];
	switch (mode)
	{
	case xybase::Stream::SM_BEGIN:
		if (offset < 0) throw InvalidParameterException(L"offset", L"Cannot seek to the negative position.", 102035);
		if ((size_t)offset > target.capacity) throw InvalidOperationException(L"Target offset out of range.", 102030);
		target.cursor = offset;
		break;
	case xybase::Stream::SM_CURRENT:
		/* 读写交织避免用，向下级传递空寻址重置流状态 */
		if (offset == 0)
		{
			infraStream->Seek(0, Stream::SM_CURRENT);
			break;
		}

		if (target.cursor + offset > target.capacity
			|| target.cursor + offset < 0)
			throw InvalidOperationException(L"Target offset out of range.", 102030);
		target.cursor += offset;
		break;
	case xybase::Stream::SM_END:
		if (offset > 0) throw InvalidParameterException(L"offset", L"Cannot seek to the position that does not exist.", 102036);
		if (target.size + offset < 0) throw InvalidOperationException(L"Cannot seek to the negative position.", 102037);
		target.cursor = target.size + offset;
		break;
	default:
		throw InvalidParameterException(L"mode", L"Invalid enum!", 109099);
		break;
	}
}

size_t xybase::FileContainerBasic::Tell(unsigned long long handle)
{
	auto &target = openedFiles[handle];
	return target.cursor;
}

void xybase::FileContainerBasic::Close(unsigned long long handle)
{
	Flush();
	if (!openedFiles.contains(handle)) return;

	auto &target = openedFiles[handle];
	auto info = target.baseEntry;
	info->occupied = false;
	info->size = XY_ALIGN(target.size, align);
	// 计算并注册未使用完的剩余空间
	size_t release = target.capacity - info->size;
	freeSpaces.RegisterFragment(info->offset + info->size, release);

	openedFiles.erase(handle);
}

bool xybase::FileContainerBasic::Exists(std::u16string path)
{
	return files.contains(path);
}
