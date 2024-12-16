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
	class FileContainerBasic : public FileContainer
	{
		// 读写状态，0 为读，1 为写
		int mode;
	public:
		/**
		 * @brief 用于记录文件包内部文件的描述信息。
		 */
		struct FileEntry
		{
			/**
			 * @brief 在文件包中的偏移量（字节）。
			 */
			size_t offset;
			/**
			 * @brief 所占大小（字节）。
			 */
			size_t size;
			/**
			 * @brief 到文件的原始路径表示。
			 */
			std::u16string path;
			/**
			 * @brief 该文件是否正被独占访问。
			 */
			bool occupied;
		};

		XY_API FileContainerBasic(xybase::Stream* stream);

		XY_API virtual ~FileContainerBasic();

		XY_API std::u16string GetName() override;

		XY_API void Flush() override;

		XY_API xybase::Stream *Open(std::u16string name, FileOpenMode mode) override;

		XY_API std::list<std::u16string> List() override;

		XY_API void MakeDir(std::u16string path) override;

		XY_API void Remove(const std::u16string &path);

		XY_API void Remove(std::u16string path, bool recursive) override;

		XY_API void SetMetadata(const std::u16string &name, const FileEntry &data);

		XY_API FileEntry GetMetadata(const std::u16string &name);

		/**
		 * @brief 内部文件类。用于承载文件包内的文件读写。
		 */
		class InnerStream : public StreamBasic
		{
			unsigned long long fileHandle;
			FileContainerBasic *host;
			bool isOpen;
		public:
			XY_API InnerStream(FileContainerBasic *host, unsigned long long fileHandle, bool isBigEndian = false);

			XY_API InnerStream(const InnerStream &) = delete;
			XY_API InnerStream(InnerStream &&) = delete;
			XY_API const InnerStream &operator=(const InnerStream &) = delete;

			XY_API ~InnerStream();

			XY_API void Flush() override;

			XY_API std::u16string GetName() const override;

			XY_API void ReadBytes(char *buffer, size_t limit) override;

			XY_API size_t Tell() const override;

			XY_API void Seek(long long offset, SeekMode mode) override;

			XY_API void Write(const char *buffer, size_t limit) override;

			// 通过 StreamBasic 继承
			XY_API void Close() override;

			XY_API bool IsEof() const noexcept override;

		};

	protected:
		xybase::Stream *infraStream;
		int align = 1;

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

		XY_API void Write(unsigned long long handle, const char *buffer, size_t limit);

		XY_API void ReadBytes(unsigned long long handle, char *buffer, size_t length);

		XY_API void Seek(unsigned long long handle, long long offset, Stream::SeekMode mode);

		XY_API size_t Tell(unsigned long long handle);

		XY_API void Close(unsigned long long handle);

		// 通过 FileContainer 继承
		XY_API bool Exists(std::u16string path) override;
	};
}


#endif // !XY_FILE_CONTAINER_BASIC_H__
