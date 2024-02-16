#pragma once

#include "Basic/Type.h"

namespace mule
{
	namespace Data
	{
		class MULERT_API Sheet : public Basic::Type
		{
			Basic::Type *infraType;
			size_t length;
			size_t offset;
			std::u16string name;
		public:
			Sheet(Basic::Type *infraType, size_t offset, size_t length, std::u16string name);

			const std::u16string &GetName() const;
			size_t Size() const override;
			std::u16string GetDataType() const override;
			bool IsComposite() const override;
			void Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler) override;
			void Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler) override;
		};
	}
}
