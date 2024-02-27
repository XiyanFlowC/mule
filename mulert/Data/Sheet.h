#pragma once

#include "Basic/Type.h"

namespace mule
{
	namespace Data
	{
		class Sheet : public Basic::Type
		{
			Basic::Type *infraType;
			size_t length;
			size_t offset;
			std::u16string name;
			int structureSimplifySuppression;
		public:
			MULERT_API Sheet(Basic::Type *infraType, size_t offset, size_t length, std::u16string name);

			MULERT_API const std::u16string &GetName() const;
			MULERT_API size_t Size() const override;
			MULERT_API std::u16string GetDataType() const override;
			MULERT_API bool IsComposite() const override;
			MULERT_API void Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler) override;
			MULERT_API void Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler) override;
		};
	}
}
