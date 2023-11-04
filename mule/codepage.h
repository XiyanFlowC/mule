#pragma once

#include <string>
#include <map>
#include <cstdint>

class CodeCvt
{
	std::map<uint32_t, uint32_t> uc2cp;
	std::map<uint32_t, uint32_t> cp2uc;
public:
	~CodeCvt();

	static CodeCvt &GetInstance();

	std::string CvtToString(const std::wstring &str);

	std::wstring CvtToWString(const std::string &str);

	void Init(const char *cp);
};

std::string cvt_to_string(const std::wstring &str);

std::wstring cvt_to_wstring(const std::string &str);
