#include "ElfStream.h"

using namespace mule::Data;

ElfStream::ElfStream(const std::string &name)
	: BinaryStream(name.c_str())
{
	// ¶ÁÈ¡Ä§ÊõÍ·
	byte_t magicHeader[16];
	fread(magicHeader, sizeof(magicHeader), 1, stream);
	if (0 != memcmp(magicSeq, magicHeader, 4))
	{
		throw ElfFormatErrorException("Not a valid ELF. (file :" + name, __FILE__, __LINE__);
	}
	if (magicHeader[4] != '\x01')
	{
		throw ElfFormatErrorException("Only support 32-bit ELF.", __FILE__, __LINE__);
	}
	if (magicHeader[5] != '\x01' && magicHeader[5] != '\x02')
	{
		throw ElfFormatErrorException("Invalid data marshal.", __FILE__, __LINE__);
	}
	isBigEndian = magicHeader[5] == '\x02' ? true : false;
	if (magicHeader[6] != '\x01')
	{
		throw ElfFormatErrorException("Unsupported ELF version.", __FILE__, __LINE__);
	}
	fseek(stream, 0, SEEK_SET);
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
		fread(header, sizeof(elf_header), 1, stream);
	}

	phs = new program_header[header->phnum];
	fseek(stream, header->phoff, SEEK_SET);
	if (isBigEndian ^ bigEndianSystem)
	{
		throw mule::Exception::NotImplementedException(__FILE__, __LINE__);
	}
	fread(phs, sizeof(program_header), header->phnum, stream);

	shs = new section_header[header->shnum];
	fseek(stream, header->shoff, SEEK_SET);
	if (isBigEndian ^ bigEndianSystem)
	{
		throw mule::Exception::NotImplementedException(__FILE__, __LINE__);
	}
	fread(shs, sizeof(section_header), header->shnum, stream);

	Seek(header->entry);
}

ElfStream::~ElfStream()
{
	Close();
}

size_t ElfStream::Tell()
{
	return size_t();
}

void ElfStream::Seek(long long offset, int mode)
{
	if (mode == SEEK_END)
	{
		throw mule::Exception::InvalidOperationException("Cannot seek from end on address mode.", __FILE__, __LINE__);
	}

	else if (mode == SEEK_CUR)
	{
		fseek(stream, offset, mode);
	}
	else
	{
		fseek(stream, AddressToOffset(offset), mode);
	}
}

void ElfStream::Close()
{
	if (!isOpen)
		return;

	if (stream != nullptr) fclose(stream);
	if (shs != nullptr) delete shs;
	if (phs != nullptr) delete phs;
	if (header != nullptr) delete header;
}

void ElfStream::SeekOffset(size_t offset, int mode)
{
	BinaryStream::Seek(offset, mode);
}

size_t ElfStream::TellOffset()
{
	return BinaryStream::Tell();
}

size_t ElfStream::GetAlign(size_t address)
{
	for (int i = 0; i < header->phnum; ++i)
	{
		if (shs[i].addr <= address && shs[i].addr + shs[i].size > address)
		{
			return shs[i].align;
		}
	}
	throw mule::Exception::InvalidParameterException("offset", "Address out of range.", __FILE__, __LINE__);
}

size_t ElfStream::AddressToOffset(size_t address)
{
	for (int i = 0; i < header->phnum; ++i)
	{
		if (phs[i].vaddr <= address && phs[i].vaddr + phs[i].filesz > address)
		{
			long long diff = static_cast<long long>(phs[i].vaddr) - phs[i].offset;
			return address + diff;
		}
	}
	throw mule::Exception::InvalidParameterException("offset", "Address out of range.", __FILE__, __LINE__);
}

size_t ElfStream::OffsetToAddress(size_t offset)
{
	for (int i = 0; i < header->phnum; ++i)
	{
		if (phs[i].vaddr <= offset && phs[i].vaddr + phs[i].filesz > offset)
		{
			long long diff = static_cast<long long>(phs[i].offset) - phs[i].vaddr;
			return offset + diff;
		}
	}
	throw mule::Exception::InvalidParameterException("offset", "Address out of range.", __FILE__, __LINE__);
}

ElfFormatErrorException::ElfFormatErrorException(std::string msg, const char *file, int line)
	: Exception(msg, file, line)
{
}
