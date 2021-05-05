#ifndef _RENDERINTERFACE_H_
#define _RENDERINTERFACE_H_
#include "EngineTypedef.h"
#include "EngineDefine.h"
#include "RenderProperty.h"
#include <filesystem>
#include "Shader.h"
#include <map>

BEGIN(ENGINE)
class ENGINE_DLL RenderInterface
{
public:
	struct ENGINE_DLL UpdateInfo
	{
		Matrix World;
		std::vector<Sphere>SubsetCullingSphere{}; 
	};

	void Initialize(
		const RenderProperty& _RenderProp)&;
	const RenderProperty & GetRenderProp()const& { return  _RenderProperty;  };
public:
	///<객체마다 준비해야 하는 행동을 하며 그렇지 않다면 RenderReadyUpdate 호출...>
	virtual void    RenderReady()      PURE;
public:
	UpdateInfo _RenderUpdateInfo{};
	RenderProperty _RenderProperty{};
};
END

#endif // !_RENDERINTERFACE_H_




