#include "ElfStream.h"
#include <format>

using namespace xybase;
using namespace mule::Stream;

ElfStream::ElfStream(xybase::Stream *stream)
	: stream(stream), shs(nullptr), phs(nullptr), shs64(nullptr), phs64(nullptr), header(nullptr), header64(nullptr)
{
	// 读取魔术头
	byte_t magicHeader[16];
	stream->ReadBytes(reinterpret_cast<char *>(magicHeader), 16);
	// fread(magicHeader, sizeof(magicHeader), 1, stream);
	if (0 != memcmp(magicSeq, magicHeader, 4))
	{
		throw ElfFormatErrorException(L"Not a valid ELF.", 410130);
	}
	if (magicHeader[4] != '\x01' && magicHeader[4] != '\x02')
	{
		throw ElfFormatErrorException(L"Invalid ELF bit or a new version of ELF.", 410120);
	}
	is64 = magicHeader[4] == '\x02';
	if (magicHeader[5] != '\x01' && magicHeader[5] != '\x02')
	{
		throw ElfFormatErrorException(L"Invalid data marshal.", 410110);
	}
	isBigEndian = magicHeader[5] == '\x02';
	if (magicHeader[6] != '\x01')
	{
		throw ElfFormatErrorException(L"Unsupported ELF version.", 410100);
	}
	stream->Seek(0, SM_BEGIN);
	if (is64) Init64(stream);
	else Init32(stream);

	//Seek(header->entry);
}

void mule::Stream::ElfStream::Init32(xybase::Stream *stream)
{
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
}

void mule::Stream::ElfStream::Init64(xybase::Stream *stream)
{
	header64 = new elf64_header;
	if (isBigEndian ^ bigEndianSystem)
	{
		ReadBytes((char *)header64->ident, 16);
		header64->type = ReadUInt16();
		header64->machine = ReadUInt16();
		header64->version = ReadUInt32();
		header64->entry = ReadUInt64();
		header64->phoff = ReadUInt64();
		header64->shoff = ReadUInt64();
		header64->flags = ReadUInt32();
		header64->ehsize = ReadUInt16();
		header64->phentsize = ReadUInt16();
		header64->phnum = ReadUInt16();
		header64->shentsize = ReadUInt16();
		header64->shnum = ReadUInt16();
		header64->shstrndx = ReadUInt16();
	}
	else
	{
		ReadBytes((char *)header64, sizeof(elf64_header));
	}

	phs64 = new program_header64[header64->phnum];
	stream->Seek(header64->phoff, SM_BEGIN);
	if (isBigEndian ^ bigEndianSystem)
	{
		auto *pp = phs64;
		for (int i = 0; i < header64->phnum; ++i, ++pp)
		{
			pp->type = ReadUInt32();
			pp->flags = ReadUInt32();
			pp->offset = ReadUInt64();
			pp->vaddr = ReadUInt64();
			pp->paddr = ReadUInt64();
			pp->filesz = ReadUInt64();
			pp->memsz = ReadUInt64();
			pp->align = ReadUInt64();
		}
	}
	else stream->ReadBytes((char *)phs64, sizeof(program_header64) * header64->phnum);

	shs64 = new section_header64[header64->shnum];
	stream->Seek(header64->shoff, SM_BEGIN);
	if (isBigEndian ^ bigEndianSystem)
	{
		for (int i = 0; i < header64->shnum; ++i)
		{
			shs64[i].name = ReadUInt32();
			shs64[i].type = ReadUInt32();
			shs64[i].flags = ReadUInt64();
			shs64[i].addr = ReadUInt64();
			shs64[i].offset = ReadUInt64();
			shs64[i].size = ReadUInt64();
			shs64[i].link = ReadUInt32();
			shs64[i].info = ReadUInt32();
			shs64[i].align = ReadUInt64();
			shs64[i].entsize = ReadUInt64();
		}
	}
	else stream->ReadBytes((char *)shs64, sizeof(section_header64) * header64->shnum);
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
		throw xybase::InvalidOperationException(L"Cannot seek from end on address mode.", 400200);
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
	if (shs64 != nullptr) delete[] shs64;
	if (phs64 != nullptr) delete[] phs64;
	if (header64 != nullptr) delete[] header64;
	shs = nullptr;
	phs = nullptr;
	header = nullptr;
	shs64 = nullptr;
	phs64 = nullptr;
	header64 = nullptr;
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
	if (is64)
		for (int i = 0; i < header64->shnum; ++i)
		{
			if (shs64[i].addr <= address && static_cast<unsigned long long>(shs64[i].addr) + shs64[i].size > address)
			{
				return shs64[i].align;
			}
		}
	else
		for (int i = 0; i < header->shnum; ++i)
		{
			if (shs[i].addr <= address && static_cast<unsigned long long>(shs[i].addr) + shs[i].size > address)
			{
				return shs[i].align;
			}
		}
	
	throw xybase::InvalidParameterException(L"address", std::format(L"Address {} out of range.", address), 400100);
}

size_t ElfStream::AddressToOffset(size_t address) const
{
	if (is64)
		for (int i = 0; i < header64->phnum; ++i)
		{
			if (phs64[i].vaddr <= address && static_cast<unsigned long long>(phs64[i].vaddr) + phs64[i].filesz > address)
			{
				long long diff = static_cast<long long>(phs64[i].vaddr) - phs64[i].offset;
				return address - diff;
			}
		}
	else
		for (int i = 0; i < header->phnum; ++i)
		{
			if (phs[i].vaddr <= address && static_cast<unsigned long long>(phs[i].vaddr) + phs[i].filesz > address)
			{
				long long diff = static_cast<long long>(phs[i].vaddr) - phs[i].offset;
				return address - diff;
			}
		}
	throw xybase::InvalidParameterException(L"address", std::format(L"Address {} out of range.", address), 400101);
}

size_t ElfStream::OffsetToAddress(size_t offset) const
{
	if (is64)
		for (int i = 0; i < header64->phnum; ++i)
		{
			if (phs64[i].offset <= offset && static_cast<unsigned long long>(phs64[i].offset) + phs64[i].filesz > offset)
			{
				long long diff = static_cast<long long>(phs64[i].offset) - phs64[i].vaddr;
				return offset - diff;
			}
		}
	else
		for (int i = 0; i < header->phnum; ++i)
		{
			if (phs[i].offset <= offset && static_cast<unsigned long long>(phs[i].offset) + phs[i].filesz > offset)
			{
				long long diff = static_cast<long long>(phs[i].offset) - phs[i].vaddr;
				return offset - diff;
			}
		}
	throw xybase::InvalidParameterException(L"offset", std::format(L"Offset {} out of range.", offset), 400102);
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
