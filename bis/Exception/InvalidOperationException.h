#pragma once

#ifndef INVALID_OPERATION_H__
#define INVALID_OPERATION_H__

#include "Exception.h"
#include <string>

namespace mule
{
	namespace Exception
	{

		/**
		 * @brief ��Ч������
		 * ��ʾ������ǰ״̬�����ԭ����޷�����ָ���������쳣��
		*/
		class InvalidOperationException : public Exception
		{
		public:
			/**
			 * @brief ��ʼ����Ч�����쳣��
			 * @param description ���쳣��Ϣ��������
			 * @param file �׳��쳣���ļ���
			 * @param line �׳��쳣���кš�
			*/
			InvalidOperationException(std::string description, const char *file, int line);
		};
	}
}

#endif
