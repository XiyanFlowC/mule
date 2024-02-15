#pragma once

#ifndef FRAGMENT_MANAGER_H__
#define FRAGMENT_MANAGER_H__

#include <list>

#include "Fragment.h"

#include "../../mulert_api.h"

namespace mule
{
	namespace Data
	{
		namespace Space
		{

#ifndef cal_align
#define cal_align(loc, align) (((loc) < 0) ? ((loc) & ~(align - 1)) : ((loc + (align - 1)) & ~(align - 1)))
#endif
			/**
			 * @brief 用于管理片段的类
			 * @deprecated
			*/
			class [[deprecated("Use xybase::Fragment::FragmentManager instead.")]] MULERT_API FragmentManager
			{
				std::list<Fragment *> frags;


			public:
				/**
				 * @brief Create a new, empty fragment manager.
				*/
				FragmentManager();

				~FragmentManager();

				const std::list<Fragment *> &GetFragments() const;

				/**
				 * @brief Register a fragment to the manager.
				 * @param frag The fragment need to be registered.
				*/
				void RegisterFragment(const Fragment &frag);

				/**
				 * @brief Register a fragment to the manager;
				 * @param position The position where the fragment located.
				 * @param size The size of the fragment.
				*/
				void RegisterFragment(unsigned int position, unsigned int size);

				/**
				 * @brief Sort the fragments registered in the manager. Merge all mergeable fragments.
				*/
				void Defragment();

				/**
				 * @brief Check if the specified position is free.
				 * @param position The position that want to check.
				 * @return bool true if it is free, false otherwise.
				*/
				bool IsFree(unsigned int position);

				/**
				 * @brief Check if the specified fragment is free.
				 * @param frag the Fragment that want to check.
				 * @return bool true if it is free, false otherwise.
				*/
				bool IsFree(Fragment *frag);

				/**
				 * @brief Try to allocate a fragment for use.
				 * @param size The size need to allocate.
				 * @param align Specifing if the fragment should be aligned.
				 * @return
				*/
				unsigned int Alloc(size_t size, int align);
			};
		}
	}
}

#endif
