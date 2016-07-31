//
// Tomato Media Core
// ����̽��
// ���ߣ�SunnyCase
// ����ʱ�䣺2015-12-31
//
#pragma once
#include "common.h"
#include <MLang.h>

DEFINE_NS_PLATFORM

class EncodingDetector
{
public:
	EncodingDetector();
	~EncodingDetector();
	
	DWORD DetectCodePage(const std::string& text);
private:
	WRL::ComPtr<IMultiLanguage2> _multiLang;
};

END_NS_PLATFORM