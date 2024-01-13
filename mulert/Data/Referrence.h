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
		class MULERT_API Referrence : public Basic::Type
		{
			/**
			 * @brief 内部的类型。
			*/
			Basic::Type *referent = nullptr;

			Logger logger = Logger::GetLogger<Referrence>();
		public:
			class MULERT_API ReferrenceCreator : public TypeCreator
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
			Referrence(Type *referent);
		};
	}
}

#endif
