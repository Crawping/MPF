//
// MPF Platform
// Geometry
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-07-21
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct LineGeometry : public ResourceBase
{
	LineGeometryData Data;
};

struct StreamGeometry : public ResourceBase
{

};

END_NS_PLATFORM