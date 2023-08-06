#pragma once

#ifndef RESOURCE_MANAGER_H__
#define RESOURCE_MANAGER_H__

#include <cstdio>

#include <map>
#include <string>
#include <memory>
#include <functional>

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

	const char *LoadData();

	size_t GetLength();

	/**
	 * @brief �����Ӧ����
	 * @param data ����
	 * @param length ���ݳ���
	 * @param duplicate �Ƿ��ƣ�����Դ������
	*/
	void SaveData(char *data, size_t length, bool duplicate = true);

	/**
	 * @brief �����Ӧ����
	 * @param data ����
	 * @param length ���ݳ���
	*/
	void SaveData(const char *data, size_t length);
};

class ResourceManager
{
public:
	std::map<std::string, unsigned int> dataNameMap;

	ResourceManager &GetInstance();

	BinaryData LoadData(std::string name);

	BinaryData LoadData(unsigned int id);

	unsigned int SaveData(BinaryData &data, unsigned int assignId = 0);

	bool IsExist(unsigned int id);

	bool IsExist(std::string name);

	BinaryData LoadResource(std::string path);

	xybase::Stream *OpenResource(std::string path, std::function<xybase::Stream *(std::string path)> creator);

	xybase::Stream *OpenData(unsigned int id, std::function<xybase::Stream *(std::string path)> creator);

	xybase::Stream *OpenData(std::string name, std::function<xybase::Stream *(std::string path)> creator);
};

#endif
