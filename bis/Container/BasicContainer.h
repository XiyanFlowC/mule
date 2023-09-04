#pragma once

#ifndef BASIC_CONTAINER_H__
#define BASIC_CONTAINER_H__

#include <stack>
#include <map>

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
	protected:

		xybase::Stream *file;

		template<typename T>
		T Read(int id)
		{
			if (locs[id] + sizeof(T) >= fdMap[id]->curSize)
				throw xybase::OutOfRangeException(u"Try to read after EOF.", __LINE__);
			if (file->Tell() != locs[id])
			{
				file->Seek(locs[id], 0);
			}
			T ret{};
			file->ReadBytes(reinterpret_cast<char *>(&ret), sizeof(T));
			locs[id] += sizeof(T);
			return ret;
		}

		void ReadBytes(int id, char *buffer, int size);

		template<typename T>
		void Write(int id, T data)
		{
			if (locs[id] + sizeof(T) >= fdMap[id]->size)
				throw xybase::OutOfRangeException(u"Write exceed maximum size.", __LINE__);
			if (file->Tell() != locs[id])
			{
				file->Seek(locs[id], 0);
			}
			file->Write(data);
			locs[id] += sizeof(T);
			fdMap[id]->curSize = std::max(fdMap[id]->curSize, locs[id]);
		}

		void Write(int id, const char *buffer, size_t size);

		void Close(int id);

	public:
		class InnerFile : public xybase::Stream
		{
			BasicContainer *host;

			int token;

			size_t size, offset;

#ifndef BIG_ENDIAN_SYSTEM
			static const bool bigEndianSystem = false;
#else
			static const bool bigEndianSystem = true;
#endif
			std::u16string name;
		public:

			bool isBigEndian = false;

			~InnerFile();

			InnerFile(int token, size_t size, size_t offset, BasicContainer *host, const std::u16string &name);

			virtual uint8_t ReadUInt8() override;
			virtual int8_t ReadInt8() override;
			virtual uint16_t ReadUInt16() override;
			virtual int16_t ReadInt16() override;
			virtual uint32_t ReadUInt32() override;
			virtual int32_t ReadInt32() override;
			virtual uint64_t ReadUInt64() override;
			virtual int64_t ReadInt64() override;
			virtual float ReadFloat() override;
			virtual double ReadDouble() override;
			virtual std::string ReadString() override;
			virtual void ReadBytes(char *buffer, int limit) override;
			virtual void Write(uint8_t value) override;
			virtual void Write(int8_t value) override;
			virtual void Write(uint16_t value) override;
			virtual void Write(int16_t value) override;
			virtual void Write(uint32_t value) override;
			virtual void Write(int32_t value) override;
			virtual void Write(uint64_t value) override;
			virtual void Write(int64_t value) override;
			virtual void Write(float value) override;
			virtual void Write(double value) override;
			virtual void Write(const std::string &value) override;
			virtual void Write(const char *buffer, size_t size) override;
			virtual size_t Tell() override;
			virtual void Seek(long long offset, int mode) override;
			virtual void Close() override;

			// 通过 Stream 继承
			virtual std::u16string GetName() override;
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

		virtual xybase::Stream *Open(std::u16string name, FileOpenMode mode) override;

		virtual std::list<std::u16string> List() override;

		virtual void MakeDir(std::u16string path) override;

		virtual void Remove(std::u16string target, bool recursive = false) override;

		void Close();

	private:
		std::map<int, FileDesc *> fdMap;

		// 通过 FileContainer 继承
		virtual std::u16string GetName() override;
};

	template<>
	void BasicContainer::Write<const std::string &>(int id, const std::string &value);
}

#endif // !ISO_CONTAINER_H__
