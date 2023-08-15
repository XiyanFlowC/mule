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
		 * @brief Singleton Type Manager. Used to manage all of the objects in the enviorment.
		 * The other compoments should get registered objects from this class.
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
			void RegisterObject(Basic::Type *object, std::string name);

			/**
			 * @brief Unregister the object specified by its name.
			 * @param name The name of the object need to be unregistered.
			*/
			void UnregisterObject(std::string name);

			/**
			 * @brief Release the object by its name. This method will also delete the pointer.
			 * @param name The name of the object need to be released.
			*/
			void ReleaseObject(std::string name);

			/**
			 * @brief Register type creator to the manager. This class
			 * is able to manage an automatic process of Type creation by using the creators registered.
			 * @param creator The creator(s) need to be registered to the manager.
			*/
			void RegisterObjectCreator(TypeCreator *creator);

			/**
			 * @brief Try to create a Field by given information. Using the creators registered to the
			 * manager. If failed, this method will return nullptr.
			 * @param info The information needed to create the Field.
			 * @return Created Field instance. nullptr if failed.
			*/
			Basic::Type *GetOrCreateObject(std::string info);

			/**
			 * @brief Try to get a object that already registered to the manager.
			 * @param name The name of the object given when registered.
			 * @return
			*/
			Basic::Type *GetObject(std::string name);
		protected:
			std::map<std::string, Basic::Type *> objects;
			TypeCreator *first, *last;
		};
	}
}

#endif
