#pragma once

#ifndef SHIFTABLE_REFERRENCE_H__
#define SHIFTABLE_REFERRENCE_H__

#include <map>

#include <Data/Space/FragmentManager.h>
#include <Data/Basic/Type.h>
#include <Data/TypeCreator.h>
#include <Data/TypeManager.h>

class ShiftableString : public mule::Data::Basic::Type
{
protected:
	int GetAlign(size_t loc, xybase::Stream *stream);

public:

	class MemoryManager
	{
		std::map<std::u16string, mule::Data::Space::FragmentManager> memories;

		std::map<std::u16string, std::map<std::u16string, size_t>> assign;

	public:
		MemoryManager();

		void DisposeStream(xybase::Stream *stream);

		static MemoryManager &GetInstance();

		mule::Data::Space::FragmentManager &GetMemory(xybase::Stream *stream);

		size_t AssignFor(xybase::Stream *stream, const std::u16string &str, size_t size);

		void SaveFreeSpace();
	};

	class ShiftableStringCreator : public mule::Data::TypeCreator
	{
		virtual mule::Data::Basic::Type *DoCreateObject(std::u16string info) override;
	};

	virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
	virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
	virtual size_t Size() const override;

	// 通过 Type 继承
	virtual std::u16string GetTypeName() const override;

	void StreamDispose(xybase::Stream *stream);

	/**
	 * @brief 是否允许移动引用
	*/
	static bool doShift;
	/**
	 * @brief 对其字节数要求，若为0则自动获取
	*/
	static int align;
};

#endif // End of SHIFTABLE_REFERRENCE_H__
