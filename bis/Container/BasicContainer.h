#pragma once

#ifndef BASIC_CONTAINER_H__
#define BASIC_CONTAINER_H__

#include <stack>
#include <map>

#include <StreamBasic.h>
#include <FileContainer.h>
#include <StringBuilder.h>
#include <Exception/OutOfRangeException.h>

namespace mule
{
	class BasicContainer : public xybase::FileContainer
	{
		std::stack<std::u16string> dir;

		std::map<int, size_t> locs;

		int curId = 0;

		std::u16string name;

		enum {
			BC_IDLE,
			BC_READ,
			BC_WRITE
		} state;
	protected:

		xybase::Stream *file;

		void ReadBytes(int id, char *buffer, size_t size);

		void Write(int id, const char *buffer, size_t size);

		void Close(int id);

	public:
		class InnerFile : public xybase::StreamBasic
		{
			BasicContainer *host;

			int handle;

			size_t size, offset;

			std::u16string name;
		public:

			bool isBigEndian = false;

			~InnerFile();

			InnerFile(int handle, size_t size, size_t offset, BasicContainer *host, const std::u16string &name);

			virtual void ReadBytes(char *buffer, size_t limit) override;
			virtual void Write(const char *buffer, size_t size) override;
			virtual size_t Tell() const override;
			virtual void Seek(long long offset, SeekMode mode) override;
			virtual void Close() override;
			virtual std::u16string GetName() const override;
			virtual void Flush() override;
		};

		struct FileDesc
		{
			size_t offset;
			size_t size;
			size_t curSize;
			bool occupied;
		};

		std::map<std::u16string, FileDesc> fileIndices;

		BasicContainer(xybase::Stream *stream);

		virtual xybase::Stream *Open(std::u16string name, xybase::FileOpenMode mode) override;

		virtual std::list<std::u16string> List() override;

		virtual void MakeDir(std::u16string path) override;

		virtual void Remove(std::u16string target, bool recursive = false) override;

		void Close();

	private:
		std::map<int, FileDesc *> fdMap;

		// 通过 FileContainer 继承
		virtual std::u16string GetName() override;

		// 通过 FileContainer 继承
		virtual void Flush() override;
	};
}

#endif // !ISO_CONTAINER_H__
