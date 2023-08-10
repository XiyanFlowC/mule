#pragma once

#ifndef RESOURCE_MANAGER_H__
#define RESOURCE_MANAGER_H__

#include <cstdio>

#include <map>
#include <string>
#include <memory>
#include <functional>

#include <xybase/Stream.h>
#include <xybase/Exception/IOException.h>
#include <BinaryStream.h>
#include <Data/TypeManager.h>
#include <Data/Structure.h>
#include <Xml/XmlParser.h>
#include <Xml/XmlNode.h>

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
	 * @brief 管理对应数据
	 * @param data 数据
	 * @param length 数据长度
	 * @param duplicate 是否复制（不对源操作）
	*/
	void SetData(char *data, size_t length, bool duplicate = true);

	/**
	 * @brief 管理对应数据
	 * @param data 数据
	 * @param length 数据长度
	*/
	void SetData(const char *data, size_t length);
};

class ResourceManager
{
	mule::Xml::XmlParser<mule::Xml::XmlNode> xmlParser;

	ResourceManager();
public:
	struct FileInfo
	{
		unsigned int id;
		unsigned int crc32;
	};

	std::map<std::string, FileInfo> fileInfos;

	static ResourceManager &GetInstance();

	BinaryData LoadData(std::string name);

	BinaryData LoadData(unsigned int id);

	unsigned int SaveData(BinaryData &data, unsigned int assignId = 0);

	bool IsExist(unsigned int id);

	bool IsExist(std::string name);

	BinaryData LoadResource(std::string path);

	xybase::Stream *OpenResource(std::string path, std::function<xybase::Stream *(std::string path)> creator);

	xybase::Stream *OpenData(unsigned int id, std::function<xybase::Stream *(std::string path)> creator);

	xybase::Stream *OpenData(std::string name, std::function<xybase::Stream *(std::string path)> creator);

	void LoadDefinition(std::string def);

	std::string LoadSheet(std::string sheetName);

	void SaveSheet(std::string sheetName, std::string sheet);
};

#endif
