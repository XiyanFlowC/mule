#include "MultiValue.h"
#include "../../xybase/StringBuilder.h"

using namespace std;

using namespace mule::Data::Basic;
using namespace mule::Exception;

const MultiValue MultiValue::MV_NULL = MultiValue(MultiValue::MVT_NULL);

InvalidRValueException::InvalidRValueException(std::string description, const char* file, int line) : Exception(description, file, line)
{
}

bool MultiValue::operator==(const MultiValue& rvalue) const
{
	if (type != rvalue.type)
		return false;

	switch (type)
	{
	case MVT_INT:
		return value.signedValue == rvalue.value.signedValue;
		break;
	case MVT_UINT:
		return value.unsignedValue == rvalue.value.unsignedValue;
		break;
	case MVT_STRING:
		return *value.stringValue == *rvalue.value.stringValue;
		break;
	case MVT_ARRAY:
		if (length != rvalue.length) return false;

		for (int i = 0; i < length; ++i) {
			if (value.arrayValue[i] != rvalue.value.arrayValue[i]) return false;
		}
		return true;
		break;
	case MVT_NULL:
		return true;
		break;
	default:
		return false;
	}
}

bool MultiValue::operator!=(const MultiValue& rvalue) const
{
	return !(*this == rvalue);
}

bool MultiValue::operator< (const MultiValue &rvalue) const
{
	if (type != rvalue.type)
		return false;

	switch (type)
	{
	case MVT_INT:
		return value.signedValue < rvalue.value.signedValue;
	case MVT_UINT:
		return value.unsignedValue < rvalue.value.unsignedValue;
	case MVT_REAL:
		return value.realValue < rvalue.value.realValue;
	case MVT_STRING:
		return *value.stringValue < *rvalue.value.stringValue;
	default:
		return false;
	}
}

bool MultiValue::operator<= (const MultiValue &rvalue) const
{
	return *this == rvalue || *this < rvalue;
}

bool MultiValue::operator> (const MultiValue &rvalue) const
{
	return *this != rvalue && !(*this < rvalue);
}

bool MultiValue::operator>= (const MultiValue &rvalue) const
{
	return !(*this < rvalue);
}

MultiValue::~MultiValue()
{
	DisposeOldValue();
}

MultiValue::MultiValue()
{
	value.unsignedValue = 0ULL;
	length = 0;
	type = MVT_NULL;
}

mule::Data::Basic::MultiValue::MultiValue(ValueType type, int length)
	: length(length)
{
	this->type = type;
	switch (type)
	{
	case mule::Data::Basic::MultiValue::MVT_NULL:
	case mule::Data::Basic::MultiValue::MVT_INT:
	case mule::Data::Basic::MultiValue::MVT_UINT:
	case mule::Data::Basic::MultiValue::MVT_REAL:
		value.unsignedValue = 0;
		break;
	case mule::Data::Basic::MultiValue::MVT_STRING:
		value.stringValue = new std::string("");
		break;
	case mule::Data::Basic::MultiValue::MVT_MAP:
		value.mapValue = new std::map<MultiValue, MultiValue>;
		useCounter = new int;
		*useCounter = 1;
		break;
	case mule::Data::Basic::MultiValue::MVT_ARRAY:
		value.arrayValue = new MultiValue[length];
		useCounter = new int;
		*useCounter = 1;
		break;
	default:
		value.unsignedValue = 0;
		break;
	}
}

MultiValue::MultiValue(const MultiValue& pattern)
{
	type = pattern.type;
	value = pattern.value;
	metadata = pattern.metadata;

	// 维护引用计数
	useCounter = pattern.useCounter;
	if (useCounter != nullptr)
	{
		*useCounter += 1;
	}

	length = 0;
	
	if (type == MVT_STRING)
	{
		value.stringValue = new std::string(*(pattern.value.stringValue));
	}
}

mule::Data::Basic::MultiValue::MultiValue(MultiValue &&movee) noexcept
{
	type = movee.type;
	value = movee.value;
	metadata = movee.metadata;
	useCounter = movee.useCounter;

	if (type == MVT_STRING && value.stringValue != nullptr)
	{
		movee.value.stringValue = nullptr;
	}
	if (type == MVT_MAP && value.mapValue != nullptr)
	{
		movee.useCounter = nullptr;
		movee.value.mapValue = nullptr;
	}
	if (type == MVT_ARRAY && value.arrayValue != nullptr)
	{
		movee.useCounter = nullptr;
		movee.value.arrayValue = nullptr;
	}

	// 阻止任何形式的数据回收
	movee.type = MVT_NULL;

	length = movee.length;
}

