#include "Mappifier.h"
#include "../Exception/InvalidParameterException.h"
#include "../Exception/InvalidOperationException.h"
#include "Table.h"

using namespace mule::Data::Basic;

void mule::Data::Mappifier::OnSheetReadStart()
{
	if (status != DHMS_IDLE) throw Exception::InvalidOperationException("Start read when not in idle.", __FILE__, __LINE__);
	status = DHMS_READ;
}

void mule::Data::Mappifier::OnSheetWriteStart()
{
	if (status != DHMS_IDLE) throw Exception::InvalidOperationException("Start write when not in idle.", __FILE__, __LINE__);
	status = DHMS_WRITE;
}

void mule::Data::Mappifier::OnSheetReadEnd()
{
	if (status != DHMS_READ) throw Exception::InvalidOperationException("End read when not in read.", __FILE__, __LINE__);
	status = DHMS_IDLE;
}

void mule::Data::Mappifier::OnSheetWriteEnd()
{
	if (status != DHMS_WRITE) throw Exception::InvalidOperationException("End write when not in write.", __FILE__, __LINE__);
	status = DHMS_IDLE;
}

void mule::Data::Mappifier::OnRealmEnter(Object *realm, std::string name)
{
	if (status == DHMS_READ)
	{
		if (realm->GetTypeName() == "structure" || realm->GetTypeName() == "table")
			values.push(new MultiValue(MultiValue::MVT_MAP));
		else
			key = name;
	}
	else if (status == DHMS_WRITE)
	{
		auto *itr = values.top();
		if (itr->type != MultiValue::MVT_MAP)
		{
			throw Exception::InvalidOperationException("Not map but entered a realm.", __FILE__, __LINE__);
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

void mule::Data::Mappifier::OnRealmExit(Object *realm, std::string name)
{
	if (status == DHMS_READ)
	{
		if (realm->GetTypeName() == "structure" || realm->GetTypeName() == "table")
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

void mule::Data::Mappifier::OnDataRead(const MultiValue &value)
{
	(*values.top()->value.mapValue)[key] = value;
}

MultiValue mule::Data::Mappifier::OnDataWrite()
{
	return *values.top();
}

void mule::Data::Mappifier::AppendMetadatum(std::string name, const Basic::MultiValue &datum)
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

MultiValue mule::Data::Mappifier::GetMap() const
{
	return result;
}

void mule::Data::Mappifier::SetMap(MultiValue &value)
{
	result = value;
}

void mule::Data::Mappifier::OnRealmEnter(Object *realm, int idx)
{
	if (status == DHMS_READ)
	{
		if (realm->GetTypeName() == "structure" || realm->GetTypeName() == "table")
			values.push(new MultiValue(MultiValue::MVT_MAP));
		else
			key = (uint64_t)idx;
	}
	else if (status == DHMS_WRITE)
	{
		auto* itr = values.top();
		if (itr->type != MultiValue::MVT_MAP)
		{
			throw Exception::InvalidOperationException("Not map but entered a realm.", __FILE__, __LINE__);
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

void mule::Data::Mappifier::OnRealmExit(Object *realm, int idx)
{
	if (status == DHMS_READ)
	{
		if (realm->GetTypeName() == "structure")
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
