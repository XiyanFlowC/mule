#include "BinaryStream.h"

#include "xystring.h"
#include "xyutils.h"

using namespace xybase;

#ifdef WIN32
#define fseek _fseeki64
#define ftell _ftelli64
#else
#define fseek fseeko64
#define ftell ftello64
#endif // WIN32

BinaryStream::BinaryStream(std::wstring path, const wchar_t *mode, bool isBigEndian)
	: name(path)
{
#ifdef WIN32
	stream = _wfopen(path.c_str(), mode);
#else
	stream = fopen(xybase::string::to_string(path).c_str(), xybase::string::to_string(mode).c_str());
#endif
	if (stream == nullptr)
	{
		throw IOException(path, L"Failed to open specified file.");
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
	return xybase::string::to_utf16(name);
}

void BinaryStream::ReadBytes(char* buffer, size_t limit)
{
	if (1 != fread(buffer, limit, 1, stream)) throw IOException(name, L"Read error.");
}

void BinaryStream::Write(const char* buffer, size_t size)
{
	if (1 != fwrite(buffer, size, 1, stream)) throw IOException(name, L"Write error.");
}

size_t BinaryStream::Tell() const
{
	return ftell(stream);
}

void BinaryStream::Seek(long long offset, SeekMode mode)
{
	int seekMode;
	switch (mode)
	{
	case xybase::Stream::SM_BEGIN:
		seekMode = SEEK_SET;
		break;
	case xybase::Stream::SM_CURRENT:
		seekMode = SEEK_CUR;
		break;
	case xybase::Stream::SM_END:
		seekMode = SEEK_END;
		break;
	default:
		throw xybase::InvalidParameterException(L"mode", L"Invalid seek mode detected.", 15563);
		break;
	}
	fseek(stream, offset, seekMode);
	OnSeek(this);
}

void BinaryStream::Close()
{
	OnClose(this);
	if (!isOpen) throw InvalidOperationException(L"Already closed.", __LINE__);
	isOpen = false;
	fclose(stream);
}

bool xybase::BinaryStream::IsEof() const noexcept
{
	return feof(stream);
}
