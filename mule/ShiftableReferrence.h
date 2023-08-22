#pragma once

#ifndef SHIFTABLE_REFERRENCE_H__
#define SHIFTABLE_REFERRENCE_H__

#include <map>

#include <Data/Space/FragmentManager.h>
#include <Data/Basic/Type.h>
#include <Data/TypeCreator.h>
#include <Data/TypeManager.h>

#include "ElfStream.h"

class ShiftableReferrence : public mule::Data::Basic::Type
{
protected:
	mule::Data::Basic::Type *referent;

	class MemoryManager
	{
		std::map<xybase::Stream *, mule::Data::Space::FragmentManager> memories;
	public:
		void DisposeStream(xybase::Stream *stream);

		static MemoryManager &GetInstance();

		mule::Data::Space::FragmentManager GetMemory(xybase::Stream * stream);
	};

	int GetAlign(size_t loc, xybase::Stream *stream);

public:
	class ShiftableReferrenceCreator : public mule::Data::TypeCreator
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
