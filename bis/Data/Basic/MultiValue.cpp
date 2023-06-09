#include "MultiValue.h"

using namespace std;

using namespace mule::Data::Basic;
using namespace mule::Exception;

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
		return value.stringValue == rvalue.value.stringValue;
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
	if (type == MVT_STRING)
		delete value.stringValue;
}

MultiValue::MultiValue()
{
	SetValue();
}

MultiValue::MultiValue(const MultiValue& pattern)
{
	type = pattern.type;
	value = pattern.value;
	length = 0;
	
	if (type == MVT_STRING)
	{
		value.stringValue = new std::string(*(pattern.value.stringValue));
	}
}

MultiValue::MultiValue(const std::string& value)
{
	SetValue(value);
}

MultiValue::MultiValue(const double value)
{
	SetValue(value);
}

MultiValue::MultiValue(const uint64_t value)
{
	SetValue(value);
}

MultiValue::MultiValue(const int64_t value)
{
	SetValue(value);
}

mule::Data::Basic::MultiValue::MultiValue(const int size, const MultiValue *array)
{
	SetValue(size, array);
}

MultiValue::MultiValue(const std::map<MultiValue, MultiValue> map)
{
	SetValue(map);
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
		return std::to_string(value.unsignedValue) + "U";
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
			else if ('1' >= *itr && *itr <= '9')
			{
				res = ((unsigned long long) * itr) - '0';
				state = 1;
			}
			else if ('0' == *itr)
			{
				state = 2; // 确认是否为十六进制数
			}
			else throw InvalidParameterException("value", "unexpected character.", __FILE__, __LINE__);
			break;
		case 1:
			if ('0' >= *itr && *itr <= '9')
			{
				res = ((unsigned long long) * itr) - '0' + res * 10;
			}
			else throw InvalidParameterException("value", "unexpected character.", __FILE__, __LINE__);
			break;
		case 2:
			if ('1' >= *itr && *itr <= '7')
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
			else throw InvalidParameterException("value", "unexpected character.", __FILE__, __LINE__);
			break;
		case 3:
			if ('0' >= *itr && *itr <= '9')
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
			else throw InvalidParameterException("value", "unexpected character.", __FILE__, __LINE__);
			break;
		case 4:
			if ('0' >= *itr && *itr <= '7')
			{
				res = (*itr - '0') | (res << 3);
			}
			else if (*itr == 'u' || *itr == 'U')
			{
				type = MVT_UINT;
				goto mvpi_forout; // 脱离循环
			}
			else throw InvalidParameterException("value", "unexpected character.", __FILE__, __LINE__);
			break;
		case 5:
			if (*itr == '0' || *itr == '1')
			{
				res = (res << 1) | (*itr - '0');
			}
			else throw InvalidParameterException("value", "unexpected character.", __FILE__, __LINE__);
		}
	}
	mvpi_forout:
	this->value.unsignedValue = neg ? ~res + 1 : res; // 如果有负号，取反
}

void MultiValue::ParseString(const std::string &value)
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
	} state = MVPS_OUT;
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
	if (state != MVPS_OUT) throw InvalidParameterException("value", "Not a valid string representation.", __FILE__, __LINE__);

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
			else throw InvalidParameterException("value", "not a valid real number.", __FILE__, __LINE__);
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
	if (type == MVT_STRING) delete this->value.stringValue;
	if (type == MVT_MAP) delete this->value.mapValue;
	if (type == MVT_ARRAY) delete this->value.arrayValue;
}

void MultiValue::Parse(const std::string &value)
{
	DisposeOldValue();

	if (value == "null")
	{
		type = MVT_NULL;
		this->value.unsignedValue = 0;
	}

	if (value[0] == '\"') // 以“"”开头视作字符串
	{
		ParseString(value);
	}
	else if (value[0] == '$')
	{
		ParseReal(value);
	}
	else
	{
		ParseInt(value);
	}
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
	value.arrayValue = new MultiValue[size];
	memcpy(value.arrayValue, array, size * sizeof(MultiValue));
}

void MultiValue::SetValue(const std::map<MultiValue, MultiValue> map)
{
	DisposeOldValue();

	type = MVT_MAP;
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
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", __FILE__, __LINE__);
	
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
		throw Exception::Exception("Type unknown.", __FILE__, __LINE__);
	}
}

MultiValue MultiValue::operator-(const MultiValue& rvalue) const
{
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", __FILE__, __LINE__);

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
		throw InvalidOperationException("String connot be subtracted.", __FILE__, __LINE__);
		break;
	default:
		throw Exception::Exception("Type unknown.", __FILE__, __LINE__);
	}
}

MultiValue MultiValue::operator*(const MultiValue& rvalue) const
{
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", __FILE__, __LINE__);

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
		throw InvalidOperationException("String connot be multiplied.", __FILE__, __LINE__);
		break;
	default:
		throw Exception::Exception("Type unknown.", __FILE__, __LINE__);
	}
}

MultiValue MultiValue::operator/(const MultiValue& rvalue) const
{
	if (type != rvalue.type) throw InvalidRValueException("Type mismatching!", __FILE__, __LINE__);

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
		throw InvalidOperationException("String connot be divided.", __FILE__, __LINE__);
		break;
	default:
		throw Exception::Exception("Type unknown.", __FILE__, __LINE__);
	}
}

const MultiValue& MultiValue::operator=(const MultiValue& rvalue)
{
	DisposeOldValue();

	type = rvalue.type;
	value = rvalue.value;
	
	if (type == MVT_STRING)
	{
		value.stringValue = new std::string(*rvalue.value.stringValue);
	}

	return *this;
}
