#pragma once

#include <string>

std::string cvt_to_string(const std::wstring &str);

std::wstring cvt_to_wstring(const std::string &str);

void init_codepage(const char *cp);
