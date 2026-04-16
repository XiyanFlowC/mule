#include "Mappifier.h"
#include <Exception/InvalidParameterException.h>
#include <Exception/InvalidOperationException.h>
#include <Configuration.h>
#include <xystring.h>
#include "../Lua/LuaHost.h"

using namespace mule::Data::Basic;
using namespace xybase;

void mule::Cpp::Mappifier::OnSheetReadStart()
{
	if (status != DHMS_IDLE) throw InvalidOperationException(L"Start read when not in idle.", 3601);
	status = DHMS_READ;
}

void mule::Cpp::Mappifier::OnSheetWriteStart()
{
	if (status != DHMS_IDLE) throw InvalidOperationException(L"Start write when not in idle.", 3602);
	status = DHMS_WRITE;
}

void mule::Cpp::Mappifier::OnSheetReadEnd()
{
	if (status != DHMS_READ) throw InvalidOperationException(L"End read when not in read.", 3603);
	status = DHMS_IDLE;
}

void mule::Cpp::Mappifier::OnSheetWriteEnd()
{
	if (status != DHMS_WRITE) throw InvalidOperationException(L"End write when not in write.", 3604);
	status = DHMS_IDLE;
}

void mule::Cpp::Mappifier::OnRealmEnter(Type *realm, const std::u16string &name)
{
	if (status == DHMS_READ)
	{
		if (realm->IsComposite())
			values.push(new MultiValue(MultiValue::MVT_MAP));
		else
			key = name;
	}
	else if (status == DHMS_WRITE)
	{
		auto *itr = values.top();
		if (itr->type != MultiValue::MVT_MAP)
		{
			throw InvalidOperationException(L"Not map but entered a realm.", 36001);
		}
		else
		{
			auto it = itr->value.mapValue->find(name);
			if (it != itr->value.mapValue->end())
			{
				values.push(&it->second);
			}
			else
			{
				throw InvalidParameterException(L"name", L"Name not found in map.", 36002);
			}
		}
	}
	textRealm = !realm->IsComposite() && realm->GetDataType() == u"string/text";
}

void mule::Cpp::Mappifier::OnRealmExit(Type *realm, const std::u16string &name)
{
	if (status == DHMS_READ)
	{
		if (realm->IsComposite())
		{
			auto value = values.top();
			values.pop();
			if (values.empty())
				result = *value;
			else
				(*values.top()->value.mapValue)[MultiValue(name)] = *value;

			delete value;
		}
		else
			key = MultiValue::MV_NULL;
	}
	else if (status == DHMS_WRITE)
	{
		values.pop();
	}
}

void mule::Cpp::Mappifier::OnDataRead(const MultiValue &value)
{
	MultiValue mv{ value };
	if (textRealm && Configuration::GetInstance().IsExist(u"mule.handler.string-read-proc"))
	{
		auto handlerName = Configuration::GetInstance().GetString(u"mule.handler.string-read-proc");
		MultiValue tmp{ value };
		mv = mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(handlerName), 1, &tmp);
	}
	(*values.top()->value.mapValue)[key] = mv;
}

const MultiValue mule::Cpp::Mappifier::OnDataWrite()
{
	MultiValue mv = *values.top();
	if (textRealm && Configuration::GetInstance().IsExist(u"mule.handler.string-write-proc"))
	{
		auto handlerName = Configuration::GetInstance().GetString(u"mule.handler.string-write-proc");
		mv = mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(handlerName), 1, &mv);
	}
	return mv;
}

void mule::Cpp::Mappifier::AppendMetadatum(std::u16string name, const MultiValue &datum)
{
	if (status == DHMS_READ)
	{
		if (key != MultiValue::MV_NULL)
			(*values.top()->value.mapValue)[key].metadata[name] = datum;
		else
			values.top()->metadata[name] = datum;
	}
	else if (status == DHMS_WRITE)
	{
		values.top()->metadata[name] = datum;
	}
}

MultiValue mule::Cpp::Mappifier::GetMap() const
{
	return result;
}

void mule::Cpp::Mappifier::SetMap(MultiValue &value)
{
	values.push(&value);
}

void mule::Cpp::Mappifier::OnRealmEnter(Type *realm, int idx)
{
	if (status == DHMS_READ)
	{
		if (realm->IsComposite())
			values.push(new MultiValue(MultiValue::MVT_MAP));
		else
			key = (uint64_t)idx;
	}
	else if (status == DHMS_WRITE)
	{
		auto *itr = values.top();
		if (itr->type != MultiValue::MVT_MAP)
		{
			throw InvalidOperationException(L"Not map but entered a realm.", 36003);
		}
		else
		{
			auto it = itr->value.mapValue->find((uint64_t)idx);
			if (it != itr->value.mapValue->end())
			{
				values.push(&it->second);
			}
			else
			{
				throw InvalidParameterException(L"idx", L"Index not found in map.", 36004);
			}
		}
	}
	textRealm = !realm->IsComposite() && realm->GetDataType() == u"string/text";
}

void mule::Cpp::Mappifier::OnRealmExit(Type *realm, int idx)
{
	if (status == DHMS_READ)
	{
		if (realm->IsComposite())
		{
			auto value = values.top();
			values.pop();
			if (values.empty())
				result = *value;
			else
				(*values.top()->value.mapValue)[MultiValue((uint64_t)idx)] = *value;

			delete value;
		}
		else
			key = MultiValue::MV_NULL;
	}
	else if (status == DHMS_WRITE)
	{
		values.pop();
	}
}
