#pragma once

#ifndef ARRAY_H__
#define ARRAY_H__

#include "../mulert_api.h"
#include <xyutils.h>
#include "Basic/Type.h"
#include "TypeCreator.h"
#include "TypeManager.h"

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

			Array();

		public:
			class MULERT_API ArrayCreator : public TypeCreator
			{
			public:
				virtual Basic::Type *DoCreateObject(const std::u16string &info) override;
			};

			MULERT_API virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
			MULERT_API virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
			MULERT_API virtual size_t Size() const override;
			MULERT_API virtual std::u16string GetDataType() const override;

			MULERT_API virtual bool IsComposite() const override;
		};
	}
}

#endif
