#include "IsoContainer.h"

#include <xystring.h>

using namespace mule::Container;
using namespace xybase;

IsoContainer::IsoContainer(xybase::Stream *stream)
    : FileContainerBasic(stream)
{
    stream->Seek(0x8000, xybase::Stream::SM_BEGIN);
    PrimaryVolume volume{};
    stream->ReadBytes(reinterpret_cast<char *>(&volume), sizeof(PrimaryVolume));
    ParseDirectory(stream, volume.rootDirectoryRecord.locationOfExtentLe * ISO_BLOCK_SIZE, "");
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
            ParseDirectory(isoFile, entry->locationOfExtentLe * ISO_BLOCK_SIZE, path + "/" + directoryName);
        }
        else {
            // It's a file
            std::string fileName(entry->fileIdentifier, entry->lengthOfFileIdentifier);
            uint32_t fileSize = entry->dataLengthLe;
            uint32_t fileOffset = entry->locationOfExtentLe * ISO_BLOCK_SIZE;
            files[xybase::string::to_utf16(path + '/' + fileName)] = new FileEntry{fileOffset, fileSize, xybase::string::to_utf16(path + '/' + fileName), false};
        }

        // Move to the next entry
        offset += entry->length;
        isoFile->Seek(offset, xybase::Stream::SM_BEGIN);
        delete[] entry;
    }
}
