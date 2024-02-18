#include "Sheet.h"
#include "../Configuration.h"

using namespace mule::Data::Basic;

mule::Data::Sheet::Sheet(Basic::Type *infraType, size_t offset, size_t length, std::u16string name)
	: infraType(infraType), offset(offset), length(length), name(name)
{
	structureSimplifySuppression = Configuration::GetInstance().GetSigned(u"mule.data.sheet.structure-simplify-suppression", 0);
}

const std::u16string &mule::Data::Sheet::GetName() const
{
	return name;
}

size_t mule::Data::Sheet::Size() const
{
	if (infraType->Size() == -1) return -1;
	return infraType->Size() * length;
}

std::u16string mule::Data::Sheet::GetDataType() const
{
	return std::u16string(u"Sheet");
}

bool mule::Data::Sheet::IsComposite() const
{
	return true;
}

void mule::Data::Sheet::Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler)
{
	Configuration::GetInstance().SetVariable(u"mule.data.sheet.name", name);
	dataHandler->OnRealmEnter(this, name);
	stream->Seek(offset, xybase::Stream::SeekMode::SM_BEGIN);
	if (length == 1 && !structureSimplifySuppression)
		infraType->Read(stream, dataHandler);
	else for (int i = 0; i < length; ++i) {
		Configuration::GetInstance().SetVariable(u"mule.data.sheet.index", (long long)i);
		dataHandler->OnRealmEnter(infraType, i);
		infraType->Read(stream, dataHandler);
		dataHandler->OnRealmExit(infraType, i);
	}
	dataHandler->OnRealmExit(this, name);
	Configuration::GetInstance().ResetVariable(u"mule.data.sheet.index");
	Configuration::GetInstance().ResetVariable(u"mule.data.sheet.name");
}

void mule::Data::Sheet::Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler)
{
	Configuration::GetInstance().SetVariable(u"mule.data.sheet.name", name);
	fileHandler->OnRealmEnter(this, name);
	stream->Seek(offset, xybase::Stream::SeekMode::SM_BEGIN);
	if (length == 1 && !structureSimplifySuppression)
		infraType->Write(stream, fileHandler);
	else for (int i = 0; i < length; ++i) {
		Configuration::GetInstance().SetVariable(u"mule.data.sheet.index", (long long)i);
		fileHandler->OnRealmEnter(infraType, i);
		infraType->Write(stream, fileHandler);
		fileHandler->OnRealmExit(infraType, i);
	}
	fileHandler->OnRealmExit(this, name);
	Configuration::GetInstance().ResetVariable(u"mule.data.sheet.index");
	Configuration::GetInstance().ResetVariable(u"mule.data.sheet.name");
}
