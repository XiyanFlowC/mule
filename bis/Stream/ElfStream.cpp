#include "ElfStream.h"

using namespace xybase;
using namespace mule::Stream;

ElfStream::ElfStream(xybase::Stream *stream)
	: stream(stream)
{
	// 读取魔术头
	byte_t magicHeader[16];
	stream->ReadBytes(reinterpret_cast<char *>(magicHeader), 16);
	// fread(magicHeader, sizeof(magicHeader), 1, stream);
	if (0 != memcmp(magicSeq, magicHeader, 4))
	{
		throw ElfFormatErrorException(L"Not a valid ELF.", __LINE__);
	}
	if (magicHeader[4] != '\x01')
	{
		throw ElfFormatErrorException(L"Only support 32-bit ELF.", __LINE__);
	}
	if (magicHeader[5] != '\x01' && magicHeader[5] != '\x02')
	{
		throw ElfFormatErrorException(L"Invalid data marshal.", __LINE__);
	}
	isBigEndian = magicHeader[5] == '\x02' ? true : false;
	if (magicHeader[6] != '\x01')
	{
		throw ElfFormatErrorException(L"Unsupported ELF version.", __LINE__);
	}
	stream->Seek(0, SM_BEGIN);
	header = new elf_header;
	if (isBigEndian ^ bigEndianSystem)
	{
		ReadBytes((char *)header->ident, 16);
		header->type = ReadUInt16();
		header->machine = ReadUInt16();
		header->version = ReadUInt32();
		header->entry = ReadUInt32();
		header->phoff = ReadUInt32();
		header->shoff = ReadUInt32();
		header->flags = ReadUInt32();
		header->ehsize = ReadUInt16();
		header->phentsize = ReadUInt16();
		header->phnum = ReadUInt16();
		header->shentsize = ReadUInt16();
		header->shnum = ReadUInt16();
		header->shstrndx = ReadUInt16();
	}
	else
	{
		stream->ReadBytes((char *)header, sizeof(elf_header));
	}

	phs = new program_header[header->phnum];
	stream->Seek(header->phoff, SM_BEGIN);
	if (isBigEndian ^ bigEndianSystem)
	{
		throw xybase::NotImplementedException();
	}
	stream->ReadBytes((char *)phs, sizeof(program_header) * header->phnum);

	shs = new section_header[header->shnum];
	stream->Seek(header->shoff, SM_BEGIN);
	if (isBigEndian ^ bigEndianSystem)
	{
		throw xybase::NotImplementedException();
	}
	stream->ReadBytes((char *)shs, sizeof(section_header) * header->shnum);

	//Seek(header->entry);
}

ElfStream::~ElfStream()
{
	Close();
}

size_t ElfStream::Tell() const
{
	return OffsetToAddress(stream->Tell());
}

void ElfStream::Seek(long long offset, SeekMode mode)
{
	if (mode == SM_END)
	{
		throw xybase::InvalidOperationException(L"Cannot seek from end on address mode.", __LINE__);
	}

	else if (mode == SM_CURRENT)
	{
		stream->Seek(offset, mode);
	}
	else
	{
		stream->Seek(AddressToOffset(offset), mode);
	}
}

void ElfStream::Close()
{
	OnClose(this);
	// if (stream != nullptr) stream->Close();
	if (shs != nullptr) delete[] shs;
	if (phs != nullptr) delete[] phs;
	if (header != nullptr) delete header;
	shs = nullptr;
	phs = nullptr;
	header = nullptr;
}

void ElfStream::SeekOffset(size_t offset, SeekMode mode)
{
	stream->Seek(offset, mode);
}

size_t ElfStream::TellOffset()
{
	return stream->Tell();
}

size_t ElfStream::GetAlign(size_t address)
{
	for (int i = 0; i < header->shnum; ++i)
	{
		if (shs[i].addr <= address && static_cast<unsigned long long>(shs[i].addr) + shs[i].size > address)
		{
			return shs[i].align;
		}
	}
	throw xybase::InvalidParameterException(L"offset", L"Address out of range.", __LINE__);
}

size_t ElfStream::AddressToOffset(size_t address) const
{
	for (int i = 0; i < header->phnum; ++i)
	{
		if (phs[i].vaddr <= address && static_cast<unsigned long long>(phs[i].vaddr) + phs[i].filesz > address)
		{
			long long diff = static_cast<long long>(phs[i].vaddr) - phs[i].offset;
			return address - diff;
		}
	}
	throw xybase::InvalidParameterException(L"offset", L"Address out of range.", __LINE__);
}

size_t ElfStream::OffsetToAddress(size_t offset) const
{
	for (int i = 0; i < header->phnum; ++i)
	{
		if (phs[i].offset <= offset && static_cast<unsigned long long>(phs[i].offset) + phs[i].filesz > offset)
		{
			long long diff = static_cast<long long>(phs[i].offset) - phs[i].vaddr;
			return offset - diff;
		}
	}
	throw xybase::InvalidParameterException(L"offset", L"Address out of range.", __LINE__);
}

void ElfStream::ReadBytes(char *buffer, size_t limit)
{
	stream->ReadBytes(buffer, limit);
}

void ElfStream::Write(const char *buffer, size_t size)
{
	stream->Write(buffer, size);
}

std::u16string mule::Stream::ElfStream::GetName() const
{
	return stream->GetName() + u"|elf";
}

void mule::Stream::ElfStream::Flush()
{
	stream->Flush();
}

bool mule::Stream::ElfStream::IsEof() const noexcept
{
	return stream->IsEof();
}

ElfFormatErrorException::ElfFormatErrorException(std::wstring msg, int line)
	: RuntimeException(msg, line)
{
}
