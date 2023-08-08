#include "BasicContainer.h"

using namespace mule;

void mule::BasicContainer::ReadBytes(int id, char *buffer, int size)
{
	if (locs[id] + size > fdMap[id]->curSize)
		throw xybase::OutOfRangeException("Try to read after EOF.", __LINE__);
	if (file->Tell() != locs[id] + fdMap[id]->offset)
	{
		file->Seek(locs[id] + fdMap[id]->offset, 0);
	}
	file->ReadBytes(buffer, size);
	locs[id] += size;
}

template<>
void mule::BasicContainer::Write<const std::string &>(int id, const std::string &value)
{
	if (locs[id] + value.size() >= fdMap[id]->size)
		throw xybase::OutOfRangeException("Write exceed maximum size.", __LINE__);
	if (file->Tell() != locs[id])
	{
		file->Seek(locs[id], 0);
	}
	file->Write(value.c_str(), value.size());
	locs[id] += value.size();
	fdMap[id]->curSize = std::max(fdMap[id]->curSize, locs[id]);
}

void mule::BasicContainer::Write(int id, const char *buffer, size_t size)
{
	if (locs[id] + size >= fdMap[id]->size)
		throw xybase::OutOfRangeException("Write exceed maximum size.", __LINE__);
	if (file->Tell() != locs[id])
	{
		file->Seek(locs[id], 0);
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
{
	file = stream;
}

xybase::Stream *BasicContainer::Open(std::string name, FileOpenMode mode)
{
	auto &&it = fileIndices.find(name);
	if (it == fileIndices.end()) return nullptr;
	if (it->second.occupied) return nullptr;

	it->second.occupied = true;
	if (mode == FOM_TRUNCATE) it->second.curSize = 0;
	
	fdMap[curId] = &it->second;
	return new InnerFile(curId++, it->second.size, it->second.offset, this);
}

std::list<std::string> BasicContainer::List()
{
	std::list<std::string> ret;

	for (auto &&item : fileIndices)
	{
		ret.push_back(item.first);
	}

	return ret;
}

std::string BasicContainer::WorkDir()
{
	return std::string("");
}

void BasicContainer::ChangeDir(std::string path)
{
}

void mule::BasicContainer::Close()
{
	file->Close();
}

void mule::BasicContainer::MakeDir(std::string path)
{
}

void mule::BasicContainer::Remove(std::string target, bool recursive)
{
}

mule::BasicContainer::InnerFile::InnerFile(int token, size_t size, size_t offset, BasicContainer *host)
	: token(token), host(host), size(size), offset(offset) { }

uint8_t mule::BasicContainer::InnerFile::ReadUInt8()
{
	return host->Read<uint8_t>(token);
}

mule::BasicContainer::InnerFile::~InnerFile()
{
	Close();
}

int8_t mule::BasicContainer::InnerFile::ReadInt8()
{
	return host->Read<int8_t>(token);
}

uint16_t mule::BasicContainer::InnerFile::ReadUInt16()
{
	auto ret = host->Read<uint16_t>(token);
	if (isBigEndian ^ bigEndianSystem)
	{
		return ((ret & 0xFF00) >> 8) | ((ret & 0xFF) << 8);
	}
	return ret;
}

int16_t mule::BasicContainer::InnerFile::ReadInt16()
{
	union
	{
		uint16_t a;
		int16_t b;
	} ret = { 0 };
	ret.a = ReadUInt16();
	return ret.b;
}

uint32_t mule::BasicContainer::InnerFile::ReadUInt32()
{
	auto ret = host->Read<uint32_t>(token);
	if (isBigEndian ^ bigEndianSystem)
	{
		return (ret & 0xFF) << 24 | (ret & 0xFF00) << 8 | (ret & 0xFF0000) >> 8 | (ret & 0xFF000000) >> 24;
	}
	return ret;
}

int32_t mule::BasicContainer::InnerFile::ReadInt32()
{
	union
	{
		uint32_t a;
		int32_t b;
	} ret = { 0 };
	ret.a = ReadUInt32();
	return ret.b;
}

uint64_t mule::BasicContainer::InnerFile::ReadUInt64()
{
	auto ret = host->Read<uint64_t>(token);
	if (isBigEndian ^ bigEndianSystem)
	{
		return (ret & 0xFF) << 56 | (ret & 0xFF00) << 40 | (ret & 0xFF0000) << 24 | (ret & 0xFF000000) << 8 |
			(ret & 0xFF00000000ull) >> 8 | (ret & 0xFF0000000000ull) >> 24 | (ret & 0xFF000000000000ull) >> 40 |
			(ret & 0xFF00000000000000ull) >> 56;
	}
	return ret;
}

int64_t mule::BasicContainer::InnerFile::ReadInt64()
{
	union
	{
		uint64_t a;
		int64_t b;
	} ret = { 0 };
	ret.a = ReadUInt64();
	return ret.b;
}

float mule::BasicContainer::InnerFile::ReadFloat()
{
	union
	{
		uint32_t a;
		float b;
	} ret = { 0 };
	ret.a = ReadUInt32();
	return ret.b;
}

double mule::BasicContainer::InnerFile::ReadDouble()
{
	union
	{
		uint64_t a;
		double b;
	} ret = { 0 };
	ret.a = ReadUInt64();
	return ret.b;
}

std::string mule::BasicContainer::InnerFile::ReadString()
{
	xybase::StringBuilder<char> sb;

	char ch = host->Read<char>(token);
	while (ch != '\0')
	{
		sb.Append(ch);

		ch = host->Read<char>(token);
	}

	return sb.ToString();
}

void mule::BasicContainer::InnerFile::ReadBytes(char *buffer, int limit)
{
	host->ReadBytes(token, buffer, limit);
}

void mule::BasicContainer::InnerFile::Write(uint8_t value)
{
	host->Write(token, value);
}

void mule::BasicContainer::InnerFile::Write(int8_t value)
{
	host->Write(token, value);
}

void mule::BasicContainer::InnerFile::Write(uint16_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = ((value & 0xFF00) >> 8) | ((value & 0xFF) << 8);
	}
	host->Write(token, value);
}

void mule::BasicContainer::InnerFile::Write(int16_t value)
{
	Write(*((uint16_t *)&value));
}

void mule::BasicContainer::InnerFile::Write(uint32_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = (value & 0xFF) << 24 | (value & 0xFF00) << 8 | (value & 0xFF0000) >> 8 | (value & 0xFF000000) >> 24;
	}

	host->Write(token, value);
}