MultiValue::MultiValue(const std::string& value)
{
	type = MVT_NULL;
	SetValue(value);
}

MultiValue::MultiValue(const double value)
{
	type = MVT_NULL;
	SetValue(value);
}

MultiValue::MultiValue(const uint64_t value)
{
	type = MVT_NULL;
	SetValue(value);
}

mule::Data::Basic::MultiValue::MultiValue(const uint32_t value)
{
	type = MVT_NULL;
	SetValue((uint64_t)value);
}

mule::Data::Basic::MultiValue::MultiValue(const uint16_t value)
{
	type = MVT_NULL;
	SetValue((uint64_t)value);
}

mule::Data::Basic::MultiValue::MultiValue(const uint8_t value)
{
	type = MVT_NULL;
	SetValue((uint64_t)value);
}

MultiValue::MultiValue(const int64_t value)
{
	type = MVT_NULL;
	SetValue(value);
}

mule::Data::Basic::MultiValue::MultiValue(const int32_t value)
{
	type = MVT_NULL;
	SetValue((int64_t)value);
}

mule::Data::Basic::MultiValue::MultiValue(const int16_t value)
{
	type = MVT_NULL;
	SetValue((int64_t)value);
}

mule::Data::Basic::MultiValue::MultiValue(const int8_t value)
{
	type = MVT_NULL;
	SetValue((int64_t)value);
}

mule::Data::Basic::MultiValue::MultiValue(const int size, const MultiValue *array)
{
	type = MVT_NULL;
	SetValue(size, array);
}

MultiValue::MultiValue(const std::map<MultiValue, MultiValue> map)
{
	type = MVT_NULL;
	SetValue(map);
}

void mule::Data::Basic::MultiValue::SetType(ValueType type, int length)
{
	if (this->type == type) return;

	DisposeOldValue();

	this->type = type;
	switch (type)
	{
	case mule::Data::Basic::MultiValue::MVT_NULL:
	case mule::Data::Basic::MultiValue::MVT_INT:
	case mule::Data::Basic::MultiValue::MVT_UINT:
	case mule::Data::Basic::MultiValue::MVT_REAL:
		value.unsignedValue = 0;
		break;
	case mule::Data::Basic::MultiValue::MVT_STRING:
		value.stringValue = new std::string("");
		break;
	case mule::Data::Basic::MultiValue::MVT_MAP:
		value.mapValue = new std::map<MultiValue, MultiValue>;
		useCounter = new int;
		*useCounter = 1;
		break;
	case mule::Data::Basic::MultiValue::MVT_ARRAY:
		value.arrayValue = new MultiValue[length];
		useCounter = new int;
		*useCounter = 1;
		break;
	default:
		value.unsignedValue = 0;
		break;
	}
}

mule::Data::Basic::MultiValue::ValueType mule::Data::Basic::MultiValue::GetType() const
{
	return type;
}

size_t mule::Data::Basic::MultiValue::GetLength() const
{
	return length;
}

std::string MultiValue::ToString() const
{
	switch (type)
	{
	case MVT_INT:
		return std::to_string(value.signedValue);
		break;
	case MVT_NULL:
		return std::string("(null)");
		break;
	case MVT_REAL:
		return std::to_string(value.realValue);
		break;
	case MVT_STRING:
		return *value.stringValue;
		break;
	case MVT_UINT:
		return std::to_string(value.unsignedValue);
		break;
	case MVT_ARRAY:
	{
		std::string aret("[");
		for (int i = 0; i < length; ++i)
		{
			aret += value.arrayValue[i].ToString() + ',';
		}
		aret += "]";
		return aret;
		break;
	}
	case MVT_MAP:
	{
		std::string mret("{");
		for (const std::pair<MultiValue, MultiValue> &pair : *value.mapValue)
		{
			mret += pair.first.ToString() + '=' + pair.second.ToString();
			mret += ',';
		}
		mret += "}";
		return mret;
		break;
	}
	default:
		abort();
	}
	return "";
}

std::string MultiValue::Stringfy() const
{
	switch (type)
	{
	case MultiValue::MVT_NULL:
		return "null";
		break;
	case MultiValue::MVT_INT:
		return std::to_string(value.signedValue);
		break;
	case MultiValue::MVT_UINT:
		return std::to_string(value.unsignedValue) + "u";
		break;
	case MultiValue::MVT_REAL:
		return std::string("$") + std::to_string(value.realValue);
		break;
	case MultiValue::MVT_STRING:
		return Stringfy(*value.stringValue);
		break;
	}
	return "";
}

