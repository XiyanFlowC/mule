#include "Stringfier.h"

using namespace mule::Data;
using namespace mule::Data::Basic;

Stringfier::Stringfier(MultiValue *target)
{
	this->target = target;
	sb = new xybase::StringBuilder();
}

Stringfier::~Stringfier()
{
	delete sb;
}

void Stringfier::OnSheetReadingStarted()
{
	sb->Append(tableName.c_str());
	sb->Append(":{");
}

void Stringfier::OnSheetReadingEnded()
{
	sb->Append("}");
	target->SetValue(sb->ToString());
}

void Stringfier::OnRecordReadingStarted()
{
	sb->Append("[");
}

void Stringfier::OnRecordReadingEnded()
{
	sb->Append("]");
}

void Stringfier::OnCellRead(const MultiValue &value)
{
	sb->Append(value.Stringfy().c_str());
	sb->Append(',');
}
