#include "IsoContainer.h"

#include <xystring.h>
#include <chrono>
#include <ctime>

using namespace mule::Container;
using namespace xybase;

IsoContainer::IsoContainer(xybase::Stream *stream)
    : FileContainerBasic(stream)
{
    stream->Seek(0x8000, xybase::Stream::SM_BEGIN);
    PrimaryVolume volume{};
    stream->ReadBytes(reinterpret_cast<char *>(&volume), sizeof(PrimaryVolume));
    ParseDirectory(stream, (xybase::bigEndianSystem ? volume.rootDirectoryRecord.locationOfExtentBe : volume.rootDirectoryRecord.locationOfExtentLe) * ISO_BLOCK_SIZE, "");
}

mule::Container::IsoContainer::~IsoContainer()
{
    infraStream->Seek(0x8000, xybase::Stream::SM_BEGIN);
    PrimaryVolume volume{};
    infraStream->ReadBytes(reinterpret_cast<char *>(&volume), sizeof(PrimaryVolume));
    OverwriteDirector(infraStream, (xybase::bigEndianSystem ? volume.rootDirectoryRecord.locationOfExtentBe : volume.rootDirectoryRecord.locationOfExtentLe) * ISO_BLOCK_SIZE, "");
}

xybase::Stream *mule::Container::IsoContainer::Open(std::u16string name, xybase::FileOpenMode mode)
{
    if (mode & xybase::FOM_WRITE)
    {
		m_modifiedFiles.insert(name);
    }

	return xybase::FileContainerBasic::Open(name, mode);
}

void IsoContainer::ParseDirectory(xybase::Stream *isoFile, uint32_t offset, std::string path) {
    isoFile->Seek(offset, xybase::Stream::SM_BEGIN);

    while (true) {
        uint8_t size = isoFile->ReadUInt8();
        isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
        if (size == 0) break;
        DirectoryEntry *entry = reinterpret_cast<DirectoryEntry *>(new char[size]);
        isoFile->ReadBytes(reinterpret_cast<char *>(entry), size);

        if (entry->fileFlags & 0x02) {
            // It's a directory
            std::string directoryName(entry->fileIdentifier, entry->lengthOfFileIdentifier);

            if (entry->fileIdentifier[0] == 0 || entry->fileIdentifier[0] == 1)
            {
                // Move to the next entry
                offset += entry->length;
                isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
                delete[] entry;
                continue; /* . & ..，omit */
            }
            // std::cout << "Directory: " << directoryName << std::endl;

            // Recursively parse subdirectories
            ParseDirectory(isoFile, (xybase::bigEndianSystem ? entry->locationOfExtentBe : entry->locationOfExtentLe) * ISO_BLOCK_SIZE, path + "/" + directoryName);
        }
        else {
            // It's a file
            std::string fileName(entry->fileIdentifier, entry->lengthOfFileIdentifier);
            uint32_t fileSize = (xybase::bigEndianSystem ? entry->dataLengthBe : entry->dataLengthLe);
            uint32_t fileOffset = (xybase::bigEndianSystem ? entry->locationOfExtentBe : entry->locationOfExtentLe) * ISO_BLOCK_SIZE;
            files[xybase::string::to_utf16(path + '/' + fileName)] = new FileEntry{ fileOffset, fileSize, xybase::string::to_utf16(path + '/' + fileName), false };
        }

        // Move to the next entry
        offset += entry->length;
        isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
        delete[] entry;
    }
}