std::string MultiValue::Stringfy(std::string str) const
{
	xybase::StringBuilder<char> sb;
	for (char ch : str)
	{
		switch (ch)
		{
		case '\\':
			sb += "\\\\";
			break;
		case '\n':
			sb += "\\n";
			break;
		case '\r':
			sb += "\\r";
			break;
		case '"':
			sb += "\\\"";
			break;
		default:
			sb += ch;
		}
	}
	return sb.ToString();
}

void MultiValue::ParseInt(const std::string &value)
{
	unsigned long long res = 0;
	int neg = 0;
	int state = 0;
	type = MVT_INT;
	for (auto itr = value.begin(); itr != value.end(); ++itr)
	{
		switch (state)
		{
		case 0:
			if (*itr == '-') // 连续等号视作输入错误，并合并为一个
			{
				neg = 1;
				// state = 1;
			}
			else if ('1' <= *itr && *itr <= '9')
			{
				res = ((unsigned long long) * itr) - '0';
				state = 1;
			}
			else if ('0' == *itr)
			{
				state = 2; // 确认是否为十六进制数
			}
			else throw InvalidParameterException("value", "unexpected character.", MULE_FILE, __LINE__);
			break;
		case 1:
			if ('0' <= *itr && *itr <= '9')
			{
				res = ((unsigned long long) * itr) - '0' + res * 10;
			}
			else if (*itr == 'u' || *itr == 'U')
			{
				type = MVT_UINT;
				goto mvpi_forout; // 脱离循环
			}
			else throw InvalidParameterException("value", "unexpected character.", MULE_FILE, __LINE__);
			break;
		case 2:
			if ('1' <= *itr && *itr <= '7')
			{
				res = ((unsigned long long) * itr) - '0';
				state = 4; // 八进制输入
			}
			else if ('x' == *itr)
			{
				state = 3; // 十六进制输入
			}
			else if ('b' == *itr)
			{
				type = MVT_UINT; // 二进制输入默认无符号
				state = 5; // 二进制输入
			}
			else throw InvalidParameterException("value", "unexpected character.", MULE_FILE, __LINE__);
			break;
		case 3:
			if ('0' <= *itr && *itr <= '9')
			{
				res = (((unsigned long long) * itr) - '0') | (res << 4);
			}
			else if ('a' >= *itr && *itr <= 'f')
			{
				res = (((unsigned long long) * itr) - 'a') | (res << 4);
			}
			else if ('A' >= *itr && *itr <= 'F')
			{
				res = (((unsigned long long) * itr) - 'A') | (res << 4);
			}
			else if (*itr == 'u' || *itr == 'U')
			{
				type = MVT_UINT;
				goto mvpi_forout; // 脱离循环
			}
			else throw InvalidParameterException("value", "unexpected character.", MULE_FILE, __LINE__);
			break;
		case 4:
			if ('0' <= *itr && *itr <= '7')
			{
				res = (*itr - '0') | (res << 3);
			}
			else if (*itr == 'u' || *itr == 'U')
			{
				type = MVT_UINT;
				goto mvpi_forout; // 脱离循环
			}
			else throw InvalidParameterException("value", "unexpected character.", MULE_FILE, __LINE__);
			break;
		case 5:
			if (*itr == '0' || *itr == '1')
			{
				res = (res << 1) | (*itr - '0');
			}
			else throw InvalidParameterException("value", "unexpected character.", MULE_FILE, __LINE__);
		}
	}
	mvpi_forout:
	this->value.unsignedValue = neg ? ~res + 1 : res; // 如果有负号，取反
}

