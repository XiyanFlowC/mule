#include "TextStream.h"

#include <locale>
#include "Exception/NotImplementedException.h"
#include "Exception/InvalidOperationException.h"
#include "StringBuilder.h"
#include "xystring.h"
#include "Exception/IOException.h"

xybase::TextStream::TextStream(std::string path, std::ios::openmode mode, std::string localeStr)
	: stream {path, mode}, name {xybase::string::to_wstring(path)}
{
	if (!localeStr.empty())
	{
		stream.imbue(std::locale{localeStr});
	}
	if (!stream)
	{
		throw xybase::IOException(name, L"Failed to open.", errno);
	}
}

xybase::TextStream::~TextStream()
{
	Close();
}

void xybase::TextStream::Flush()
{
	stream.flush();
}

void xybase::TextStream::Close()
{
	OnClose(this);
	stream.close();
}

std::u16string xybase::TextStream::GetName() const
{
	return xybase::string::to_utf16(name);
}

bool xybase::TextStream::ReadLine(std::string &out)
{
	if (std::getline(stream, out)) return true;
	else return false;
}

char xybase::TextStream::ReadChar()
{
	return stream.get();
}

uint8_t xybase::TextStream::ReadUInt8()
{
	return (uint8_t)ReadUInt64();
}

int8_t xybase::TextStream::ReadInt8()
{
	return (int8_t)ReadInt64();
}

uint16_t xybase::TextStream::ReadUInt16()
{
	return (uint16_t)ReadUInt64();
}

int16_t xybase::TextStream::ReadInt16()
{
	return (int16_t)ReadInt64();
}

uint32_t xybase::TextStream::ReadUInt32()
{
	return (uint32_t)ReadUInt64();
}

int32_t xybase::TextStream::ReadInt32()
{
	return (int32_t)ReadInt64();
}

uint64_t xybase::TextStream::ReadUInt64()
{
	uint64_t ret{};
	stream >> ret;
	return ret;
}

int64_t xybase::TextStream::ReadInt64()
{
	int64_t ret{};
	stream >> ret;
	return ret;
}

float xybase::TextStream::ReadFloat()
{
	float ret{};
	stream >> ret;
	return ret;
}

double xybase::TextStream::ReadDouble()
{
	double ret{};
	stream >> ret;
	return ret;
}

std::string xybase::TextStream::ReadString()
{
	std::string ret;
	stream >> ret;
	return ret;
}

void xybase::TextStream::ReadBytes(char *buffer, size_t limit)
{
	throw InvalidOperationException(L"Cannot read raw bytes from a text stream.", 2350);
}

void xybase::TextStream::Write(char value)
{
	stream << value;
}

void xybase::TextStream::Write(uint8_t value)
{
	stream << value;
}

void xybase::TextStream::Write(int8_t value)
{
	stream << value;
}

void xybase::TextStream::Write(uint16_t value)
{
	stream << value;
}

void xybase::TextStream::Write(int16_t value)
{
	stream << value;
}

void xybase::TextStream::Write(uint32_t value)
{
	stream << value;
}

void xybase::TextStream::Write(int32_t value)
{
	stream << value;
}

void xybase::TextStream::Write(uint64_t value)
{
	stream << value;
}

void xybase::TextStream::Write(int64_t value)
{
	stream << value;
}

void xybase::TextStream::Write(float value)
{
	stream << value;
}

void xybase::TextStream::Write(double value)
{
	stream << value;
}

void xybase::TextStream::Write(const std::string &value)
{
	stream << value;
}

void xybase::TextStream::Write(const char *value)
{
	stream << value;
}

void xybase::TextStream::Write(const char *buffer, size_t size)
{
	throw InvalidOperationException(L"TextStream cannot write binary content.", 2351);
}

size_t xybase::TextStream::Tell() const
{
	throw xybase::NotImplementedException();
}

void xybase::TextStream::Seek(long long offset, SeekMode mode)
{
	OnSeek(this);
	std::ios_base::seekdir sm;
	switch (mode)
	{
	case xybase::Stream::SM_BEGIN:
		sm = std::ios::beg;
		break;
	case xybase::Stream::SM_CURRENT:
		sm = std::ios::cur;
		break;
	case xybase::Stream::SM_END:
		sm = std::ios::end;
		break;
	default:
		throw xybase::InvalidParameterException(L"mode", L"Invalid seek mode.", 6542);
		break;
	}
	stream.seekg(offset, sm);
	stream.seekp(offset, sm);
}

bool xybase::TextStream::IsEof() const noexcept
{
	return stream.eof();
}
