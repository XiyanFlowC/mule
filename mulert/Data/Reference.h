#ifndef REFERRENCE_FIELD_H__
#define REFERRENCE_FIELD_H__

#pragma once

#include <map>
#include <set>
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
		class Reference : public Basic::Type
		{
			/**
			 * @brief 内部的类型。
			*/
			Basic::Type *referent = nullptr;

			Logger logger = Logger::GetLogger<Reference>();

			Reference(Type *referent);

			class ReferenceRegistry
			{
				ReferenceRegistry();

				std::map<std::u16string, std::set<size_t>> records;

			public:
				static ReferenceRegistry &GetInstance();

				void RemoveStream(xybase::Stream *sender);

				bool IsRegistered(xybase::Stream *stream, size_t loc);

				void Register(xybase::Stream *stream, size_t loc);
			};
		public:
			class MULERT_API ReferenceCreator : public TypeCreator
			{
				virtual Basic::Type *DoCreateObject(const std::u16string &info) override;
				virtual Basic::Type *DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo) override;
			};

			MULERT_API virtual void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;
			MULERT_API virtual void Write(xybase::Stream *stream, Basic::Type::FileHandler *fileHandler) override;
			MULERT_API virtual size_t Size() const override;
			MULERT_API virtual std::u16string GetDataType() const override;
			MULERT_API virtual bool IsComposite() const override;
		};
	}
}

#endif
