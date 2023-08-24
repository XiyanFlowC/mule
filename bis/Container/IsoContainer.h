#pragma once

#ifndef ISO_CONTAINER_H__
#define ISO_CONTAINER_H__

#include "BasicContainer.h"

namespace mule
{
    namespace Container
    {
#define ISO_BLOCK_SIZE (2048)

#pragma pack(push, 1)

        // Structure to represent Directory Entry
        struct DirectoryEntry {
            uint8_t length;
            uint8_t extendedAttributeRecordLength;
            uint32_t locationOfExtentLe;
            uint32_t locationOfExtentBe;
            uint32_t dataLengthLe;
            uint32_t dataLengthBe;
            char recordingDateTime[7];
            uint8_t fileFlags;
            uint8_t fileUnitSize;
            uint8_t interleaveGapSize;
            uint16_t volumeSequenceNumberLe;
            uint16_t volumeSequenceNumberBe;
            uint8_t lengthOfFileIdentifier;
            char fileIdentifier[1]; // Variable size, but we'll dynamically allocate it.
            // Padding field is not included in the structure.
        };

        // Structure to represent Primary Volume Descriptor
        struct PrimaryVolume {
            uint8_t typeCode;
            char standardIdentifier[5];
            uint8_t version;
            uint8_t unused1;
            char systemIdentifier[32];
            char volumeIdentifier[32];
            uint8_t unused2[8];
            uint32_t volumeSpaceSizeLe;
            uint32_t volumeSpaceSizeBe;
            uint8_t unused3[32];
            uint16_t volumeSetSizeLe;
            uint16_t volumeSetSizeBe;
            uint16_t volumeSequenceNumberLe;
            uint16_t volumeSequenceNumberBe;
            uint16_t logicalBlockSizeLe;
            uint16_t logicalBlockSizeBe;
            uint32_t pathTableSizeLe;
            uint32_t pathTableSizeBe;
            uint32_t locationOfTypeLPathTable;
            uint32_t locationOfOptionalTypeLPathTable;
            uint32_t locationOfTypeMPathTable;
            uint32_t locationOfOptionalTypeMPathTable;
            DirectoryEntry rootDirectoryRecord;
            char volumeSetIdentifier[128];
            char publisherIdentifier[128];
            char dataPreparerIdentifier[128];
            char applicationIdentifier[128];
            char copyrightFileIdentifier[37];
            char abstractFileIdentifier[37];
            char bibliographicFileIdentifier[37];
            char volumeCreationDateAndTime[17];
            char volumeModificationDateAndTime[17];
            char volumeExpirationDateAndTime[17];
            char volumeEffectiveDateAndTime[17];
            uint8_t fileStructureVersion;
            uint8_t unused4;
            uint8_t applicationUsed[512];
            uint8_t reserved[653];
        };

#pragma pack(pop)

        class IsoContainer : public mule::BasicContainer
        {
        private:
            void ParseDirectory(xybase::Stream *isoFile, uint32_t offset, std::string path);

        public:
            IsoContainer(xybase::Stream *stream);
        };
    }
}

#endif