void MultiValue::ParseString(const std::string &value, bool isBareString)
{
	type = MVT_STRING;
	enum
	{
		MVPS_OUT,
		MVPS_NORMAL,
		MVPS_ESCAPE_WAITING,
		// TODO: 未来添加8进制支持
		// MVPS_ESCAPE_OCT,
		MVPS_ESCAPE_HEX,
		MVPS_ESCAPE_HEX2,
	} state = isBareString ? MVPS_NORMAL : MVPS_OUT;
	xybase::StringBuilder sb;

	char tmp;
	for (auto ch : value)
	{
		switch (state)
		{
		case MVPS_OUT:
			if (ch == '\"')
			{
				state = MVPS_NORMAL;
			}
			break;
		case MVPS_NORMAL:
			if (ch == '\"')
			{
				state = MVPS_OUT;
			}
			else if (ch == '\\')
			{
				state = MVPS_ESCAPE_WAITING;
			}
			else
			{
				sb.Append(ch);
			}
			break;
		case MVPS_ESCAPE_WAITING:
			if (ch == '0')
			{
				sb.Append('\0');
			}
			else if (ch == 'n')
			{
				sb.Append('\n');
			}
			else if (ch == 'r')
			{
				sb.Append('\r');
			}
			else if (ch == 'x')
			{
				state = MVPS_ESCAPE_HEX;
				break;
			}
			else
			{
				sb.Append(ch);
			}
			state = MVPS_NORMAL;
			break;
		case MVPS_ESCAPE_HEX:
			if (ch >= '0' && ch <= '9')
			{
				tmp = (ch - '0') << 4;
			}
			else if (ch >= 'A' && ch <= 'F')
			{
				tmp = (ch - 'A' + 10) << 4;
			}
			else if (ch >= 'a' && ch <= 'f')
			{
				tmp = (ch - 'a' + 10) << 4;
			}
			state = MVPS_ESCAPE_HEX2;
			break;
		case MVPS_ESCAPE_HEX2:
			if (ch >= '0' && ch <= '9')
			{
				tmp |= (ch - '0');
			}
			else if (ch >= 'A' && ch <= 'F')
			{
				tmp |= (ch - 'A' + 10);
			}
			else if (ch >= 'a' && ch <= 'f')
			{
				tmp |= (ch - 'a' + 10);
			}
			sb.Append(ch);
			state = MVPS_NORMAL;
			break;
		}
	}
	// 没有终结的引号
	if (state != (isBareString ? MVPS_NORMAL : MVPS_OUT)) throw InvalidParameterException("value", "Not a valid string representation.", MULE_FILE, __LINE__);

	this->value.stringValue = new std::string(sb.ToString());
}

void MultiValue::ParseReal(const std::string &value)
{
	type = MVT_REAL;

	double res = 0.0;
	int isNeg = 0, flag = 0;
	double fact = 0.1;
	for (char ch : value) {
		if (!isdigit(ch))
		{
			if (ch == '-') isNeg = 1;
			else if (ch == '.') break;
			else throw InvalidParameterException("value", "not a valid real number.", MULE_FILE, __LINE__);
		}

		if (flag)
		{
			res = res + fact * (ch - '0');
			fact *= 0.1;
		}
		else
			res = res * 10 + ch - '0';
	}


	this->value.realValue = isNeg ? -res : res;
}

void MultiValue::DisposeOldValue()
{
	if (type == MVT_STRING && value.stringValue != nullptr) delete this->value.stringValue;
	if (type == MVT_MAP && value.mapValue != nullptr) 
	{
		*useCounter -= 1;
		if (*useCounter == 0)
		{
			delete useCounter;
			delete this->value.mapValue;
			useCounter = nullptr;
		}
	}
	if (type == MVT_ARRAY && value.arrayValue != nullptr)
	{
		*useCounter -= 1;
		if (*useCounter == 0)
		{
			delete useCounter;
			delete[] this->value.arrayValue;
			useCounter = nullptr;
		}
	}
}

MultiValue MultiValue::Parse(const std::string &value)
{
	MultiValue ret;

	if (value == "null")
	{
		ret.type = MVT_NULL;
		ret.value.unsignedValue = 0;
	}

	if (value[0] == '\"') // 以“"”开头视作字符串
	{
		ret.ParseString(value);
	}
	else if (value[0] == '$')
	{
		ret.ParseReal(value);
	}
	else if (value[0] >= '0' && value[0] <= '9')
	{
		ret.ParseInt(value);
	}
	else
	{
		ret.ParseString(value, true);
	}

	return ret;
}

void MultiValue::SetValue(const std::string& value)
{
	DisposeOldValue();

	this->type = MVT_STRING;
	this->value.stringValue = new std::string(value);
}

void MultiValue::SetValue(const double value)
{
	DisposeOldValue();

	type = MVT_REAL;
	this->value.realValue = value;
}

void MultiValue::SetValue(const uint64_t value)
{
	DisposeOldValue();

	type = MVT_UINT;
	this->value.unsignedValue = value;
}

void MultiValue::SetValue(const int64_t value)
{
	DisposeOldValue();

	type = MVT_INT;
	this->value.signedValue = value;
}

void MultiValue::SetValue(const int size, const MultiValue *array)
{
	DisposeOldValue();

	type = MVT_ARRAY;
	length = size;
	useCounter = new int;
	*useCounter = 1;
	value.arrayValue = new MultiValue[size];
	memcpy(value.arrayValue, array, size * sizeof(MultiValue));
}

void MultiValue::SetValue(const std::map<MultiValue, MultiValue> &map)
{
	DisposeOldValue();

	type = MVT_MAP;
	useCounter = new int;
	*useCounter = 1;
	value.mapValue = new std::map<MultiValue, MultiValue>(map);
}

