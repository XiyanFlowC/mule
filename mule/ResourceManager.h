#pragma once

#ifndef RESOURCE_MANAGER_H__
#define RESOURCE_MANAGER_H__

#include <cstdio>

#include <map>
#include <string>
#include <memory>

#include <xybase/Stream.h>
#include <BinaryStream.h>

#include "Configuration.h"
#include "crc32.h"

class BinaryData
{
	std::shared_ptr<char[]> data;
	size_t length;

public:

	BinaryData(char *data, size_t length, bool duplicate = true);

	const char *GetData();

	size_t GetLength();

	/**
	 * @brief �����Ӧ����
	 * @param data ����
	 * @param length ���ݳ���
	 * @param duplicate �Ƿ��ƣ�����Դ������
	*/
	void SetData(char *data, size_t length, bool duplicate = true);

	/**
	 * @brief �����Ӧ����
	 * @param data ����
	 * @param length ���ݳ���
	*/
	void SetData(const char *data, size_t length);
};

class ResourceManager
{

public:
	std::map<std::string, int> dataNameMap;

	ResourceManager &GetInstance();

	BinaryData GetData(std::string name);

	BinaryData GetData(unsigned int id);

	unsigned int SetData(BinaryData &data);
};

#endif
