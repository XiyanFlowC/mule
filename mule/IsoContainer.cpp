#include "IsoContainer.h"

#include <xystring.h>

IsoContainer::IsoContainer(xybase::Stream *stream)
	: BasicContainer(stream)
{
    stream->Seek(0x8000, 0);
    PrimaryVolume volume{};
    stream->ReadBytes(reinterpret_cast<char *>(&volume), sizeof(PrimaryVolume));
    ParseDirectory(stream, volume.rootDirectoryRecord.locationOfExtentLe * ISO_BLOCK_SIZE, "");
}

void IsoContainer::ParseDirectory(xybase::Stream *isoFile, uint32_t offset, std::string path) {
    isoFile->Seek(offset, 0);

    while (true) {
        uint8_t size = isoFile->ReadUInt8();
        isoFile->Seek(offset, 0);
        if (size == 0) break;
        DirectoryEntry *entry = reinterpret_cast<DirectoryEntry *>(new char[size]);
        isoFile->ReadBytes(reinterpret_cast<char *>(entry), size);

        if (entry->fileFlags & 0x02) {
            // It's a directory
            std::string directoryName(entry->fileIdentifier, entry->lengthOfFileIdentifier);
            /*if (entry->fileIdentifier[0] == 0) directoryName = ".", skip = true;
            if (entry->fileIdentifier[0] == 1) directoryName = "..", skip = true;*/
            if (entry->fileIdentifier[0] == 0 || entry->fileIdentifier[0] == 1)
            {
                // Move to the next entry
                offset += entry->length;
                isoFile->Seek(offset, 0);
                delete[] entry;
                continue; /* . & ..，忽略 */
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
            fileIndices[xybase::string::to_utf16(path + '/' + fileName)] = FileDesc{ fileOffset, fileSize, fileSize, false };
        }

        // Move to the next entry
        offset += entry->length;
        isoFile->Seek(offset, 0);
        delete[] entry;
    }
}
