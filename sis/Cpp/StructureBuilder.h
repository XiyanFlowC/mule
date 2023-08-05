#pragma once

#ifndef STRUCTURE_BUILDER_H__
#define STRUCTURE_BUILDER_H__

#include <string>
#include <list>
#include <tuple>
#include <Data/TypeManager.h>
#include <Data/Structure.h>

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

			mule::Data::Structure *Build();

			void NewStruct(std::string name);

			void AppendField(std::string type, std::string name);
		};
	}
}


#endif // !STRUCTURE_BUILDER_H__
