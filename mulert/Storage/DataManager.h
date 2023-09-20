#pragma once

#ifndef DATA_MANAGER_H__
#define DATA_MANAGER_H__

#include <string>
#include <map>

#include "BinaryData.h"

#include "../mulert_api.h"

namespace mule
{
	namespace Storage
	{
		class MULERT_API DataManager
		{

			int CreateDirectoryRecursively(std::string path, size_t index = 0);
		protected:

			struct FileInfo
			{
				unsigned int id;
				unsigned int crc32;
			};

			std::map<std::string, FileInfo> fileInfos;

			std::u16string dataPath;

			uint32_t currentId = 0x0100'0000;

		public:

			bool IsExist(unsigned int id);

			static DataManager &GetInstance();

			virtual ~DataManager();

			void Initialise(const std::u16string &datadir);

			BinaryData LoadData(std::string name);

			BinaryData LoadData(unsigned int id);

			FILE *OpenRaw(unsigned int id, bool create = false);

			unsigned int SaveData(const BinaryData &data, unsigned int id);

			unsigned int SaveData(const BinaryData &data);
		};
	}
}

#endif // !DATA_MANAGER_H__