void mule::BasicContainer::InnerFile::Write(int32_t value)
{
	Write(*((uint32_t *)&value));
}

void mule::BasicContainer::InnerFile::Write(uint64_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = (value & 0xFF) << 56 | (value & 0xFF00) << 40 | (value & 0xFF0000) << 24 | (value & 0xFF000000) << 8 |
			(value & 0xFF00000000) >> 8 | (value & 0xFF0000000000) >> 24 | (value & 0xFF000000000000) >> 40 |
			(value & 0xFF00000000000000) >> 56;
	}

	host->Write(token, value);
}

void mule::BasicContainer::InnerFile::Write(int64_t value)
{
	Write(*((uint64_t *)&value));
}

void mule::BasicContainer::InnerFile::Write(float value)
{
	Write(*((uint32_t *)&value));
}

void mule::BasicContainer::InnerFile::Write(double value)
{
	Write(*((uint64_t *)&value));
}

void mule::BasicContainer::InnerFile::Write(const std::string &value)
{
	host->Write(token, value);
}

void mule::BasicContainer::InnerFile::Write(const char *buffer, size_t size)
{
	host->Write(token, buffer, size);
}

size_t mule::BasicContainer::InnerFile::Tell()
{
	return host->locs[token];
}

void mule::BasicContainer::InnerFile::Seek(long long offset, int mode)
{
	if (mode == 0)
	{
		host->locs[token] = offset;
	}
	else if (mode == 1)
	{
		host->locs[token] += offset;
	}
	else if (mode == 2)
	{
		host->locs[token] = this->size + offset;
	}
}

void mule::BasicContainer::InnerFile::Close()
{
	host->Close(token);
}
