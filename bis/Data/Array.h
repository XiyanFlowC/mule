#pragma once

#ifndef ARRAY_H__
#define ARRAY_H__

#include <xyutils.h>
#include <Data/Basic/Type.h>
#include <Data/TypeCreator.h>
#include <Data/TypeManager.h>

namespace mule
{
	namespace Data
	{
		class Array : public Basic::Type
		{
		protected:
			std::u16string sizeCache;

			size_t length;

			Basic::Type *innerObject;

		public:
			class ArrayCreator : public TypeCreator
			{
			public:
				virtual Basic::Type *DoCreateObject(std::u16string info) override;
			};

			virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
			virtual size_t Size() const override;
			virtual std::u16string GetDataType() const override;

			virtual bool IsComposite() const override;
		};
	}
}

#endif
