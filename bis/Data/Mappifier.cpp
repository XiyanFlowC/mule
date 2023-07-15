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
	if (realm->GetTypeName() == "structure" || realm->GetTypeName() == "table")
		values.push(new MultiValue(MultiValue::MVT_MAP));
	else
		key = name;
}

void mule::Data::Mappifier::OnRealmExit(Object *realm, std::string name)
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
}

void mule::Data::Mappifier::OnDataRead(const MultiValue &value)
{
	(*values.top()->value.mapValue)[MultiValue(key)] = value;
}

MultiValue mule::Data::Mappifier::OnDataWrite()
{
	throw Exception::InvalidOperationException("Mappifier cannot handle write.", __FILE__, __LINE__);
}

MultiValue mule::Data::Mappifier::GetMap() const
{
	return result;
}

void mule::Data::Mappifier::OnRealmEnter(Object *realm, int idx)
{
	if (realm->GetTypeName() == "structure" || realm->GetTypeName() == "table")
		values.push(new MultiValue(MultiValue::MVT_MAP));
	else
		key = (uint64_t)idx;
}

void mule::Data::Mappifier::OnRealmExit(Object *realm, int idx)
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
}
