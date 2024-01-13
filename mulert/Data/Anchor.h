#pragma once

#include "../mulert_api.h"
#include "Basic/Type.h"

namespace mule
{
	namespace Data
	{
		/**
		 * @brief 锚点。保存读取开始偏移地址的类。读写前会进行寻址。
		*/
		class MULERT_API Anchor : Basic::Type
		{
			Basic::Type *infraType;
			size_t offset;
			std::u16string name;
		public:
			Anchor(Basic::Type *infraType, size_t offset, const std::u16string &name);
			size_t Size() const override;
			std::u16string GetDataType() const override;
			void Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler) override;
			void Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler) override;
		};
	}
}
