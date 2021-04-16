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
	//struct ENGINE_DLL ShaderInfo
	//{
	//public:
	//	void RegistShader(const RenderProperty::Order& RegistOrder,
	///* �Ķ���ʹ� ���̴��� Create�� ����ȭ !*/
	//					  const std::filesystem::path& LoadPath, 
	//					  const std::any& InitParams)&;
	//	std::shared_ptr<ENGINE::Shader> GetShader(const RenderProperty::Order& _RegistOrder)const&;
	//private:
	//	std::map<RenderProperty::Order, std::shared_ptr<ENGINE::Shader>>_Shaders{};
	//};
	struct ENGINE_DLL UpdateInfo
	{
		Matrix World;
		std::vector<Sphere>SubsetCullingSphere{}; 
	};

	void Initialize(
		const RenderProperty& _RenderProp)&;
	const RenderProperty & GetRenderProp()const& { return  _RenderProperty;  };

	/*UpdateInfo GetRenderUpdateInfo()const& 
	{
		return _UpdateInfo; 
	};*/
	/// </summary>
public:
	///<��ü���� �غ��ؾ� �ϴ� �ൿ�� �ϸ� �׷��� �ʴٸ� RenderReadyUpdate ȣ��...>
	virtual void    RenderReady()      PURE;
public:
	// ShaderInfo     _ShaderInfo{};
	UpdateInfo _RenderUpdateInfo{};
	RenderProperty _RenderProperty{};
};
END

#endif // !_RENDERINTERFACE_H_




