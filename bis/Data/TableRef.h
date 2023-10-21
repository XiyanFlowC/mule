#pragma once

#ifndef TABLE_REF_H__
#define TABLE_REF_H__

#include <Data/Basic/Type.h>
#include <Data/TypeCreator.h>

namespace mule
{
	namespace Data
	{
		class TableRef : public mule::Data::Basic::Type
		{
		protected:
			mule::Data::Basic::Type *infraType;

			std::u16string locCacheName, sizeCacheName;

		public:
			virtual size_t Size() const override;
			virtual std::u16string GetDataType() const override;
			virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
			virtual bool IsComposite() const override;

			class TableRefCreator : public TypeCreator
			{
				virtual Basic::Type *DoCreateObject(const std::u16string &info) override;
			};
		};
	}
}

#endif
