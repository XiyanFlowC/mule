#pragma once

#ifndef TYPE_CREATOR_H__
#define TYPE_CREATOR_H__

#include <string>

#include "Basic/Type.h"

#include "../mulert_api.h"

namespace mule
{
	namespace Data
	{

		/**
		 * @brief 字段创建器，ObjectManager用
		*/
		class MULERT_API TypeCreator
		{
		public:
			virtual ~TypeCreator();

			TypeCreator();

			TypeCreator(TypeCreator *next);

			TypeCreator(TypeCreator &pattern);

			mule::Data::TypeCreator *nextCreator;

			/**
			 * @brief Append a creator to the end of the chain.
			 * @param nextCreator The creator needs to be appended.
			 */
			void AppendCreator(mule::Data::TypeCreator *nextCreator);

			/**
			 * @brief Try to create a field handler by given information.
			 * @param info
			 * @return The created Type, nullptr if failed to create.
			*/
			mule::Data::Basic::Type *CreateType(const std::u16string & info);

			/**
			 * @brief Try to create a type handler by given information.
			 * @param name The name of the type
			 * @param extraInfo The extra creation information
			 * @return The created Type, nullptr if failed to create.
			*/
			mule::Data::Basic::Type *CreateType(const std::u16string & info, const std::map<std::u16string, std::u16string> & metainfo);

		protected:
			// Return nullptr so that flow can move to the next ring of chain-of-responsibility
			virtual mule::Data::Basic::Type *DoCreateObject(const std::u16string &info) = 0;

			// Return nullptr so that flow can move to the next ring of chain-of-responsibility
			virtual mule::Data::Basic::Type *DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> & metainfo);
		};
	}
}

#endif
