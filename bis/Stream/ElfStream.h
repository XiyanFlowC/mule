#pragma once

#ifndef ELF_STREAM_H__

#include <Exception/InvalidOperationException.h>
#include <Exception/NotImplementedException.h>
#include <StreamBasic.h>

#include <cstdlib>
#include <cstdint>

namespace mule
{
	namespace Stream
	{
		typedef uint8_t  byte_t;
		typedef uint16_t half_t;
		typedef uint32_t word_t;
		typedef uint64_t dword_t;
#pragma pack(push,1)
		struct elf_header {
			byte_t ident[16];  /*we don't care info in it*/
			half_t type;
			half_t machine;
			word_t version;
			word_t entry;
			word_t phoff;
			word_t shoff;
			word_t flags;
			half_t ehsize;
			half_t phentsize;
			half_t phnum;
			half_t shentsize;
			half_t shnum;
			half_t shstrndx;
		};

		struct elf64_header {
			byte_t ident[16];
			half_t type;
			half_t machine;
			word_t version;
			dword_t entry;
			dword_t phoff;
			dword_t shoff;
			word_t flags;
			half_t ehsize;
			half_t phentsize;
			half_t phnum;
			half_t shentsize;
			half_t shnum;
			half_t shstrndx;
		};

		struct section_header {
			word_t name;
			word_t type;
			word_t flags;
			word_t addr;
			word_t offset;
			word_t size;
			word_t link;
			word_t info;
			word_t align;
			word_t entsize;
		};

		struct section_header64 {
			word_t name;
			word_t type;
			dword_t flags;
			dword_t addr;
			dword_t offset;
			dword_t size;
			word_t link;
			word_t info;
			dword_t align;
			dword_t entsize;
		};

		struct program_header {
			word_t type;
			word_t offset;
			word_t flags;
			word_t vaddr;
			word_t paddr;
			word_t filesz;
			word_t memsz;
			word_t align;
		};

		struct program_header64 {
			word_t type;
			word_t flags;
			dword_t offset;
			dword_t vaddr;
			dword_t paddr;
			dword_t filesz;
			dword_t memsz;
			dword_t align;
		};

		struct rel_entry {
			word_t offset;
			word_t info;
		};
#pragma pack(pop)
		class ElfFormatErrorException : public xybase::RuntimeException
		{
		public:
			ElfFormatErrorException(std::wstring message, int line);
		};

		class ElfStream : public xybase::StreamBasic
		{
			elf_header *header;
			section_header *shs;
			program_header *phs;
			elf64_header *header64;
			section_header64 *shs64;
			program_header64 *phs64;
			bool is64;
			const char magicSeq[4] = { 0x7F, 'E', 'L', 'F' };
			xybase::Stream *stream;

		public:
			ElfStream(xybase::Stream *stream);
			void Init32(xybase::Stream *stream);
			void Init64(xybase::Stream *stream);
			~ElfStream();
			virtual size_t Tell() const override;
			virtual void Seek(long long offset, SeekMode mode = SM_BEGIN) override;
			virtual void Close() override;

			virtual void SeekOffset(size_t offset, SeekMode mode = SM_BEGIN);
			virtual size_t TellOffset();

			size_t GetAlign(size_t address);
			size_t AddressToOffset(size_t address) const;
			size_t OffsetToAddress(size_t offset) const;

			virtual void ReadBytes(char *buffer, size_t limit) override;
			virtual void Write(const char *buffer, size_t size) override;

			// 通过 StreamBasic 继承
			virtual std::u16string GetName() const override;

			// 通过 StreamBasic 继承
			virtual void Flush() override;

			virtual bool IsEof() const noexcept override;
		};
	}
}

#endif // !ELF_STREAM_H__
