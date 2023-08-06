#ifndef TABLE_H__
#define TABLE_H__

#include <string>

#include "Structure.h"
#include "../xybase/Stream.h"

namespace mule {
	namespace Data {
		/**
		 * @brief ����дʱ�����ж�λ�׵�ַ��Ԫ�ء�
		*/
		class Table : public Basic::Type {
		public:
			/**
			 * @brief ĳ�������е�һ�����ݣ��䵥ԪΪ structure
			 * @param structure �ṹ
			 * @param name ����
			 * @param length ����
			 * @param offset �������е�ƫ��
			*/
			Table(Structure *structure, const std::string &name, int length, size_t offset);

			/**
			 * @brief ִ�ж�ȡ
			 * @param stream ��ȡ����
			 * @param dataHandler ���ݴ�����
			*/
			void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;

			/**
			 * @brief ִ��д��
			 * @param stream д�����
			 * @param dataHandler ���ݴ�����
			*/
			void Write(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;

			virtual size_t Size() const override;

			/**
			 * @brief ��ȡ��ĳ��ȣ�Ԫ�ظ�����
			 * @return Ԫ�صĸ���
			*/
			int Length() const;

			std::string GetTypeName() const override;

			bool IsComposite() const override;

		protected:
			Structure *structure;

			int length;
			size_t offset;
			std::string name;
		private:
		};
	}
}

#endif
