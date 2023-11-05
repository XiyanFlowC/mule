#pragma once
#ifndef XY_FILE_CONTAINER_BASIC_H__
#define XY_FILE_CONTAINER_BASIC_H__

#include <cstdio>
#include <map>
#include "FileContainer.h"
#include "StreamBasic.h"
#include "Fragment/FragmentManager.h"
#include "Exception/InvalidOperationException.h"

#include "xyapi.h"

namespace xybase
{
	/**
	 * @brief 文件容器的基础实现。提供文件包的读写操作。不包含任何文件包解析。
	*/
	class XY_API FileContainerBasic : public FileContainer
	{
		// 读写状态，0 为读，1 为写
		int mode;
	public:
		FileContainerBasic(xybase::Stream* stream);

		virtual ~FileContainerBasic();

		std::u16string GetName() override;

		void Flush() override;

		xybase::Stream *Open(std::u16string name, FileOpenMode mode) override;

		std::list<std::u16string> List() override;

		void MakeDir(std::u16string path) override;

		void Remove(const std::u16string &path);

		void Remove(std::u16string path, bool recursive) override;

		class XY_API InnerStream : public StreamBasic
		{
			unsigned long long fileHandle;
			FileContainerBasic *host;
		public:
			InnerStream(FileContainerBasic *host, unsigned long long fileHandle);

			InnerStream(const InnerStream &) = delete;
			InnerStream(InnerStream &&) = delete;
			const InnerStream &operator=(const InnerStream &) = delete;

			~InnerStream();

			void Flush() override;

			std::u16string GetName() const override;

			void ReadBytes(char *buffer, size_t limit) override;

			size_t Tell() const override;

			void Seek(long long offset, SeekMode mode) override;

			void Write(const char *buffer, size_t limit) override;


			// 通过 StreamBasic 继承
			void Close() override;

		};

	protected:
		xybase::Stream *infraStream;
		int align = 1;

		struct FileEntry
		{
			size_t offset;
			size_t size;
			std::u16string path;
			bool occupied;
		};

		struct OpenedFileInformation
		{
			size_t cursor;
			size_t size;
			size_t capacity;
			InnerStream *stream;
			FileEntry *baseEntry;
			bool readable;
			bool writable;
		};

		std::map<unsigned long long, OpenedFileInformation> openedFiles;
		unsigned long long currentHandle;

		std::map<std::u16string, FileEntry*> files;

		Fragment::FragmentManager freeSpaces;

		void Write(unsigned long long handle, const char *buffer, size_t limit);

		void ReadBytes(unsigned long long handle, char *buffer, size_t length);

		void Seek(unsigned long long handle, long long offset, Stream::SeekMode mode);

		size_t Tell(unsigned long long handle);

		void Close(unsigned long long handle);

		// 通过 FileContainer 继承
		bool Exists(std::u16string path) override;
	};
}


#endif // !XY_FILE_CONTAINER_BASIC_H__
