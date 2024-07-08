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
		/**
		 * @brief Binary internal data manager. Used to store/load binary data to/from disc.
		 * Internal persistence system should use this class to access data file.
		 */
		class DataManager
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

			DataManager();
		public:
			/**
			 * @brief Is data file of specified id exists?
			 * @param id Specified id
			 * @return true if exist, otherwise false.
			 */
			MULERT_API bool IsExist(unsigned int id);

			/**
			 * @brief Get the instance.
			 * @return 
			 */
			MULERT_API static DataManager &GetInstance();

			MULERT_API virtual ~DataManager();

			/**
			 * @brief Set data root directory.
			 * @param datadir The root directory.
			 */
			MULERT_API void SetDataDir(const std::u16string &datadir);

			/**
			 * @brief Load a data into memory.
			 * @param name File name.
			 * @return Loaded data.
			 * @deprecated
			 */
			MULERT_API mule::Storage::BinaryData LoadData(std::string name);

			/**
			 * @brief Load a data file into memory.
			 * @param id File id.
			 * @return Loaded data.
			 */
			MULERT_API mule::Storage::BinaryData LoadData(unsigned int id);

			/**
			 * @brief Open a C-style I/O stream.
			 * @param id File id.
			 * @param create Create file if it is not exist?
			 * @return A FILE* handle.
			 */
			MULERT_API FILE *OpenRaw(unsigned int id, bool create = false);

			/**
			 * @brief Save a data into specified file.
			 * @param data Binary data.
			 * @param id File id.
			 * @return File id.
			 */
			MULERT_API unsigned int SaveData(const mule::Storage::BinaryData &data, unsigned int id);

			/**
			 * @brief Save a data into a file, whose file will be automatic assigned.
			 * @param data Binary data.
			 * @return Assigned id.
			 */
			MULERT_API unsigned int SaveData(const mule::Storage::BinaryData &data);
		};
	}
}

#endif // !DATA_MANAGER_H__

