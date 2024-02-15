#pragma once

#ifndef BINARY_BLOCK_H__
#define BINARY_BLOCK_H__

#include <Data/Basic/Type.h>
#include <Data/TypeCreator.h>

namespace mule
{
	namespace Data
	{
		namespace Storage
		{
			class BinaryBlock : public mule::Data::Basic::Type
			{
				BinaryBlock(size_t size);
			public:
				class BinaryBlockCreator : public mule::Data::TypeCreator
				{
				public:

					virtual mule::Data::Basic::Type *DoCreateObject(const std::u16string &info) override;
				};
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
				virtual size_t Size() const override;
				virtual std::u16string GetDataType() const override;

				virtual mule::Data::Basic::MultiValue ReadValue(xybase::Stream *stream) override;
				virtual void WriteValue(xybase::Stream *stream, mule::Data::Basic::MultiValue value) override;
			protected:
				size_t size;
			};
		}
	}
}

#endif // !BINARY_BLOCK_H__
