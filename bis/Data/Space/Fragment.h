#pragma once

#ifndef FRAGMENT_H__
#define FRAGMENT_H__

#include "../../Exception/InvalidParameterException.h"

namespace mule
{
	namespace Data
	{
		namespace Space
		{

			class Fragment
			{
				unsigned int begin, size;

			public:
				Fragment(unsigned int begin, unsigned size);

				Fragment(const Fragment &pattern);

				unsigned int GetBegining() const;

				unsigned int GetEnding() const;

				unsigned int GetSize() const;

				void SetFragment(unsigned int begining, unsigned int size);

				void SetBegining(unsigned int begining);

				void SetSize(unsigned int size);

				void SetEnding(unsigned int ending);

				void EliminateBegining(unsigned int size);

				void EliminateEnding(unsigned int size);

				bool IsOverlapsWith(const Fragment &frag) const;

				bool IsContains(const Fragment &frag) const;

				bool IsContains(unsigned int position) const;

				Fragment *MergeToNew(const Fragment &target) const;

				void Merge(const Fragment &target);

				bool operator== (const Fragment &right) const;

				bool operator!= (const Fragment &right) const;
			};
		}
	}
}

#endif
