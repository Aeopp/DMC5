#ifndef __RENDERPROPERTY_H__
#define __RENDERPROPERTY_H__
#include "EngineTypedef.h"
#include "EngineDefine.h"
#include <map>
#include <functional>
#include <any>
#include "Frustum.h"

BEGIN(ENGINE)

struct ENGINE_DLL DrawInfo
{
	IDirect3DDevice9* _Device{};
	ID3DXEffect* Fx{};
	uint32 PassIndex{ 0u };
	Frustum* _Frustum{ nullptr };
	std::any BySituation{};
	bool IsAfterPostProcessing = false;	// 나중에 그리는 UI 위해 추가. 더 좋은 방법 있으면 알려 좀 주쇼.
};

struct ENGINE_DLL EffectInfo
{
	float SoftParticleDepthBiasScale = 0.0f;
};

struct ENGINE_DLL RenderProperty
{
public : 
	enum class Order : uint8
	{
		GBuffer = 0u,

		ForwardAlphaBlend,

		Shadow,

		Emissive,

		AlphaBlendEffect,
		UI,

		Debug,
		Collider,
		DebugBone,
	};
	bool bRender = true;
	bool bShadowCache = false;
	using CallType = std::function<void(const DrawInfo&)>;
	std::map<Order,std::map<std::string, CallType>> RenderOrders{};
};  
END

#endif // !__RENDERPROPERTY_H__