void mule::Container::IsoContainer::OverwriteDirector(xybase::Stream *isoFile, uint32_t offset, std::string path)
{
    isoFile->Seek(offset, xybase::Stream::SM_BEGIN);

	bool modified = false;

    while (true) {
        uint8_t size = isoFile->ReadUInt8();
        isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
        if (size == 0) break;
        DirectoryEntry *entry = reinterpret_cast<DirectoryEntry *>(new char[size]);
        isoFile->ReadBytes(reinterpret_cast<char *>(entry), size);

        if (entry->fileFlags & 0x02) {
            // It's a directory
            std::string directoryName(entry->fileIdentifier, entry->lengthOfFileIdentifier);

            if (entry->fileIdentifier[0] == 0 || entry->fileIdentifier[0] == 1)
            {
                // Move to the next entry
                offset += entry->length;
                isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
                delete[] entry;
                continue; /* . & ..，omit */
            }
            // std::cout << "Directory: " << directoryName << std::endl;

            // Recursively parse subdirectories
            OverwriteDirector(isoFile, (xybase::bigEndianSystem ? entry->locationOfExtentBe : entry->locationOfExtentLe) * ISO_BLOCK_SIZE, path + "/" + directoryName);
        }
        else {
            // It's a file
            std::string fileName(entry->fileIdentifier, entry->lengthOfFileIdentifier);
            uint32_t newFileSize = (uint32_t)files[xybase::string::to_utf16(path + '/' + fileName)]->size;
            uint32_t newLocation = (uint32_t)files[xybase::string::to_utf16(path + '/' + fileName)]->offset / ISO_BLOCK_SIZE;

            if (xybase::bigEndianSystem)
            {
                newFileSize = (newFileSize >> 24) | ((newFileSize >> 8) & 0xFF00) | ((newFileSize & 0xFF) << 8) | (newFileSize << 24);
                newLocation = (newLocation >> 24) | ((newLocation >> 8) & 0xFF00) | ((newLocation & 0xFF) << 8) | (newLocation << 24);
            }

            // if file modifed
            if (m_modifiedFiles.contains(xybase::string::to_utf16(path + '/' + fileName)))
            {
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                std::tm localTime = *std::localtime(&now_c);
                std::tm utcTime = *std::gmtime(&now_c);
                
                // Calculate GMT offset in minutes
                int gmtOffsetMinutes = (localTime.tm_hour - utcTime.tm_hour) * 60 + (localTime.tm_min - utcTime.tm_min);

                // Handle day boundary cases
                if (localTime.tm_yday != utcTime.tm_yday)
                {
                    gmtOffsetMinutes += (localTime.tm_yday > utcTime.tm_yday) ? 24 * 60 : -24 * 60;
                }

                // Convert GMT offset to 15-minute units
                int gmtOffset = gmtOffsetMinutes / 15;
                
                entry->recordingDateTime[0] = static_cast<uint8_t>(localTime.tm_year);// Year since 1900
                entry->recordingDateTime[1] = static_cast<uint8_t>(localTime.tm_mon + 1);  // Month (1-12)
                entry->recordingDateTime[2] = static_cast<uint8_t>(localTime.tm_mday);     // Day (1-31)
                entry->recordingDateTime[3] = static_cast<uint8_t>(localTime.tm_hour);     // Hour (0-23)
                entry->recordingDateTime[4] = static_cast<uint8_t>(localTime.tm_min);      // Minute (0-59)
                entry->recordingDateTime[5] = static_cast<uint8_t>(localTime.tm_sec);      // Second (0-59)
                entry->recordingDateTime[6] = static_cast<uint8_t>(gmtOffset);             // GMT offset 
            //}
            //
            //// if the location information has been updated.
            //if (entry->dataLengthLe != newFileSize || entry->locationOfExtentLe != newLocation)
            //{
                entry->dataLengthLe = newFileSize;
                entry->dataLengthBe = (newFileSize >> 24) | ((newFileSize >> 8) & 0xFF00) | ((newFileSize & 0xFF) << 8) | (newFileSize << 24);
                entry->locationOfExtentLe = newLocation;
                entry->locationOfExtentBe = (newLocation >> 24) | ((newLocation >> 8) & 0xFF00) | ((newLocation & 0xFF) << 8) | (newLocation << 24);
                isoFile->Seek(offset);
                // write new size information.
                isoFile->Write((char *)entry, entry->length);

				modified = true;
            }
        }

        // Move to the next entry
        offset += entry->length;
        isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
        delete[] entry;
    }

    if (modified)
    {
		// Update volume modification time
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm localTime = *std::localtime(&now_c);
		isoFile->Seek(0x8000, xybase::Stream::SM_BEGIN);
		PrimaryVolume volume{};
		isoFile->ReadBytes(reinterpret_cast<char *>(&volume), sizeof(PrimaryVolume));
        snprintf(volume.volumeModificationDateAndTime, sizeof(volume.volumeModificationDateAndTime),
            "%04d%02d%02d%02d%02d%02d00",
            localTime.tm_year + 1900,
            localTime.tm_mon + 1,
            localTime.tm_mday,
            localTime.tm_hour,
            localTime.tm_min,
			localTime.tm_sec);
		std::tm utcTime = *std::gmtime(&now_c);

        // Calculate GMT offset in minutes
        int gmtOffsetMinutes = (localTime.tm_hour - utcTime.tm_hour) * 60 + (localTime.tm_min - utcTime.tm_min);

        // Handle day boundary cases
        if (localTime.tm_yday != utcTime.tm_yday)
        {
            gmtOffsetMinutes += (localTime.tm_yday > utcTime.tm_yday) ? 24 * 60 : -24 * 60;
        }

        // Convert GMT offset to 15-minute units
        int gmtOffset = gmtOffsetMinutes / 15;

        // Time zone offset from GMT in 15 minute intervals, starting at interval -48 (west) and running up to interval 52 (east). So value 0 indicates interval -48 which equals GMT-12 hours, and value 100 indicates interval 52 which equals GMT+13 hours. 

		volume.volumeModificationDateAndTime[16] = static_cast<char>(gmtOffset);

		isoFile->Seek(0x8000, xybase::Stream::SM_BEGIN);
		isoFile->Write(reinterpret_cast<char *>(&volume), sizeof(PrimaryVolume));
    }
}

