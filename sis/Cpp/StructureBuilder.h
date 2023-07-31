#pragma once

#ifndef STRUCTURE_BUILDER_H__
#define STRUCTURE_BUILDER_H__

#include <string>
#include <list>
#include <tuple>
#include <Data/ObjectManager.h>
#include <Data/Basic/Structure.h>

namespace mule
{
	namespace Cpp
	{
		class StructureBuilder
		{
			std::string name;

			std::list<std::tuple<std::string, std::string>> defList;

		public:
			StructureBuilder(std::string name);

			mule::Data::Basic::Structure *Build();

			void AppendField(std::string type, std::string name);
		};
	}
}


#endif // !STRUCTURE_BUILDER_H__
