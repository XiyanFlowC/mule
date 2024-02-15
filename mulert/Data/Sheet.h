#pragma once

#include "Basic/Type.h"

namespace mule
{
	namespace Data
	{
		class Sheet : Basic::Type
		{
			Basic::Type *infraType;
			size_t length;
			size_t offset;
			std::u16string name;
		public:
			size_t Size() const override;
			std::u16string GetDataType() const override;
			bool IsComposite() const override;
			void Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler) override;
			void Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler) override;
		};
	}
}
