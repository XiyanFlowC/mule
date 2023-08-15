#ifndef TABLE_H__
#define TABLE_H__

#include <string>

#include "Structure.h"
#include "../xybase/Stream.h"

namespace mule {
	namespace Data {
		/**
		 * @brief 表。读写时在流中定位首地址的元素。
		*/
		class Table : public Basic::Type {
		public:
			/**
			 * @brief 某个在流中的一列数据，其单元为 structure
			 * @param structure 结构
			 * @param name 表名
			 * @param length 表长度
			 * @param offset 表在流中的偏移
			*/
			Table(Structure *structure, const std::u16string &name, int length, size_t offset);

			/**
			 * @brief 执行读取
			 * @param stream 读取的流
			 * @param dataHandler 数据处理器
			*/
			void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;

			/**
			 * @brief 执行写入
			 * @param stream 写入的流
			 * @param dataHandler 数据处理器
			*/
			void Write(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;

			virtual size_t Size() const override;

			/**
			 * @brief 获取表的长度（元素个数）
			 * @return 元素的个数
			*/
			int Length() const;

			std::u16string GetTypeName() const override;

			bool IsComposite() const override;

		protected:
			Structure *structure;

			int length;
			size_t offset;
			std::u16string name;
		private:
		};
	}
}

#endif
