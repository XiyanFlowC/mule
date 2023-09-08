#include "BinaryStream.h"
#include "xyutils.h"

using namespace xybase;

#ifdef _WIN32
#define fseek _fseeki64
#define ftell _ftelli64
#else
#define fseek fseeko64
#define ftell ftello64
#endif

BinaryStream::BinaryStream(std::u16string path, bool truncate, bool isBigEndian)
	: name(path)
{
	stream = fopen(xybase::string::to_string(path).c_str(), truncate ? "wb+" : "rb+");
	if (stream == nullptr)
	{
		throw IOException(path, u"Open file error.");
	}
	isOpen = true;
	this->isBigEndian = isBigEndian;
}

BinaryStream::~BinaryStream()
{
	if (isOpen) Close();
}

void xybase::BinaryStream::Flush()
{
	fflush(stream);
}

std::u16string xybase::BinaryStream::GetName() const
{
	return name;
}

uint8_t BinaryStream::ReadUInt8()
{
	uint8_t ret;
	ReadBytes((char *) & ret, sizeof(ret));
	return ret;
}

int8_t BinaryStream::ReadInt8()
{
	int8_t ret;
	ReadBytes((char *)&ret, sizeof(ret));
	return ret;
}

uint16_t BinaryStream::ReadUInt16()
{
	uint16_t ret;
	ReadBytes((char *)&ret, sizeof(ret));

	if (isBigEndian ^ bigEndianSystem)
	{
		ret = (ret & 0xFF) << 8 | (ret & 0xFF00) >> 8;
	}

	return ret;
}

int16_t BinaryStream::ReadInt16()
{
	union
	{
		uint16_t a;
		int16_t b;
	} ret = { 0 };
	ret.a = ReadUInt16();
	return ret.b;
}

uint32_t BinaryStream::ReadUInt32()
{
	uint32_t ret;
	ReadBytes((char *)&ret, sizeof(ret));

	if (isBigEndian ^ bigEndianSystem)
	{
		ret = (ret & 0xFF) << 24 | (ret & 0xFF00) << 8 | (ret & 0xFF0000) >> 8 | (ret & 0xFF000000) >> 24;
	}

	return ret;
}

int32_t BinaryStream::ReadInt32()
{
	union
	{
		uint32_t a;
		int32_t b;
	} ret = { 0 };
	ret.a = ReadUInt32();                                                                                                                                         
	return ret.b;
}

uint64_t BinaryStream::ReadUInt64()
{
	uint64_t ret;
	ReadBytes((char *)&ret, sizeof(ret));

	if (isBigEndian ^ bigEndianSystem)
	{
		ret = (ret & 0xFF) << 56 | (ret & 0xFF00) << 40 | (ret & 0xFF0000) << 24 | (ret & 0xFF000000) << 8 |
			(ret & 0xFF00000000ull) >> 8 | (ret & 0xFF0000000000ull) >> 24 | (ret & 0xFF000000000000ull) >> 40 |
			(ret & 0xFF00000000000000ull) >> 56;
	}

	return ret;
}

int64_t BinaryStream::ReadInt64()
{
	union
	{
		uint64_t a;
		int64_t b;
	} ret = { 0 };
	ret.a = ReadUInt64();
	return ret.b;
}

float BinaryStream::ReadFloat()
{
	union
	{
		uint32_t a;
		float b;
	} ret = { 0 };
	ret.a = ReadUInt32();
	return ret.b;
}

double BinaryStream::ReadDouble()
{
	union
	{
		uint64_t a;
		double b;
	} ret = { 0 };
	ret.a = ReadUInt64();
	return ret.b;
}

std::string BinaryStream::ReadString()
{
	StringBuilder sb;

	int ch = fgetc(stream);
	while (ch != 0)
	{
		if (ch == EOF) throw IOException(name, u"String reached EOF without a NUL!", __LINE__);

		sb += ch;

		ch = fgetc(stream);
	}
	return sb.ToString();
}

void BinaryStream::ReadBytes(char* buffer, int limit)
{
	if (1 != fread(buffer, limit, 1, stream)) throw IOException(name, u"Read error.");
}

void BinaryStream::Write(uint8_t value)
{
	Write((char *)&value, sizeof(value));
}

void BinaryStream::Write(int8_t value)
{
	Write((char *)&value, sizeof(value));
}

void BinaryStream::Write(uint16_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = (value & 0xFF) << 8 | (value & 0xFF00) >> 8;
	}

	Write((char *)&value, sizeof(value));
}

void BinaryStream::Write(int16_t value)
{
	Write(*((uint16_t*)&value));
}

void BinaryStream::Write(uint32_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = (value & 0xFF) << 24 | (value & 0xFF00) << 8 | (value & 0xFF0000) >> 8 | (value & 0xFF000000) >> 24;
	}

	Write((char *)&value, sizeof(value));
}

void BinaryStream::Write(int32_t value)
{
	Write(*((uint32_t*)&value));
}

void BinaryStream::Write(uint64_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = (value & 0xFF) << 56 | (value & 0xFF00) << 40 | (value & 0xFF0000) << 24 | (value & 0xFF000000) << 8 |
			(value & 0xFF00000000) >> 8 | (value & 0xFF0000000000) >> 24 | (value & 0xFF000000000000) >> 40 |
			(value & 0xFF00000000000000) >> 56;
	}

	Write((char *)&value, sizeof(value));
}

void BinaryStream::Write(int64_t value)
{
	Write(*((int64_t*)&value));
}

void BinaryStream::Write(float value)
{
	Write(*((uint32_t*)&value));
}

void BinaryStream::Write(double value)
{
	Write(*((uint64_t*)&value));
}

void BinaryStream::Write(const std::string& value)
{
	Write(value.c_str(), value.size() + 1);
}

void BinaryStream::Write(const char* buffer, size_t size)
{
	if (1 != fwrite(buffer, size, 1, stream)) throw IOException(name, u"Write error.");
}

size_t BinaryStream::Tell() const
{
	return ftell(stream);
}

void BinaryStream::Seek(long long offset, int mode)
{
	fseek(stream, offset, mode);
}

void BinaryStream::Close()
{
	if (!isOpen) throw InvalidOperationException(u"Already closed.", __LINE__);
	isOpen = false;
	fclose(stream);
}
