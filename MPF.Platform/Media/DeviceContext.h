//
// MPF Platform
// DeviceContext
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-07-16
//
#pragma once
#include "../../inc/common.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"
END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateDeviceContext(NS_PLATFORM::INativeWindow* window, NS_PLATFORM::IDeviceContext** obj) noexcept;
}