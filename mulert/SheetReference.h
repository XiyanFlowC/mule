#pragma once

#include "mulert_api.h"
#include "SheetManager.h"
#include "Data/TypeCreator.h"
#include "Logger.h"

namespace mule
{
	class SheetReference : public Data::Basic::Type
	{
		Logger logger = Logger::GetLogger<SheetReference>();
	protected:
		std::u16string locCacheName, sizeCacheName;
		size_t sizeDefined;

		Data::Basic::Type *infraType;

		std::u16string namePattern;

		std::u16string GenerateName(size_t offset, std::u16string streamName);
	public:
		MULERT_API virtual ~SheetReference();
		MULERT_API size_t Size() const override;
		MULERT_API std::u16string GetDataType() const override;
		MULERT_API void Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler) override;
		MULERT_API void Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler) override;

		class MULERT_API SheetReferenceCreator : public mule::Data::TypeCreator
		{
		public:
			mule::Data::Basic::Type *DoCreateObject(const std::u16string &info) override;

			// Return nullptr so that flow can move to the next ring of chain-of-responsibility
			virtual mule::Data::Basic::Type *DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo);
		};
	};
}
