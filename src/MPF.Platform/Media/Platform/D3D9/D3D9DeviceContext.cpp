//
// MPF Platform
// Direct3D 9 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#include "stdafx.h"
#include "D3D9DeviceContext.h"
#include <process.h>
#include "../../ResourceManager.h"
#include "resource.h"
#include "NativeApplication.h"
#include "Controls/NativeWindow.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D9;
using namespace concurrency;

namespace
{
	class DummyWindowCallback : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, INativeWindowCallback>
	{
		// 通过 RuntimeClass 继承
		STDMETHODIMP OnClosing(void) override
		{
			return S_OK;
		}
		STDMETHODIMP OnLocationChanged(Point location) override
		{
			return S_OK;
		}
		STDMETHODIMP OnSizeChanged(Point size) override
		{
			return S_OK;
		}
	};
}

D3D9DeviceContext::D3D9DeviceContext(IDeviceContextCallback* callback)
	:_rootSwapChainLock(5000), _callback(callback), _renderObjectContainer(std::make_shared<RenderableObjectContainer<RenderableObject>>())
{
	_d3d.Attach(Direct3DCreate9(D3D_SDK_VERSION));
	ThrowIfNot(_d3d, L"Cannot Initialize Direct3D 9 Interface.");

	WeakRef<D3D9DeviceContext> weakRef(AsWeak());
	NativeApplication::AddAtExit([weakRef]
	{
		if (auto me = weakRef.Resolve())
			me->_isRenderLoopActive = false;
	});

	auto dummyCallback = Make<DummyWindowCallback>();
	ThrowIfFailed(CreateNativeWindow(dummyCallback.Get(), &_dummyWindow));
}

D3D9DeviceContext::~D3D9DeviceContext()
{
	_isRenderLoopActive = false;
}