#include <Storage/DataManager.h>

using FilePtr = std::unique_ptr<FILE, decltype(&fclose)>;
inline void hash_combine(size_t &seed, size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

void mule::Container::IsoContainer::SaveFreeSpace(uint32_t dataId)
{
    // 1. 检查是否有内部文件活动
    if (!openedFiles.empty())
        throw InvalidOperationException((L"Cannot save free space while inner file(s) active. One of the opened file(s): " + xybase::string::to_wstring(openedFiles.begin()->second.baseEntry->path)).c_str(), 754100);

    // 2. 打开文件，检查是否成功
    FILE *raw = mule::Storage::DataManager::GetInstance().OpenRaw(dataId, true);
    if (!raw)
        throw RuntimeException(L"Failed to open raw data file for writing free space.",754101);
    FilePtr out(raw, &fclose);  // RAII 自动关闭

    // 3. 计算校验和（基于当前文件映射）
    size_t checksum1 = 0, checksum2 = 0;
    for (const auto &item : files) {
        // 1. 处理字符串 Key（获取其哈希值）
        size_t key_hash = std::hash<std::u16string>{}(item.first);

        // 2. 混合 Key 的哈希（彻底杜绝“相同布局不同名”的碰撞）
        hash_combine(checksum1, key_hash);
        hash_combine(checksum2, key_hash);

        // 3. 混合 offset 和 size（此处顺序敏感，消除了 XOR/加法的交换律问题）
        hash_combine(checksum1, item.second->offset);
        hash_combine(checksum2, item.second->size);

        // 4. 额外将 offset 和 size 交叉混合进另一个校验和，增加区分度
        hash_combine(checksum1, item.second->size);
        hash_combine(checksum2, item.second->offset);
    }

    // 4. 写入校验和
    if (fwrite(&checksum1, sizeof(checksum1), 1, out.get()) != 1 ||
        fwrite(&checksum2, sizeof(checksum2), 1, out.get()) != 1) {
        throw RuntimeException(L"Failed to write checksums to free space file.",754102);
    }

    // 5. 获取空闲块列表
    auto fragments = freeSpaces.GetFragments();
    size_t count = fragments.size();

    // 6. 写入块数量
    if (fwrite(&count, sizeof(count), 1, out.get()) != 1) {
        throw RuntimeException(L"Failed to write fragment count.",754103);
    }

    // 7. 写入每个空闲块的偏移和长度
    for (const auto &space : fragments) {
        size_t offset = space->GetBeginning();
        size_t length = space->GetSize();
        if (fwrite(&offset, sizeof(offset), 1, out.get()) != 1 ||
            fwrite(&length, sizeof(length), 1, out.get()) != 1) {
            throw RuntimeException(L"Failed to write free space fragment.",754104);
        }
    }

    // out 析构时自动 fclose，无需手动调用
}

void mule::Container::IsoContainer::LoadFreeSpace(uint32_t dataId)
{
    if (!m_modifiedFiles.empty())
        throw InvalidOperationException(L"Cannot load free space when file is modified.", 754110);

    // 1. 打开文件，检查是否成功
    FILE *raw = mule::Storage::DataManager::GetInstance().OpenRaw(dataId, false);
    if (!raw)
        throw RuntimeException(L"Failed to open raw data file for reading free space.",754105);
    FilePtr in(raw, &fclose);  // RAII 自动关闭

    // 2. 读取保存的校验和
    size_t saved_cs1 = 0, saved_cs2 = 0;
    if (fread(&saved_cs1, sizeof(saved_cs1), 1, in.get()) != 1 ||
        fread(&saved_cs2, sizeof(saved_cs2), 1, in.get()) != 1) {
        throw RuntimeException(L"Failed to read checksums from free space file.",754106);
    }

    // 3. 重新计算当前文件映射的校验和（用于验证）
    size_t computed_cs1 = 0, computed_cs2 = 0;
    for (const auto &item : files) {
        // 1. 处理字符串 Key（获取其哈希值）
        size_t key_hash = std::hash<std::u16string>{}(item.first);

        // 2. 混合 Key 的哈希（彻底杜绝“相同布局不同名”的碰撞）
        hash_combine(computed_cs1, key_hash);
        hash_combine(computed_cs2, key_hash);

        // 3. 混合 offset 和 size（此处顺序敏感，消除了 XOR/加法的交换律问题）
        hash_combine(computed_cs1, item.second->offset);
        hash_combine(computed_cs2, item.second->size);

        // 4. 额外将 offset 和 size 交叉混合进另一个校验和，增加区分度
        hash_combine(computed_cs1, item.second->size);
        hash_combine(computed_cs2, item.second->offset);
    }

    // 4. 验证校验和
    if (saved_cs1 != computed_cs1 || saved_cs2 != computed_cs2) {
        throw RuntimeException(L"Checksum mismatch in free space file – data may be corrupted.",754107);
    }

    // 5. 读取空闲块数量
    size_t count = 0;
    if (fread(&count, sizeof(count), 1, in.get()) != 1) {
        throw RuntimeException(L"Failed to read fragment count.",754108);
    }

    // 7. 循环读取每个空闲块并添加到 freeSpaces
    for (size_t i = 0; i < count; ++i) {
        size_t offset = 0, length = 0;
        if (fread(&offset, sizeof(offset), 1, in.get()) != 1 ||
            fread(&length, sizeof(length), 1, in.get()) != 1) {
            throw RuntimeException(L"Failed to read free space fragment data.", 754109);
        }
        // 假设 freeSpaces 有 AddFragment 方法
        freeSpaces.RegisterFragment({ offset, length });
    }
    // 文件由 RAII 自动关闭
}
