#include "BasicContainer.h"

#include <Exception/NotImplementedException.h>

using namespace mule;

void mule::BasicContainer::ReadBytes(int id, char *buffer, size_t size)
{
	if (state == BC_WRITE)
	{
		file->Flush();
	}
	state = BC_READ;

	if (locs[id] + size > fdMap[id]->curSize)
		throw xybase::OutOfRangeException(L"Try to read after EOF.", __LINE__);
	if (file->Tell() != locs[id] + fdMap[id]->offset)
	{
		file->Seek(locs[id] + fdMap[id]->offset, xybase::Stream::SM_BEGIN);
	}
	file->ReadBytes(buffer, size);
	locs[id] += size;
}

void mule::BasicContainer::Write(int id, const char *buffer, size_t size)
{
	if (state == BC_READ)
	{
		file->Seek(0, xybase::Stream::SM_CURRENT);
	}
	state = BC_WRITE;

	if (locs[id] + size > fdMap[id]->size)
		throw xybase::OutOfRangeException(L"Write exceed maximum size.", __LINE__);
	if (file->Tell() != locs[id] + fdMap[id]->offset)
	{
		file->Seek(locs[id] + fdMap[id]->offset, xybase::Stream::SM_BEGIN);
	}
	file->Write(buffer, size);
	locs[id] += size;
	fdMap[id]->curSize = std::max(fdMap[id]->curSize, locs[id]);
}

void mule::BasicContainer::Close(int id)
{
	if (fdMap[id]->curSize != fdMap[id]->size)
	{
		fdMap[id]->size = fdMap[id]->curSize;
	}

	fdMap[id]->occupied = false;
}

BasicContainer::BasicContainer(xybase::Stream * stream)
	: state{BC_IDLE}
{
	file = stream;
	name = stream->GetName() + u":";
}

xybase::Stream *BasicContainer::Open(std::u16string name, xybase::FileOpenMode mode)
{
	auto &&it = fileIndices.find(name);
	if (it == fileIndices.end()) return nullptr;
	if (it->second.occupied) return nullptr;

	it->second.occupied = true;
	if (mode & xybase::FOM_TRUNCATE) it->second.curSize = 0;
	if (mode & xybase::FOM_APPEND) locs[curId] = it->second.curSize;
	
	fdMap[curId] = &it->second;
	return new InnerFile(curId++, it->second.size, it->second.offset, this, name);
}

std::list<std::u16string> BasicContainer::List()
{
	std::list<std::u16string> ret;

	for (auto &&item : fileIndices)
	{
		ret.push_back(item.first);
	}

	return ret;
}

void mule::BasicContainer::Close()
{
	file->Close();
}

std::u16string mule::BasicContainer::GetName()
{
	return name;
}

void mule::BasicContainer::Flush()
{
	file->Flush();
}

void mule::BasicContainer::MakeDir(std::u16string path)
{
	throw xybase::NotImplementedException();
}

void mule::BasicContainer::Remove(std::u16string target, bool recursive)
{
	throw xybase::NotImplementedException();
}

mule::BasicContainer::InnerFile::InnerFile(int handle, size_t size, size_t offset, BasicContainer *host, const std::u16string &name)
	: handle(handle), host(host), size(size), offset(offset)
{
	this->name = host->GetName() + name;
}

mule::BasicContainer::InnerFile::~InnerFile()
{
	Close();
}

void mule::BasicContainer::InnerFile::ReadBytes(char *buffer, size_t limit)
{
	host->ReadBytes(handle, buffer, limit);
}

void mule::BasicContainer::InnerFile::Write(const char *buffer, size_t size)
{
	host->Write(handle, buffer, size);
}

size_t mule::BasicContainer::InnerFile::Tell() const
{
	return host->locs[handle];
}

void mule::BasicContainer::InnerFile::Seek(long long offset, SeekMode mode)
{
	if (mode == SM_BEGIN)
	{
		host->locs[handle] = offset;
	}
	else if (mode == SM_CURRENT)
	{
		host->locs[handle] += offset;
	}
	else if (mode == SM_END)
	{
		host->locs[handle] = this->size + offset;
	}
}

void mule::BasicContainer::InnerFile::Close()
{
	host->Close(handle);
}

std::u16string mule::BasicContainer::InnerFile::GetName() const
{
	return name;
}

void mule::BasicContainer::InnerFile::Flush()
{
	host->Flush();
}

bool mule::BasicContainer::InnerFile::IsEof() const noexcept
{
	return host->locs[handle] == host->fdMap[handle]->size;
}
