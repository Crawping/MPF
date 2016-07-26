//
// MPF Platform
// Renderable Object
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-07-22
//
#pragma once
#include "../../inc/common.h"
#include "Geometry.h"
#include "ResourceRef.h"
#include "RenderCommandBuffer.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct IDrawCallList;

class RenderableObject : public ResourceBase
{
public:
	RenderableObject();

	void SetContent(IRenderCommandBuffer* buffer);
	void Update();
	void Draw();
private:
	void SetBufferDirty();
private:
	bool _isDirty;
	bool _isBufferDirty;
	WRL::ComPtr<RenderCommandBuffer> _buffer;
	std::shared_ptr<IDrawCallList> _drawCallList;
};

END_NS_PLATFORM