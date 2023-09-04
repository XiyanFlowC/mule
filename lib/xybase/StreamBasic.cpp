#include "StreamBasic.h"

xybase::StreamBasic::~StreamBasic()
{
}

uint8_t xybase::StreamBasic::ReadUInt8()
{
	uint8_t ret{0};
	ReadBytes(reinterpret_cast<char *>(&ret), 1);
	return ret;
}

int8_t xybase::StreamBasic::ReadInt8()
{
	int8_t ret{0};
	ReadBytes(reinterpret_cast<char *>(&ret), 1);
	return ret;
}

uint16_t xybase::StreamBasic::ReadUInt16()
{
	uint16_t ret{0};
	ReadBytes(reinterpret_cast<char *>(&ret), 2);
	if (isBigEndian ^ bigEndianSystem)
	{
		return ((ret & 0xFF00) >> 8) | ((ret & 0xFF) << 8);
	}
	return ret;
}

int16_t xybase::StreamBasic::ReadInt16()
{
	union
	{
		uint16_t a;
		int16_t b;
	} ret = { 0 };
	ret.a = ReadUInt16();
	return ret.b;
}

uint32_t xybase::StreamBasic::ReadUInt32()
{
	uint32_t ret{ 0 };
	ReadBytes(reinterpret_cast<char *>(&ret), 4);
	if (isBigEndian ^ bigEndianSystem)
	{
		return (ret & 0xFF) << 24 | (ret & 0xFF00) << 8 | (ret & 0xFF0000) >> 8 | (ret & 0xFF000000) >> 24;
	}
	return ret;
}

int32_t xybase::StreamBasic::ReadInt32()
{
	union
	{
		uint32_t a;
		int32_t b;
	} ret = { 0 };
	ret.a = ReadUInt32();
	return ret.b;
}

uint64_t xybase::StreamBasic::ReadUInt64()
{
	uint64_t ret{ 0 };
	ReadBytes(reinterpret_cast<char *>(&ret), 8);
	if (isBigEndian ^ bigEndianSystem)
	{
		return (ret & 0xFF) << 56 | (ret & 0xFF00) << 40 | (ret & 0xFF0000) << 24 | (ret & 0xFF000000) << 8 |
			(ret & 0xFF00000000ull) >> 8 | (ret & 0xFF0000000000ull) >> 24 | (ret & 0xFF000000000000ull) >> 40 |
			(ret & 0xFF00000000000000ull) >> 56;
	}
	return ret;
}

int64_t xybase::StreamBasic::ReadInt64()
{
	union
	{
		uint64_t a;
		int64_t b;
	} ret = { 0 };
	ret.a = ReadUInt64();
	return ret.b;
}

float xybase::StreamBasic::ReadFloat()
{
	union
	{
		uint32_t a;
		float b;
	} ret = { 0 };
	ret.a = ReadUInt32();
	return ret.b;
}

double xybase::StreamBasic::ReadDouble()
{
	union
	{
		uint64_t a;
		double b;
	} ret = { 0 };
	ret.a = ReadUInt64();
	return ret.b;
}

std::string xybase::StreamBasic::ReadString()
{
	xybase::StringBuilder<char> sb;

	char ch = ReadInt8();
	while (ch != '\0')
	{
		sb.Append(ch);

		ch = ReadInt8();
	}

	return sb.ToString();
}

void xybase::StreamBasic::Write(uint8_t value)
{
	Write(reinterpret_cast<char *>(&value), sizeof(value));
}

void xybase::StreamBasic::Write(int8_t value)
{
	Write(reinterpret_cast<char *>(&value), sizeof(value));
}

void xybase::StreamBasic::Write(uint16_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = ((value & 0xFF00) >> 8) | ((value & 0xFF) << 8);
	}
	Write(reinterpret_cast<char *>(&value), sizeof(value));
}

void xybase::StreamBasic::Write(int16_t value)
{
	Write(*((uint16_t *)&value));
}

void xybase::StreamBasic::Write(uint32_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = (value & 0xFF) << 24 | (value & 0xFF00) << 8 | (value & 0xFF0000) >> 8 | (value & 0xFF000000) >> 24;
	}
	Write(reinterpret_cast<char *>(&value), sizeof(value));
}

void xybase::StreamBasic::Write(int32_t value)
{
	Write(*((uint32_t *)&value));
}

void xybase::StreamBasic::Write(uint64_t value)
{
	if (isBigEndian ^ bigEndianSystem)
	{
		value = ((value & 0xFFFFFFFF00000000llu) >> 32) | ((value & 0xFFFFFFFFllu) << 32);
		value = ((value & 0xFFFF0000FFFF0000llu) >> 16) | ((value & 0xFFFF0000FFFFllu) << 16);
		value = ((value & 0xFF00FF00FF00FF00llu) >> 8) | ((value & 0xFF00FF00FF00FFllu) << 8);
	}
	Write(reinterpret_cast<char *>(&value), sizeof(value));
}

void xybase::StreamBasic::Write(int64_t value)
{
	Write(*((uint64_t *)&value));
}

void xybase::StreamBasic::Write(float value)
{
	Write(*((uint32_t *)&value));
}

void xybase::StreamBasic::Write(double value)
{
	Write(*((uint64_t *)&value));
}

void xybase::StreamBasic::Write(const std::string &value)
{
	Write(value.c_str(), value.size() + 1);
}