void MultiValue::SetValue()
{
	DisposeOldValue();

	this->value.unsignedValue = 0ULL;
	this->type = MVT_NULL;
}

MultiValue MultiValue::operator+(const MultiValue& rvalue) const
{
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", MULE_FILE, __LINE__);
	
	switch (type)
	{
	case MVT_INT:
		return MultiValue(value.signedValue + rvalue.value.signedValue);
		break;
	case MVT_UINT:
		return MultiValue(value.unsignedValue + rvalue.value.unsignedValue);
		break;
	case MVT_REAL:
		return MultiValue(value.realValue + rvalue.value.realValue);
		break;
	case MVT_STRING:
		return MultiValue(*value.stringValue + *rvalue.value.stringValue);
		break;
	default:
		throw Exception::Exception("Type unknown.", MULE_FILE, __LINE__);
	}
}

MultiValue MultiValue::operator-(const MultiValue& rvalue) const
{
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", MULE_FILE, __LINE__);

	switch (type)
	{
	case MVT_INT:
		return MultiValue(value.signedValue - rvalue.value.signedValue);
		break;
	case MVT_UINT:
		return MultiValue(value.unsignedValue - rvalue.value.unsignedValue);
		break;
	case MVT_REAL:
		return MultiValue(value.realValue - rvalue.value.realValue);
		break;
	case MVT_STRING:
		throw InvalidOperationException("String connot be subtracted.", MULE_FILE, __LINE__);
		break;
	default:
		throw Exception::Exception("Type unknown.", MULE_FILE, __LINE__);
	}
}

MultiValue MultiValue::operator*(const MultiValue& rvalue) const
{
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", MULE_FILE, __LINE__);

	switch (type)
	{
	case MVT_INT:
		return MultiValue(value.signedValue * rvalue.value.signedValue);
		break;
	case MVT_UINT:
		return MultiValue(value.unsignedValue * rvalue.value.unsignedValue);
		break;
	case MVT_REAL:
		return MultiValue(value.realValue * rvalue.value.realValue);
		break;
	case MVT_STRING:
		throw InvalidOperationException("String connot be multiplied.", MULE_FILE, __LINE__);
		break;
	default:
		throw Exception::Exception("Type unknown.", MULE_FILE, __LINE__);
	}
}

MultiValue MultiValue::operator/(const MultiValue& rvalue) const
{
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", MULE_FILE, __LINE__);

	switch (type)
	{
	case MVT_INT:
		return MultiValue(value.signedValue / rvalue.value.signedValue);
		break;
	case MVT_UINT:
		return MultiValue(value.unsignedValue / rvalue.value.unsignedValue);
		break;
	case MVT_REAL:
		return MultiValue(value.realValue / rvalue.value.realValue);
		break;
	case MVT_STRING:
		throw InvalidOperationException("String connot be divided.", MULE_FILE, __LINE__);
		break;
	default:
		throw Exception::Exception("Type unknown.", MULE_FILE, __LINE__);
	}
}

const MultiValue& MultiValue::operator=(const MultiValue& rvalue)
{
	DisposeOldValue();

	metadata = rvalue.metadata;
	type = rvalue.type;
	
	if (type == MVT_STRING)
	{
		value.stringValue = new std::string(*rvalue.value.stringValue);
	}
	else if (type == MVT_MAP)
	{
		value.mapValue = rvalue.value.mapValue;
		useCounter = rvalue.useCounter;
		*useCounter += 1;
	}
	else if (type == MVT_ARRAY)
	{
		value.arrayValue = rvalue.value.arrayValue;
		useCounter = rvalue.useCounter;
		*useCounter += 1;
	}
	else
		value = rvalue.value;

	return *this;
}

const MultiValue &mule::Data::Basic::MultiValue::operator=(MultiValue &&movee) noexcept
{
	DisposeOldValue();

	type = movee.type;
	value = movee.value;
	metadata = movee.metadata;
	useCounter = movee.useCounter;

	if (type == MVT_STRING && value.stringValue != nullptr)
	{
		movee.value.stringValue = nullptr;
	}
	if (type == MVT_MAP && value.mapValue != nullptr)
	{
		movee.useCounter = nullptr;
		movee.value.mapValue = nullptr;
	}
	if (type == MVT_ARRAY && value.arrayValue != nullptr)
	{
		movee.useCounter = nullptr;
		movee.value.arrayValue = nullptr;
	}

	// 阻止任何形式的数据回收
	movee.type = MVT_NULL;

	length = movee.length;

	return *this;
}
