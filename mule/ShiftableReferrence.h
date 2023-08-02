#pragma once

#ifndef SHIFTABLE_REFERRENCE_H__
#define SHIFTABLE_REFERRENCE_H__

#include <map>

#include <Data/Space/FragmentManager.h>
#include <Data/Basic/Object.h>
#include <Data/ObjectCreator.h>
#include <Data/ObjectManager.h>

#include "ElfStream.h"

class ShiftableReferrence : public mule::Data::Basic::Object
{
protected:
	mule::Data::Basic::Object *referent;

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
	class ShiftableStringCreator : public mule::Data::ObjectCreator
	{
		virtual mule::Data::Basic::Object *DoCreateObject(std::string info) override;
	};

	virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
	virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
	virtual size_t Size() const override;

	// ͨ�� Object �̳�
	virtual std::string GetTypeName() const override;

	void StreamDispose(xybase::Stream *stream);

	/**
	 * @brief �Ƿ������ƶ�����
	*/
	static bool doShift;
	/**
	 * @brief �����ֽ���Ҫ����Ϊ0���Զ���ȡ
	*/
	static int align;
};

#endif // End of SHIFTABLE_REFERRENCE_H__