#include "BinaryStream.h"

using namespace mule;
using namespace mule::Exception;

#ifdef _WIN32
#define fseek _fseeki64
#define ftell _ftelli64
#else
#define fseek fseeko64
#define ftell ftello64
#endif

BinaryStream::BinaryStream(const char* path, bool isBigEndian)
{
	stream = fopen(path, "rb+");
	if (stream == nullptr)
	{
		throw Exception::Exception(std::string("Open file error: ") + strerror(errno), MULE_FILE, __LINE__);
	}
	isOpen = true;
	this->isBigEndian = isBigEndian;
}

BinaryStream::~BinaryStream()
{
	if (isOpen) Close();
}

#ifdef BS_CPP_BSREADEX
#error Macro BS_CPP_BSREADEX pre-defined alarm!
#endif
#define BS_CPP_BSREADEX(size) if (0 == fread(&ret, size, 1, stream)) throw Exception::Exception(std::string("Read error: ") + strerror(errno), MULE_FILE, __LINE__)

uint8_t BinaryStream::ReadUInt8()
{
	uint8_t ret;
	BS_CPP_BSREADEX(1);
	return ret;
}

int8_t BinaryStream::ReadInt8()
{
	int8_t ret;
	BS_CPP_BSREADEX(1);
	return ret;
}

uint16_t BinaryStream::ReadUInt16()
{
	uint16_t ret;
	BS_CPP_BSREADEX(2);

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
	BS_CPP_BSREADEX(4);

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
	BS_CPP_BSREADEX(8);

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
	char* buffer = nullptr;
	size_t currentSize = 128, length = 0;

	buffer = (char*)malloc(currentSize);
	if (buffer == nullptr) throw Exception::Exception("Memory allocation falied.", MULE_FILE, __LINE__);

	int ch = fgetc(stream);
	while (ch != 0)
	{
		if (ch == EOF) throw Exception::Exception("String reached EOF without a NUL!", MULE_FILE, __LINE__);

		buffer[length++] = ch;

		if (length == currentSize)
		{
			currentSize <<= 1;
			char* tmp = (char*)realloc(buffer, currentSize);
			if (tmp == nullptr)
			{
				tmp = (char*)malloc(currentSize);
				if (tmp == nullptr) throw Exception::Exception("Memory re-allocation failed.", MULE_FILE, __LINE__);
				memcpy(tmp, buffer, currentSize >> 1);
				free(buffer);
			}
			buffer = tmp;
		}
		ch = fgetc(stream);
	}
	buffer[length] = '\0';

	std::string ret(buffer);
	free(buffer);

	return ret;
}

void BinaryStream::ReadBytes(char* buffer, int limit)
{
	fread(buffer, limit, 1, stream);
}

#undef BS_CPP_BSREADEX

#ifdef BS_CPP_BSWRITEEX
#error Macro BS_CPP_BSWRITEEX have been defiend already!
#endif
#define BS_CPP_BSWRITEEX(size) if (0 == fwrite(&value, size, 1, stream)) throw Exception::Exception(std::string("Write error: ") + strerror(errno), MULE_FILE, __LINE__)

void BinaryStream::Write(uint8_t value)
{
	BS_CPP_BSWRITEEX(1);
}

void BinaryStream::Write(int8_t value)
{
	BS_CPP_BSWRITEEX(1);
}

void BinaryStream::Write(uint16_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = (value & 0xFF) << 8 | (value & 0xFF00) >> 8;
	}

	BS_CPP_BSWRITEEX(2);
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

	BS_CPP_BSWRITEEX(4);
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

	BS_CPP_BSWRITEEX(8);
}

void BinaryStream::Write(int64_t value)
{
	Write(*((uint64_t*)&value));
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
	if (1 != fwrite(buffer, size, 1, stream)) throw Exception::Exception(std::string("Write error: ") + strerror(errno), MULE_FILE, __LINE__);
}
#undef BS_CPP_BSWRITEEX

size_t BinaryStream::Tell()
{
	return ftell(stream);
}

void BinaryStream::Seek(long long offset, int mode)
{
	fseek(stream, offset, mode);
}

void BinaryStream::Close()
{
	if (!isOpen) throw InvalidOperationException("Already closed.", MULE_FILE, __LINE__);
	isOpen = false;
	fclose(stream);
}
