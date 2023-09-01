#include "BisEnv.h"

#include <Container/IsoContainer.h>
#include <Data/Array.h>
#include <Data/Referrence.h>
#include <Data/Mappifier.h>
#include <Data/TableRef.h>
#include <Stream/ElfStream.h>

#include "CStyleInitHandler.h"
#include "../Xml/XmlHandler.h"
#include "../Csv/CsvHandler.h"

using namespace mule::Data;

mule::Data::Basic::Type::DataHandler *BisEnvGetHandler(const char16_t *name)
{
	static char16_t cStyleName[] = u"c-style";
	static char16_t xmlHandlerName[] = u"xml";
	static char16_t csvName[] = u"csv";

	if (!memcmp(name, cStyleName, sizeof(cStyleName)))
		return new mule::Cpp::CStyleInitHandler();
	if (!memcmp(name, xmlHandlerName, sizeof(xmlHandlerName)))
		return new mule::Xml::XmlHandler();
	if (!memcmp(name, csvName, sizeof(csvName)))
		return new mule::Csv::CsvHandler();

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
	ret->nextCreator = new TableRef::TableRefCreator();
	ret->nextCreator->nextCreator = new Referrence::ReferrenceCreator();
	ret->nextCreator->nextCreator->nextCreator = new BasicFieldCreator();

	return ret;
}

mule::Cpp::PluginDescription mule::Cpp::bisDesc = {
	"bis-sis basic",
	"xiyan",
	"MIT",
	1, 0,
	"Basic mule features.",
	BisEnvGetCreators,
	BisEnvGetHandler,
	nullptr,
	BisEnvApplyStream,
	BisEnvApplyContainer
};
