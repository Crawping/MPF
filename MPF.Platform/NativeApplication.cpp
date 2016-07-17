//
// MPF Platform
// Native Application
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-07-14
//
#include "stdafx.h"
#include "NativeApplication.h"
using namespace WRL;

HRESULT __stdcall CreateNativeApplication(NS_PLATFORM::INativeApplication** obj) noexcept
{
	try
	{
		*obj = Make<NS_PLATFORM::NativeApplication>().Detach();
		return S_OK;
	}
	CATCH_ALL();
}

using namespace NS_PLATFORM;


NativeApplication::NativeApplication()
{
}

NativeApplication::~NativeApplication()
{
}

HRESULT NativeApplication::Run(void)
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return S_OK;
}
