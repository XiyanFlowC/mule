#ifndef TABLE_H__
#define TABLE_H__

#include <string>

#include "Basic/Structure.h"
#include "../xybase/Stream.h"

namespace mule {
	namespace Data {
		/**
		 * @brief ����дʱ�����ж�λ�׵�ַ��Ԫ�ء�
		*/
		class Table : public Basic::Object {
		public:
			/**
			 * @brief ĳ�������е�һ�����ݣ��䵥ԪΪ structure
			 * @param structure �ṹ
			 * @param name ����
			 * @param length ����
			 * @param offset �������е�ƫ��
			*/
			Table(Basic::Structure *structure, const std::string &name, int length, size_t offset);

			/**
			 * @brief ִ�ж�ȡ
			 * @param stream ��ȡ����
			 * @param dataHandler ���ݴ�����
			*/
			void Read(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) const override;

			/**
			 * @brief ִ��д��
			 * @param stream д�����
			 * @param dataHandler ���ݴ�����
			*/
			void Write(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) const override;

			virtual size_t Size() const override;

			/**
			 * @brief ��ȡ��ĳ��ȣ�Ԫ�ظ�����
			 * @return Ԫ�صĸ���
			*/
			int Length() const;

			std::string GetTypeName() const override;

		protected:
			Basic::Structure *structure;

			int length;
			size_t offset;
			std::string name;
		private:
		};
	}
}

#endif
