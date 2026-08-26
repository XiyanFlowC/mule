#pragma once

#include <string>

/**
 * @brief 元数据键常量（mule::Data::Basic::MultiValue::metadata 的键）。
 */
namespace mule::Data::MetadataKey
{
	/// 引用的目标地址（32 位偏移）
	inline const std::u16string Ptr = u"ptr";
	/// 引用目标的大小（字节）
	inline const std::u16string Size = u"size";
	/// 引用重新分配时的对齐要求
	inline const std::u16string Align = u"align";
	/// 是否需要重新分配空间（非 0 整数，或字符串 "true"）
	inline const std::u16string Realloc = u"realloc";
	/// 数组/序列的元素索引（XmlHandler 等标记元素序号用）
	inline const std::u16string Index = u"i";
}
