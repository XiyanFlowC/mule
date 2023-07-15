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
			 * @brief �洢Ƭ�Σ����ڱ��һ������ҿ�����
			*/
			class Fragment
			{
				unsigned int begin, size;

			public:
				/**
				 * @brief �����´洢Ƭ�Σ�ָ������ʼ��ַ�ͳ���
				 * @param begin ��ʼ��ַ
				 * @param size ����
				*/
				Fragment(unsigned int begin, unsigned size);

				/**
				 * @brief �����´洢Ƭ��
				 * @param pattern �ɴ洢Ƭ��
				*/
				Fragment(const Fragment &pattern);

				/**
				 * @brief ��ȡƬ�ο�ʼ��ַ
				 * @return Ƭ�ο�ʼ��ַ
				*/
				unsigned int GetBegining() const;

				/**
				 * @brief ��ȡƬ�ν�����ַ���õ�ַ����Ƭ���У�
				 * @return Ƭ�ν�����ַ
				*/
				unsigned int GetEnding() const;

				/**
				 * @brief ��ȡƬ�δ�С
				 * @return Ƭ�δ�С
				*/
				unsigned int GetSize() const;

				/**
				 * @brief ����Ƭ������
				 * @param begining ��ʼ��ַ
				 * @param size ��С
				*/
				void SetFragment(unsigned int begining, unsigned int size);

				/**
				 * @brief ������ʼ��ַ
				 * @param begining ����������ʼ��ַ
				*/
				void SetBegining(unsigned int begining);

				/**
				 * @brief �����С
				 * @param size ��С
				*/
				void SetSize(unsigned int size);

				/**
				 * @brief ���������ַ
				 * @param ending ������ַ
				*/
				void SetEnding(unsigned int ending);

				/**
				 * @brief ����ͷ������ͷ������ָ����С
				 * @param size Ҫ�����Ĵ�С
				*/
				void EliminateBegining(unsigned int size);

				/**
				 * @brief ����β������β��ǰ��ָ����С
				 * @param size Ҫ�����Ĵ�С
				*/
				void EliminateEnding(unsigned int size);

				/**
				 * @brief ����Ƿ�����������ص�
				 * @param frag Ҫ�ȶԵ�����
				 * @return �������ص�������û��
				*/
				bool IsOverlapsWith(const Fragment &frag) const;

				/**
				 * @brief ����Ƿ����ָ������
				 * @param frag Ҫ�ȶԵ�Ƭ��
				 * @return �������������û��
				*/
				bool IsContains(const Fragment &frag) const;

				/**
				 * @brief ����Ƿ����ָ����ַ
				 * @param position Ҫ���ĵ�ַ
				 * @return �������������û��
				*/
				bool IsContains(unsigned int position) const;

				/**
				 * @brief ��ָ��Ƭ�κϲ�Ϊ�µ�Ƭ�Σ�������Ƭ�Σ�
				 * @param target Ҫ�ϲ���Ƭ��
				 * @return �´����ĺϲ���Ƭ�Σ�ʧ�ܷ��� nullptr
				*/
				Fragment *MergeToNew(const Fragment &target) const;

				/**
				 * @brief ������Ƭ�κϲ�����Ƭ����
				 * @param target Ҫ�ϲ���Ƭ��
				*/
				void Merge(const Fragment &target);

				/**
				 * @brief �ȶ�Ƭ���Ƿ����
				 * @param right Ҫ�ȶԵ�Ƭ��
				 * @return ������ȣ����򲻵�
				*/
				bool operator== (const Fragment &right) const;

				/**
				 * @brief �ȶ�Ƭ���Ƿ񲻵�
				 * @param right Ҫ�ȶԵ�Ƭ��
				 * @return ���򲻵ȣ��������
				*/
				bool operator!= (const Fragment &right) const;
			};
		}
	}
}

#endif