STDMETHODIMP D3D9DeviceContext::CreateSwapChain(INativeWindow * window, ISwapChain ** swapChain)
{
	try
	{
		EnsureDevice();
		ComPtr<D3D9ChildSwapChain> d3dSwapChain;
		_rootSwapChain->CreateAdditionalSwapChain(window, &d3dSwapChain);
		_childSwapChains.emplace_back(d3dSwapChain->GetWeakContext());
		ThrowIfFailed(d3dSwapChain.CopyTo(swapChain));
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP D3D9DeviceContext::Update()
{
	try
	{
		_callback->OnRender();
		UpdateResourceManagers();
		UpdateRenderObjects();

		return S_OK;
	}
	CATCH_ALL();
}

namespace
{
	const DWORD* LoadShaderResource(int id)
	{
		auto hres = FindResourceW(ModuleHandle, MAKEINTRESOURCE(id), L"SHADER");
		ThrowWin32IfNot(hres);
		auto handle = LoadResource(ModuleHandle, hres);
		ThrowWin32IfNot(handle);
		auto ret = reinterpret_cast<const DWORD*>(LockResource(handle));
		ThrowWin32IfNot(ret);
		return ret;
	}
}

task<void> D3D9DeviceContext::CreateDeviceResourcesAsync()
{
	ComPtr<IDirect3DVertexDeclaration9> vertexDeclaration;
	static const D3DVERTEXELEMENT9 elements[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, offsetof(StrokeVertex, Data1), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, offsetof(StrokeVertex, Data2), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, offsetof(StrokeVertex, SegmentType), D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, offsetof(StrokeVertex, Data3), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 0, offsetof(StrokeVertex, Data4), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 0, offsetof(StrokeVertex, Data5), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		D3DDECL_END()
	};
	ThrowIfFailed(_device->CreateVertexDeclaration(elements, &vertexDeclaration));
	ThrowIfFailed(_device->SetVertexDeclaration(vertexDeclaration.Get()));

	auto uiVSData = LoadShaderResource(IDR_D3D9_UIVERTEXSHADER);
	ComPtr<IDirect3DVertexShader9> vs;
	ThrowIfFailed(_device->CreateVertexShader(uiVSData, &vs));

	auto uiPSData = LoadShaderResource(IDR_D3D9_UIPIXELSHADER);
	ComPtr<IDirect3DPixelShader9> ps;
	ThrowIfFailed(_device->CreatePixelShader(uiPSData, &ps));

	ThrowIfFailed(_device->SetVertexShader(vs.Get()));
	ThrowIfFailed(_device->SetPixelShader(ps.Get()));

	ThrowIfFailed(_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
	ThrowIfFailed(_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
	ThrowIfFailed(_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

	return task_from_result();
}

void D3D9DeviceContext::StartRenderLoop()
{
	if (!_isRenderLoopActive.load(std::memory_order_acquire))
	{
		auto handle = _beginthread(RenderLoop, 0, new WeakRef<D3D9DeviceContext>(GetWeakContext()));
		ThrowIfNot(handle != -1, L"Cannot create Render Loop Thread.");
		NativeApplication::AddEventToWait(reinterpret_cast<HANDLE>(handle));
	}
}

bool D3D9DeviceContext::IsActive() const noexcept
{
	return _isRenderLoopActive;
}

void D3D9DeviceContext::DoFrame()
{
	for (auto&& resMgrRef : _resourceManagers)
		if (auto resMgr = resMgrRef.Resolve())
			resMgr->UpdateGPU();

	std::vector<ComPtr<D3D9SwapChainBase>> swapChains;
	{
		auto locker = _rootSwapChainLock.Lock();
		for (auto&& weakRef : _childSwapChains)
			if (auto swapChain = weakRef.Resolve())
				swapChains.emplace_back(swapChain);
	}
	for (auto&& swapChain : swapChains)
		swapChain->DoFrame();
}

void D3D9DeviceContext::UpdateResourceManagers()
{

}

void D3D9DeviceContext::ActiveDeviceAndStartRender()
{
	ComPtr<D3D9DeviceContext> thisHolder;
	CreateDeviceResourcesAsync().then([thisHolder, this]
	{
		this->StartRenderLoop();
	});
}

void D3D9DeviceContext::DoFrameWrapper() noexcept
{
	auto hr = S_OK;
	try
	{
		DoFrame();
	}
	CATCH_ALL_WITHHR(hr);
}

void D3D9DeviceContext::UpdateRenderObjects()
{
	_renderObjectContainer->Update();
}

void D3D9DeviceContext::RenderLoop(void * weakRefVoid)
{
	std::unique_ptr<WeakRef<D3D9DeviceContext>> weakRef(reinterpret_cast<WeakRef<D3D9DeviceContext>*>(weakRefVoid));
	if (auto me = weakRef->Resolve())
		if (me->_isRenderLoopActive.exchange(true))return;

	while (true)
	{
		if (auto me = weakRef->Resolve())
		{
			if (!me->IsActive())
			{
				me->_isRenderLoopActive.store(false, std::memory_order_release);
				break;
			}
			me->DoFrameWrapper();
		}
	}
}

void D3D9DeviceContext::BeginResetDevice()
{
	for (auto&& resMgrRef : _resourceManagers)
		if (auto resMgr = resMgrRef.Resolve())
			resMgr->BeginResetDevice();
	ThrowIfFailed(_device->SetPixelShader(nullptr));
	ThrowIfFailed(_device->SetVertexShader(nullptr));
	ThrowIfFailed(_device->SetVertexDeclaration(nullptr));
	ThrowIfFailed(_device->SetStreamSource(0, nullptr, 0, 0));
}

void D3D9DeviceContext::EndResetDevice()
{
	CreateDeviceResourcesAsync().get();
	for (auto&& resMgrRef : _resourceManagers)
		if (auto resMgr = resMgrRef.Resolve())
			resMgr->EndResetDevice();
}

void D3D9DeviceContext::EnsureDevice()
{
	auto locker = _rootSwapChainLock.Lock();
	if (!_rootSwapChain)
	{
		auto d3dSwapChain = Make<D3D9SwapChain>(_d3d.Get(), _dummyWindow.Get());
		_device = d3dSwapChain->GetDevice();
		_rootSwapChain = d3dSwapChain;
		ActiveDeviceAndStartRender();
	}
}

HRESULT D3D9DeviceContext::CreateResourceManager(IResourceManager **resMgr)
{
	try
	{
		EnsureDevice();
		ComPtr<D3D9DeviceContext> me(this);
		auto myResMgr = Make<ResourceManager<PlatformId::D3D9>>(me);
		_resourceManagers.emplace_back(myResMgr->GetWeakContext());
		*resMgr = myResMgr.Detach();
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT D3D9DeviceContext::CreateRenderableObject(IRenderableObject ** obj)
{
	try
	{
		*obj = Make<RenderableObjectRef>(_renderObjectContainer, _renderObjectContainer->Allocate()).Detach();
		return S_OK;
	}
	CATCH_ALL();
}
