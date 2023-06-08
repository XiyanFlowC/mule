#include "DataHandler.h"

using namespace mule::Data::Basic;
using namespace mule::Exception;

void DataHandler::BeforeSheetRead(std::string tableName)
{
	if (status != DHS_IDLE) throw InvalidOperationException("Try to start a sheet process while processing another sheet!", __FILE__, __LINE__);

	status = DHS_SHEET_READ;
	this->tableName = tableName;
	OnSheetReadingStarted();
}

void DataHandler::BeforeSheetWrite(std::string tableName)
{
	if (status != DHS_IDLE) throw InvalidOperationException("Try to start a sheet process while processing another sheet!", __FILE__, __LINE__);

	status = DHS_SHEET_WRITE;
	this->tableName = tableName;
	OnSheetWritingStarted();
}

void DataHandler::AfterSheetRead()
{
	if (status != DHS_SHEET_READ) throw InvalidOperationException("Try to end sheet reading while no sheet is being read!", __FILE__, __LINE__);

	status = DHS_IDLE;
	OnSheetReadingEnded();
}

void DataHandler::AfterSheetWrite()
{
	if (status != DHS_SHEET_WRITE) throw InvalidOperationException("Try to end sheet reading while no sheet is being read!", __FILE__, __LINE__);

	status = DHS_IDLE;
	OnSheetWritingEnded();
}

void DataHandler::BeforeRecordRead()
{
	if (status != DHS_SHEET_READ) throw InvalidOperationException("Try to start read whilst not reading a sheet.", __FILE__, __LINE__);

	status = DHS_RECORD_READ;
	OnRecordReadingStarted();
}

void DataHandler::BeforeRecordWrite()
{
	if (status != DHS_SHEET_WRITE) throw InvalidOperationException("Try to start write record whilst not writing a sheet.", __FILE__, __LINE__);

	status = DHS_RECORD_WRITE;
	OnRecordWritingStarted();
}

void DataHandler::AfterRecordRead()
{
	if (status != DHS_RECORD_READ) throw InvalidOperationException("Cannot end reading whilst not reading.", __FILE__, __LINE__);
	status = DHS_SHEET_READ;
	OnRecordReadingEnded();
}

void DataHandler::AfterRecordWrite()
{
	if (status != DHS_RECORD_WRITE) throw InvalidOperationException("Cannot end record write while not writing a record.", __FILE__, __LINE__);
	status = DHS_SHEET_WRITE;
	OnRecordWritingEnded();
}

void DataHandler::AfterCellRead(const MultiValue &value)
{
	OnCellRead(value);
}

const MultiValue DataHandler::BeforeCellWrite()
{
	return OnCellWrite();
}

DataHandler::DataHandler() : tableName(), status(DHS_IDLE)
{
}

void DataHandler::OnSheetReadingStarted()
{
}

void DataHandler::OnSheetWritingStarted()
{
}

void DataHandler::OnSheetReadingEnded()
{
}

void DataHandler::OnSheetWritingEnded()
{
}

void DataHandler::OnRecordReadingStarted()
{
}

void DataHandler::OnRecordWritingStarted()
{
}

void DataHandler::OnRecordReadingEnded()
{
}

void DataHandler::OnRecordWritingEnded()
{
}

void DataHandler::OnCellRead(const MultiValue &value)
{
}

MultiValue DataHandler::OnCellWrite()
{
	return MultiValue();
}
