#pragma once

#ifndef SHIFT_BINARY_STREAM_H__
#define SHIFT_BINARY_STREAM_H__

#include "BinaryStream.h"


namespace mule
{
	namespace Data
	{

		class ShiftBinaryStream : public BinaryStream
		{
		protected:
			long long offset;
		public:
			ShiftBinaryStream(const char *path, long long offset, bool isBigEndian = false);

			void SetOffset(long long offset);

			virtual size_t Tell() override;

			virtual void Seek(long long offset, int mode) override;
		};
	}
}

#endif
