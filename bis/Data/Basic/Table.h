#ifndef TABLE_H__
#define TABLE_H__

#include <string>

#include "Structure.h"
#include "DataHandler.h"
#include "../../xybase/Stream.h"

namespace mule {
	namespace Data {
		namespace Basic {
			/**
			 * @brief ����дʱ�����ж�λ�׵�ַ��Ԫ�ء�
			*/
			class Table {
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
				void Read(xybase::Stream *stream, DataHandler *dataHandler);

				/**
				 * @brief ִ��д��
				 * @param stream д�����
				 * @param dataHandler ���ݴ�����
				*/
				void Write(xybase::Stream *stream, DataHandler *dataHandler);

			protected:
				Structure *structure;

				int length;
				size_t offset;
				std::string name;
			private:
			};
		}
	}
}

#endif
