//
// MPF Platform
// Native Application
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-07-14
//
#pragma once
#include "../inc/common.h"
#include "../inc/NonCopyable.h"
#include "../inc/WeakReferenceBase.h"

DEFINE_NS_PLATFORM
#include "MPF.Platform_i.h"

class NativeApplication : private NonCopyable,
	public WeakReferenceBase<NativeApplication, WRL::RuntimeClassFlags<WRL::ClassicCom>, INativeApplication>
{
public:
	NativeApplication();
	virtual ~NativeApplication();

	// ͨ�� WeakReferenceBase �̳�
	STDMETHODIMP Run(void) override;
private:
private:
};

END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateNativeApplication(NS_PLATFORM::INativeApplication** obj) noexcept;
}