/**
* Addressed, lengthed string
* Sniffer and importer
*/
#pragma once

#include <Stream.h>
#include <TextStream.h>

int str_ex(xybase::Stream *input, xybase::TextStream *output, int begin, int end);

int str_im(xybase::TextStream *input, xybase::Stream *output);
