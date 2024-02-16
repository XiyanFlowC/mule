#include "Mappifier.h"
#include <Exception/InvalidParameterException.h>
#include <Exception/InvalidOperationException.h>

using namespace mule::Data::Basic;
using namespace xybase;

void mule::Cpp::Mappifier::OnSheetReadStart()
{
	if (status != DHMS_IDLE) throw InvalidOperationException(L"Start read when not in idle.", __LINE__);
	status = DHMS_READ;
}

void mule::Cpp::Mappifier::OnSheetWriteStart()
{
	if (status != DHMS_IDLE) throw InvalidOperationException(L"Start write when not in idle.", __LINE__);
	status = DHMS_WRITE;
}

void mule::Cpp::Mappifier::OnSheetReadEnd()
{
	if (status != DHMS_READ) throw InvalidOperationException(L"End read when not in read.", __LINE__);
	status = DHMS_IDLE;
}

void mule::Cpp::Mappifier::OnSheetWriteEnd()
{
	if (status != DHMS_WRITE) throw InvalidOperationException(L"End write when not in write.", __LINE__);
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
			throw InvalidOperationException(L"Not map but entered a realm.", __LINE__);
		}
		else
		{
			auto it = itr->value.mapValue->find(name);
			if (it != itr->value.mapValue->end())
			{
				values.push(&it->second);
			}
		}
	}
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
	(*values.top()->value.mapValue)[key] = value;
}

const MultiValue mule::Cpp::Mappifier::OnDataWrite()
{
	return *values.top();
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
	result = value;
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
			throw InvalidOperationException(L"Not map but entered a realm.", __LINE__);
		}
		else
		{
			auto it = itr->value.mapValue->find((uint64_t)idx);
			if (it != itr->value.mapValue->end())
			{
				values.push(&it->second);
			}
		}
	}
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
