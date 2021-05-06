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
	///<��ü���� �غ��ؾ� �ϴ� �ൿ�� �ϸ� �׷��� �ʴٸ� RenderReadyUpdate ȣ��...>
	virtual void    RenderReady()      PURE;
public:
	UpdateInfo _RenderUpdateInfo{};
	RenderProperty _RenderProperty{};
};
END

#endif // !_RENDERINTERFACE_H_




