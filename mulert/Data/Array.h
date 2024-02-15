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
		class MULERT_API Array : public Basic::Type
		{
		protected:
			std::u16string sizeCache;

			size_t length;

			Basic::Type *innerObject;

		public:
			class MULERT_API ArrayCreator : public TypeCreator
			{
			public:
				virtual Basic::Type *DoCreateObject(const std::u16string &info) override;
			};

			virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
			virtual size_t Size() const override;
			virtual std::u16string GetDataType() const override;

			virtual bool IsComposite() const override;

			void WriteValue(xybase::Stream *stream, mule::Data::Basic::MultiValue mv) override;
			mule::Data::Basic::MultiValue ReadValue(xybase::Stream *stream) override;
		};
	}
}

#endif
