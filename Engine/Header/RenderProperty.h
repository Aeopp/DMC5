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
};

struct ENGINE_DLL RenderProperty
{
public : 
	enum class Order : uint8
	{
		GBuffer = 0u,

		ForwardAlphaBlend,

		Shadow,

		AlphaBlendEffect,
		UI,

		Debug,

		DebugBone,
	};
	bool bRender = true;
	using CallType = std::function<void(const DrawInfo&)>;
	std::map<Order,std::map<std::string, CallType>> RenderOrders{};
};  

END
#endif // !__RENDERPROPERTY_H__