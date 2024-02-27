#include "Configuration.h"
#include <xystring.h>

using namespace mule::Data::Basic;

mule::Configuration &mule::Configuration::GetInstance()
{
	static Configuration _inst;
	return _inst;
}

std::u16string mule::Configuration::ResolveVariable(const std::u16string &str)
{
	auto ret = str;
	auto startPosition = ret.find(u"${");
	while (startPosition != std::u16string::npos)
	{
		auto endPosition = ret.find(u"}", startPosition);
		auto variableName = ret.substr(startPosition + 2, endPosition - startPosition - 2);
		auto value = xybase::string::to_utf16(Configuration::GetInstance().GetVariable(variableName.c_str()).ToString());
		ret.replace(startPosition, endPosition - startPosition + 1, value);
		startPosition = ret.find(u"${", startPosition + value.length());
	}
	return ret;
}

void mule::Configuration::ResetVariable(const char16_t *name)
{
	variables.erase(name);
}

void mule::Configuration::SetVariable(const char16_t *name, mule::Data::Basic::MultiValue value)
{
	variables[name] = value;
}

void mule::Configuration::SetVariable(const char16_t *name, unsigned long long uval)
{
	SetVariable(name, MultiValue{ (uint64_t)uval });
}

void mule::Configuration::SetVariable(const char16_t *name, long long val)
{
	SetVariable(name, MultiValue{ (int64_t)val });
}

void mule::Configuration::SetVariable(const char16_t *name, double rval)
{
	SetVariable(name, MultiValue{ rval });
}

void mule::Configuration::SetVariable(const char16_t *name, const char16_t *sval)
{
	SetVariable(name, MultiValue{ sval });
}

mule::Data::Basic::MultiValue mule::Configuration::GetVariable(const char16_t *name)
{
	if (!variables.contains(name)) throw ConfigurationNotFoundException(xybase::string::to_wstring(name));
	return variables[name];
}

const std::u16string mule::Configuration::GetString(const char16_t *name)
{
	auto &&mv = GetVariable(name);
	if (!mv.IsType(MultiValue::MVT_STRING)) throw ConfigurationTypeMismatch(xybase::string::to_wstring(name));
	return *mv.value.stringValue;
}

double mule::Configuration::GetReal(const char16_t *name, double def)
{
	if (!variables.contains(name)) return def;
	auto &&mv = variables[name];
	if (!mv.IsType(MultiValue::MVT_REAL)) throw ConfigurationTypeMismatch(xybase::string::to_wstring(name));
	return mv.value.realValue;
}

long long mule::Configuration::GetSigned(const char16_t *name, long long def)
{
	if (!variables.contains(name)) return def;
	auto &&mv = variables[name];
	if (!mv.IsType(MultiValue::MVT_INT)) throw ConfigurationTypeMismatch(xybase::string::to_wstring(name));
	return mv.value.signedValue;
}

unsigned long long mule::Configuration::GetUnsigned(const char16_t *name, unsigned long long def)
{
	if (!variables.contains(name)) return def;
	auto &&mv = variables[name];
	if (!mv.IsType(MultiValue::MVT_UINT)) throw ConfigurationTypeMismatch(xybase::string::to_wstring(name));
	return mv.value.unsignedValue;
}

bool mule::Configuration::IsExist(const char16_t *name)
{
	return variables.contains(name);
}

mule::Configuration::ConfigurationNotFoundException::ConfigurationNotFoundException(std::wstring name)
	: RuntimeException(name + L" cannot be found in the variables.", 4000100)
{
}

mule::Configuration::ConfigurationNotFoundException::~ConfigurationNotFoundException()
{
}

mule::Configuration::ConfigurationTypeMismatch::ConfigurationTypeMismatch(std::wstring name)
	: RuntimeException(name + L" do not match the type required.", 4000101)
{
}

mule::Configuration::ConfigurationTypeMismatch::~ConfigurationTypeMismatch()
{
}
