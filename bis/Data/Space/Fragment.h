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
			/**
			 * @brief 存储片段，用于标记一段左闭右开区间
			*/
			class Fragment
			{
				unsigned int begin, size;

			public:
				/**
				 * @brief 创建新存储片段，指定其起始地址和长度
				 * @param begin 起始地址
				 * @param size 长度
				*/
				Fragment(unsigned int begin, unsigned size);

				/**
				 * @brief 创建新存储片段
				 * @param pattern 旧存储片段
				*/
				Fragment(const Fragment &pattern);

				/**
				 * @brief 获取片段开始地址
				 * @return 片段开始地址
				*/
				unsigned int GetBegining() const;

				/**
				 * @brief 获取片段结束地址（该地址不在片段中）
				 * @return 片段结束地址
				*/
				unsigned int GetEnding() const;

				/**
				 * @brief 获取片段大小
				 * @return 片段大小
				*/
				unsigned int GetSize() const;

				/**
				 * @brief 设置片段区间
				 * @param begining 起始地址
				 * @param size 大小
				*/
				void SetFragment(unsigned int begining, unsigned int size);

				/**
				 * @brief 重设起始地址
				 * @param begining 重新设置起始地址
				*/
				void SetBegining(unsigned int begining);

				/**
				 * @brief 重设大小
				 * @param size 大小
				*/
				void SetSize(unsigned int size);

				/**
				 * @brief 重设结束地址
				 * @param ending 结束地址
				*/
				void SetEnding(unsigned int ending);

				/**
				 * @brief 消除头部，将头部后移指定大小
				 * @param size 要消除的大小
				*/
				void EliminateBegining(unsigned int size);

				/**
				 * @brief 消除尾部，将尾部前移指定大小
				 * @param size 要消除的大小
				*/
				void EliminateEnding(unsigned int size);

				/**
				 * @brief 检查是否和所给区段重叠
				 * @param frag 要比对的区段
				 * @return 真则有重叠，假则没有
				*/
				bool IsOverlapsWith(const Fragment &frag) const;

				/**
				 * @brief 检查是否包括指定区段
				 * @param frag 要比对的片段
				 * @return 真则包含，假则没有
				*/
				bool IsContains(const Fragment &frag) const;

				/**
				 * @brief 检查是否包括指定地址
				 * @param position 要检查的地址
				 * @return 真则包含，假则没有
				*/
				bool IsContains(unsigned int position) const;

				/**
				 * @brief 和指定片段合并为新的片段（创建新片段）
				 * @param target 要合并的片段
				 * @return 新创建的合并后片段，失败返回 nullptr
				*/
				Fragment *MergeToNew(const Fragment &target) const;

				/**
				 * @brief 将所给片段合并到此片段中
				 * @param target 要合并的片段
				*/
				void Merge(const Fragment &target);

				/**
				 * @brief 比对片段是否相等
				 * @param right 要比对的片段
				 * @return 真则相等，假则不等
				*/
				bool operator== (const Fragment &right) const;

				/**
				 * @brief 比对片段是否不等
				 * @param right 要比对的片段
				 * @return 真则不等，假则相等
				*/
				bool operator!= (const Fragment &right) const;
			};
		}
	}
}

#endif
