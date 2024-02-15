#ifndef REFERRENCE_FIELD_H__
#define REFERRENCE_FIELD_H__

#pragma once

#include "../mulert_api.h"
#include "Basic/Type.h"
#include "TypeCreator.h"
#include "TypeManager.h"
#include "../Logger.h"

namespace mule
{
	namespace Data
	{
		/**
		 * @brief 指向其他位置的32位引用类型。
		*/
		class MULERT_API Reference : public Basic::Type
		{
			/**
			 * @brief 内部的类型。
			*/
			Basic::Type *referent = nullptr;

			Logger logger = Logger::GetLogger<Reference>();
		public:
			class MULERT_API ReferenceCreator : public TypeCreator
			{
				virtual Basic::Type *DoCreateObject(const std::u16string &info) override;
				virtual Basic::Type *DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo) override;
			};

			virtual void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, Basic::Type::FileHandler *fileHandler) override;
			virtual size_t Size() const override;
			virtual std::u16string GetDataType() const override;
			virtual bool IsComposite() const override;

		private:
			Reference(Type *referent);

			// 通过 Type 继承
			void WriteValue(xybase::Stream *stream, mule::Data::Basic::MultiValue mv) override;
			mule::Data::Basic::MultiValue ReadValue(xybase::Stream *stream) override;
		};
	}
}

#endif
