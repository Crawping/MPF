//
// MPF Platform
// Render Command Buffer
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-07-22
//
#pragma once
#include "../../inc/common.h"
#include "Geometry.h"
#include "ResourceRef.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class ResourceManagerBase;

class RenderCommandBuffer : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IRenderCommandBuffer>
{
public:
	RenderCommandBuffer(ResourceManagerBase* resourceManager);

	// ͨ�� RuntimeClass �̳�
	STDMETHODIMP DrawGeometry(IResource * geometry) override;

	const std::vector<WRL::ComPtr<ResourceRef>>& GetGeometries() const noexcept { return _geometries; }
	ResourceManagerBase* GetResourceManager() const noexcept { return _resourceManager.Get(); }
private:
	WRL::ComPtr<ResourceManagerBase> _resourceManager;
	std::vector<WRL::ComPtr<ResourceRef>> _geometries;
};
END_NS_PLATFORM