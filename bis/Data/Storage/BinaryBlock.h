#pragma once

#ifndef BINARY_BLOCK_H__
#define BINARY_BLOCK_H__

#include "../Basic/Type.h"
#include "../TypeCreator.h"

namespace mule
{
	namespace Data
	{
		namespace Storage
		{
			class BinaryBlock : public mule::Data::Basic::Type
			{
			public:
				class BinaryBlockCreator : public mule::Data::TypeCreator
				{
				public:

					virtual mule::Data::Basic::Type *DoCreateObject(std::u16string info) override;
				};
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual size_t Size() const override;
				virtual std::u16string GetTypeName() const override;
			protected:
				size_t size;
			};
		}
	}
}

#endif // !BINARY_BLOCK_H__
