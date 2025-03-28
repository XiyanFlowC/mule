#pragma once

#include <Data/Basic/BasicType.h>

#include <Data/TypeCreator.h>

namespace mule
{
	namespace Data
	{

		/**
		 * @brief 基本字段创建器，用于创建整形、浮点型等基础读写器。
		*/
		class BasicTypeCreator : public TypeCreator
		{
			virtual Basic::Type *DoCreateObject(const std::u16string &info) override;

			virtual Basic::Type *DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo);
		};
	}
}