//
// MPF Platform
// Render Command Buffer
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-07-22
//
#include "stdafx.h"
#include "RenderCommandBuffer.h"
#include "ResourceManagerBase.h"
using namespace WRL;
using namespace NS_PLATFORM;

RenderCommandBuffer::RenderCommandBuffer(ResourceManagerBase* resourceManager)
	:_resourceManager(resourceManager)
{
}

HRESULT RenderCommandBuffer::DrawGeometry(IResource * geometry, IResource* pen)
{
	try
	{
		_geometries.emplace_back<GeometryRenderCommand>({ static_cast<ResourceRef*>(geometry), static_cast<ResourceRef*>(pen) });
		return S_OK;
	}
	CATCH_ALL();
}
