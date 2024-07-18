#include "BisEnv.h"

#include <Container/IsoContainer.h>
#include <Data/Array.h>
#include <Data/Reference.h>
#include <Data/SmartReference.h>
#include <SheetReference.h>
#include <Data/VarChar.h>
#include <Data/Storage/BinaryBlock.h>
#include <Data/BasicTypeCreator.h>
#include <Stream/ElfStream.h>
#include <MuleRtVersion.h>

#include "CStyleInitHandler.h"
#include "../Xml/XmlHandler.h"
#include "../Csv/CsvHandler.h"
#include "../Csv/CsvFileHandler.h"

using namespace mule::Data;

mule::Data::Basic::Type::DataHandler *BisEnvGetDataHandler(const char16_t *name)
{
	static char16_t cStyleName[] = u"inc";
	static char16_t xmlHandlerName[] = u"xml";
	static char16_t csvName[] = u"csv";

	if (!memcmp(name, cStyleName, sizeof(cStyleName)))
		return new mule::Cpp::CStyleInitHandler();
	if (!memcmp(name, xmlHandlerName, sizeof(xmlHandlerName)))
		return new mule::Xml::XmlHandler();
	if (!memcmp(name, csvName, sizeof(csvName)))
		return new mule::Csv::CsvOutHandler();

	return nullptr;
}

mule::Data::Basic::Type::FileHandler *BisEnvGetFileHandler(const char16_t *name)
{
	static char16_t cStyleName[] = u"inc";
	static char16_t xmlHandlerName[] = u"xml";
	static char16_t csvName[] = u"csv";

	/*if (!memcmp(name, cStyleName, sizeof(cStyleName)))
		return new mule::Cpp::CStyleInitHandler();*/
	if (!memcmp(name, xmlHandlerName, sizeof(xmlHandlerName)))
		return new mule::Xml::XmlHandler();
	if (!memcmp(name, csvName, sizeof(csvName)))
		return new mule::Csv::CsvFileHandler();

	return nullptr;
}

xybase::Stream *BisEnvApplyStream(const char16_t *name, xybase::Stream *infraStream)
{
	static char16_t elfName[] = u"elf";
	
	if (!memcmp(name, elfName, sizeof(elfName)))
	{
		return new mule::Stream::ElfStream(infraStream);
	}

	return nullptr;
}

xybase::FileContainer *BisEnvApplyContainer(const char16_t *name, xybase::Stream *infraStream)
{
	static char16_t isoName[] = u"iso";

	if (!memcmp(name, isoName, sizeof(isoName)))
	{
		return new mule::Container::IsoContainer(infraStream);
	}

	return nullptr;
}

TypeCreator *BisEnvGetCreators()
{
	auto ret = new Array::ArrayCreator();
	ret->nextCreator = new Reference::ReferenceCreator();
	ret->nextCreator->nextCreator = new SmartReference::SmartReferenceCreator();
	ret->nextCreator->nextCreator->nextCreator = new BasicTypeCreator();
	ret->nextCreator->nextCreator->nextCreator->nextCreator = new Storage::BinaryBlock::BinaryBlockCreator();
	ret->nextCreator->nextCreator->nextCreator->nextCreator->nextCreator = new VarChar::VarCharCreator();
	ret->nextCreator->nextCreator->nextCreator->nextCreator->nextCreator->nextCreator = new mule::SheetReference::SheetReferenceCreator();

	return ret;
}

mule::PluginDescription mule::Cpp::bisDesc = {
	L"bis-sis basic",
	L"xiyan",
	L"MIT",
	1, 0,
	L"Elemental features.",
	MULERT_MAJOR_VERSION, MULERT_MINOR_VERSION,
	BisEnvGetCreators,
	BisEnvGetDataHandler,
	BisEnvGetFileHandler,
	BisEnvApplyStream,
	BisEnvApplyContainer,
	nullptr,
	nullptr,
	nullptr,
	nullptr
};
