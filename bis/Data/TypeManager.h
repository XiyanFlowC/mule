#pragma once

#ifndef TYPE_MANAGER_H__
#define TYPE_MANAGER_H__

#include "Basic/Type.h"
#include "Structure.h"
#include "TypeCreator.h"
#include <map>
#include <string>

namespace mule
{
	namespace Data
	{
		/**
		 * @brief Singleton Type Manager. Used to manage all of the objects in the environment.
		 * The other components should get registered objects from this class.
		*/
		class TypeManager
		{
			TypeManager();
			~TypeManager();
		public:
			/**
			 * @brief Get the instance of this class.
			 * @return The instance of this class.
			*/
			static TypeManager &GetInstance();

			/**
			 * @brief Register given object to manager with name specified. You can drop the pointer
			 * if you want to use ReleaseObject() to remove it from the manager.
			 * @param object The pointer to the object need to be register.
			 * @param name The name of this object.
			*/
			void RegisterObject(Basic::Type *object, std::u16string name);

			/**
			 * @brief Unregister the object specified by its name.
			 * @param name The name of the object need to be unregistered.
			*/
			void UnregisterObject(std::u16string name);

			/**
			 * @brief Release the object by its name. This method will also delete the pointer.
			 * @param name The name of the object need to be released.
			*/
			void ReleaseObject(std::u16string name);

			/**
			 * @brief Register type creator to the manager. This class
			 * is able to manage an automatic process of Type creation by using the creators registered.
			 * @param creator The creator(s) need to be registered to the manager.
			*/
			void RegisterObjectCreator(TypeCreator *creator);

			/**
			 * @brief Try to create a Field by given information. Using the creators registered to the
			 * manager. If failed, this method will return nullptr.
			 * @param name The name of the type.
			 * @return Created Field instance. nullptr if failed.
			*/
			Basic::Type *GetOrCreateType(std::u16string name);

			/**
			 * @brief 尝试获取一个已经创建的 Type 或者创建一个一个新的。注意若 meta-info 不为空，则总是创建新的。
			 * @param name Type 的名字。
			 * @param metainfo 修饰 Type 的元信息。
			 * @return 创建的 Type，若失败则为 nullptr。
			*/
			Basic::Type *GetOrCreateType(std::u16string name, const std::map<std::u16string, std::u16string> &metainfo);

			/**
			 * @brief Try to get a object that already registered to the manager.
			 * @param name The name of the object given when registered.
			 * @return
			*/
			Basic::Type *GetType(std::u16string name);
		protected:
			std::map<std::u16string, Basic::Type *> objects;
			TypeCreator *first, *last;
		};
	}
}

#endif
