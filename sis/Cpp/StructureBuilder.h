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
			std::u16string name;

			std::list<std::tuple<std::u16string, std::u16string>> defList;

		public:
			StructureBuilder(std::u16string name);

			mule::Data::Structure *Build();

			void NewStruct(std::u16string name);

			void AppendField(std::u16string type, std::u16string name);
		};
	}
}


#endif // !STRUCTURE_BUILDER_H__
