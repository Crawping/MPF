//
// MPF Platform
// Camera
// ���ߣ�SunnyCase
// ����ʱ�䣺2017-02-08
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include "ResourceRef.h"
#include <DirectXMath.h>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct Camera : public ResourceBase
{
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 Projection;
};

END_NS_PLATFORM