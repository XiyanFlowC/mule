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
            int newFileSize = files[xybase::string::to_utf16(path + '/' + fileName)]->size;
            int newLocation = files[xybase::string::to_utf16(path + '/' + fileName)]->offset / ISO_BLOCK_SIZE;

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
                
                entry->recordingDateTime[0] = static_cast<uint8_t>(localTime.tm_year - 80);// Year since 1980
                entry->recordingDateTime[1] = static_cast<uint8_t>(localTime.tm_mon + 1);  // Month (1-12)
                entry->recordingDateTime[2] = static_cast<uint8_t>(localTime.tm_mday);     // Day (1-31)
                entry->recordingDateTime[3] = static_cast<uint8_t>(localTime.tm_hour);     // Hour (0-23)
                entry->recordingDateTime[4] = static_cast<uint8_t>(localTime.tm_min);      // Minute (0-59)
                entry->recordingDateTime[5] = static_cast<uint8_t>(localTime.tm_sec);      // Second (0-59)
                entry->recordingDateTime[6] = static_cast<uint8_t>(gmtOffset);             // GMT offset 
            }
            
            // if the location information has been updated.
            if (entry->dataLengthLe != newFileSize || entry->locationOfExtentLe != newLocation)
            {
                entry->dataLengthLe = newFileSize;
                entry->dataLengthBe = (newFileSize >> 24) | ((newFileSize >> 8) & 0xFF00) | ((newFileSize & 0xFF) << 8) | (newFileSize << 24);
                entry->locationOfExtentLe = newLocation;
                entry->locationOfExtentBe = (newLocation >> 24) | ((newLocation >> 8) & 0xFF00) | ((newLocation & 0xFF) << 8) | (newLocation << 24);
                isoFile->Seek(offset);
                // write new size information.
                isoFile->Write((char *)entry, entry->length);
            }
        }

        // Move to the next entry
        offset += entry->length;
        isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
        delete[] entry;
    }
}
