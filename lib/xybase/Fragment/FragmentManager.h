#pragma once

#ifndef XY_FRAGMENT_MANAGER_H__
#define XY_FRAGMENT_MANAGER_H__

#include <list>

#include "Fragment.h"

#include "../xyapi.h"

namespace xybase
{
	namespace Fragment
	{

#ifndef cal_align
#define cal_align(loc, align) (((loc) < 0) ? ((loc) & ~(align - 1)) : ((loc + (align - 1)) & ~(align - 1)))
#endif

		class FragmentManager
		{
			std::list<Fragment *> frags;

		public:
			/**
			 * @brief 创建新的空碎片管理器。
			*/
			XY_API FragmentManager();

			XY_API virtual ~FragmentManager();

			XY_API const std::list<Fragment *> &GetFragments() const;

			/**
			 * @brief 注册一个碎片。
			 * @param frag 要注册的碎片。
			*/
			XY_API void RegisterFragment(const Fragment &frag);

			/**
			 * @brief 初始化一个碎片并注册。
			 * @param position 碎片处在的位置。
			 * @param size 碎片的大小。
			*/
			XY_API void RegisterFragment(size_t position, size_t size);

			/**
			 * @brief 碎片整理。
			*/
			XY_API void Defragment();

			/**
			 * @brief 检查指定的位置是否是空闲。
			 * @param position 要检查的位置。
			 * @return bool 若空闲则为真，否则为假。
			*/
			XY_API bool IsFree(size_t position);

			/**
			 * @brief 检查指定的区段是否空闲。
			 * @param frag 要检查的区段。
			 * @return bool 若空闲则为真，否则为假。
			*/
			XY_API bool IsFree(Fragment *frag);

			/**
			 * @brief 尝试分配一段空间。
			 * @param size 要分配的大小。
			 * @param align 要求的对齐参数。
			 * @return 分配成功的空间起始地址。
			*/
			XY_API size_t Alloc(size_t size, int align);

			/**
			 * @brief 将最大的碎片分配掉。（不定大小分配用）
			 * @return 最大的碎片。
			*/
			XY_API Fragment AllocMaximumFragment();
		};
	}
}

#endif
