//
// MPF Platform
// Platform Provider Traits
// ���ߣ�SunnyCase
// ����ʱ�䣺2016-12-25
//
#pragma once
#include "PlatformProvider.h"
#include "../BufferManager.h"

DEFINE_NS_PLATFORM

template<PlatformId PId>
struct PlatformProviderTraits
{
	using VertexBufferManager = NS_PLATFORM::VertexBufferManager<PId>;
};

END_NS_PLATFORM