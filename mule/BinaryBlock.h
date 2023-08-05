#pragma once

#ifndef BINARY_BLOCK_H__
#define BINARY_BLOCK_H__

#include <Data/Basic/Type.h>
#include <Data/TypeCreator.h>

class BinaryBlock : public mule::Data::Basic::Type
{
public:
	class BinaryBlockCreator : public mule::Data::TypeCreator
	{
	public:
		virtual mule::Data::Basic::Type *DoCreateObject(std::string info) override;
	};
	virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
	virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
	virtual size_t Size() const override;
	virtual std::string GetTypeName() const override;
};

#endif
