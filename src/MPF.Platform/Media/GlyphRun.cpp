//
// MPF Platform
// Glyph Run
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-08-07
//
#include "stdafx.h"
#include "GlyphRun.h"
using namespace WRL;
using namespace NS_PLATFORM;

GlyphRun::GlyphRun()
{
	ThrowIfNot(FT_Init_FreeType(&_freetype) == 0, L"Cannot initialize Freetype.");
}
