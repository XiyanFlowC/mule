#include "TextStream.h"

#include "Exception/InvalidOperationException.h"
#include "StringBuilder.h"
#include "xystring.h"
#include "Exception/IOException.h"

xybase::TextStream::TextStream(std::string path, int mode)
{
	stream = fopen(path.c_str(), mode ? "w" : "r");
	if (stream == nullptr)
	{
		throw IOException(string::to_utf16(path), u"Failed to initialise TextStream.");
	}

	open = true;
}

xybase::TextStream::~TextStream()
{
	Close();
}

void xybase::TextStream::Close()
{
	if (open)
		fclose(stream);

	open = false;
}

std::string xybase::TextStream::ReadLine()
{
	StringBuilder<char> sb;
	
	int ch = fgetc(stream);
	while (ch != EOF && ch != '\n')
	{
		sb += ch;
	}

	return sb.ToString();
}

char xybase::TextStream::ReadChar()
{
	return fgetc(stream);
}

uint8_t xybase::TextStream::ReadUInt8()
{
	return ReadUInt64();
}

int8_t xybase::TextStream::ReadInt8()
{
	return ReadInt64();
}

uint16_t xybase::TextStream::ReadUInt16()
{
	return ReadUInt64();
}

int16_t xybase::TextStream::ReadInt16()
{
	return ReadInt64();
}

uint32_t xybase::TextStream::ReadUInt32()
{
	return ReadUInt64();
}

int32_t xybase::TextStream::ReadInt32()
{
	return ReadInt64();
}

uint64_t xybase::TextStream::ReadUInt64()
{
	uint64_t ret{};
	if (1 != fscanf(stream, "%llu", &ret)) throw IOException(u"", u"Unspecified/Format error.");
	return ret;
}

int64_t xybase::TextStream::ReadInt64()
{
	int64_t ret{};
	if (1 != fscanf(stream, "%lld", &ret)) throw IOException(u"", u"Unspecified/Format error.");
	return ret;
}

float xybase::TextStream::ReadFloat()
{
	float ret{};
	if (1 != fscanf(stream, "%f", &ret)) throw IOException(u"", u"Unspecified/Format error.");
	return ret;
}

double xybase::TextStream::ReadDouble()
{
	double ret{};
		if (1 != fscanf(stream, "%lf", &ret)) throw IOException(u"", u"Unspecified/Format error.");
	return ret;
}

std::string xybase::TextStream::ReadString()
{
	StringBuilder<char> sb;

	if (feof(stream))
	{
		throw IOException(u"", u"Read at EOF.");
	}

	int ch = fgetc(stream);

	while (ch != EOF && ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n')
		sb.Append(ch);

	if (ch != EOF) ungetc(ch, stream);

	return sb.ToString();
}

void xybase::TextStream::ReadBytes(char *buffer, int limit)
{
	throw InvalidOperationException(u"Cannot read raw bytes from a text stream.", 2350);
}

void xybase::TextStream::Write(char value)
{
	fprintf(stream, "%c", value);
}

void xybase::TextStream::Write(uint8_t value)
{
	fprintf(stream, "%hhu", value);
}

void xybase::TextStream::Write(int8_t value)
{
	fprintf(stream, "%hhd", value);
}

void xybase::TextStream::Write(uint16_t value)
{
	fprintf(stream, "%hu", value);
}

void xybase::TextStream::Write(int16_t value)
{
	fprintf(stream, "%hd", value);
}

void xybase::TextStream::Write(uint32_t value)
{
	fprintf(stream, "%u", value);
}

void xybase::TextStream::Write(int32_t value)
{
	fprintf(stream, "%d", value);
}

void xybase::TextStream::Write(uint64_t value)
{
	fprintf(stream, "%llu", value);
}

void xybase::TextStream::Write(int64_t value)
{
	fprintf(stream, "%lld", value);
}

void xybase::TextStream::Write(float value)
{
	fprintf(stream, "%f", value);
}

void xybase::TextStream::Write(double value)
{
	fprintf(stream, "%lf", value);
}

void xybase::TextStream::Write(const std::string &value)
{
	fprintf(stream, "%s", value.c_str());
}

void xybase::TextStream::Write(const char *value)
{
	fprintf(stream, "%s", value);
}

void xybase::TextStream::Write(const char *buffer, size_t size)
{
	throw InvalidOperationException(u"TextStream cannot write binary content.", 2351);
}

size_t xybase::TextStream::Tell()
{
	return ftell(stream);
}

void xybase::TextStream::Seek(long long offset, int mode)
{
	fseek(stream, offset, mode);
}
