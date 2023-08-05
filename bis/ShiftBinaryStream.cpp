#include "ShiftBinaryStream.h"

using namespace mule::Data;

ShiftBinaryStream::ShiftBinaryStream(const char *path, long long offset, bool isBigEndian)
	: BinaryStream(path, isBigEndian)
{
	SetOffset(offset);
}

void ShiftBinaryStream::SetOffset(long long offset)
{
	this->offset = offset;
}

size_t ShiftBinaryStream::Tell()
{
	return offset + BinaryStream::Tell();
}

void ShiftBinaryStream::Seek(long long offset, int mode)
{
	BinaryStream::Seek(this->offset + offset, mode);
}
