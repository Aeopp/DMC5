#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "Object.h"
#include "GameObject.h"
#include <map>
#include "FLight.h"
#include <memory>
#include <vector>
#include "RenderProperty.h"
#include "Frustum.h"
#include "RenderInformation.h"
#include "RenderInterface.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Quad.h"
#include "basiccamera.h"


// TODO :: ����ü �ø� (����Ʈ ����Ʈ�� ) 
// TODO :: ������Ʈ ��� 
BEGIN(ENGINE)
class ENGINE_DLL Renderer final : public Object
{
	DECLARE_SINGLETON(Renderer) 
private : 
	explicit Renderer();
	virtual ~Renderer() = default;
	virtual void Free()	override ;
public :
	HRESULT ReadyRenderSystem(LPDIRECT3DDEVICE9 const _pDevice);
private:
	void    ReadyRenderTargets();
	void	ReadyShader(const std::filesystem::path & TargetPath);
	void    ReadyLights();
	void    ReadyRenderInfo();
	void    ReadyFrustum();
	void    ReadyQuad();	
public : 
	HRESULT Render()&;
	void    Editor()&;
	// ������Ʈ�� ���� ������ �����ִٸ� RenderInterface �������̽��� �˻��ϰ� ��ƼƼ�� �߰� .
	void Push(const std::weak_ptr<GameObject>&_RenderEntity)&;
	const Frustum * GetCameraFrustum()const& { return CameraFrustum.get();  };
private:
	void RenderReady()&;
	void RenderBegin()&;
	void RenderReadyEntitys()&;
	void RenderEnd()&;
	void RenderEntityClear()&;
private:
	void ResetState()&;
private:
	HRESULT RenderDebug()&;
	HRESULT RenderDebugBone()&;
	HRESULT ImguiRender()&;
	HRESULT RenderTargetDebugRender()&;
	HRESULT RenderSky()&;
	HRESULT Tonemapping()&;
	HRESULT AlphaBlendEffectRender()&;
	HRESULT UIRender()&;
	HRESULT RendererCollider()&;
public:
	float ShadowMin = 0.0f;
	bool bEdit = false;
	RenderInformation _RenderInfo{};
	RenderInformation _PrevRenderInfo{};
private:
	IDirect3DSurface9* BackBuffer{ nullptr };
	std::shared_ptr<Frustum> CameraFrustum{};
	std::shared_ptr<Frustum> CurShadowFrustum{};
	LPDIRECT3DDEVICE9	Device{ nullptr };
	// �����н��� ���̴� Ű 
	// ���̴� Ű�� �ش� ���̴��� ȣ���Լ�,��ü��
	using RenderEntityType = std::pair< RenderInterface*, RenderProperty::CallType>;
	std::map<RenderProperty::Order, 
		std::unordered_map<std::string,std::vector<RenderEntityType>>>
		RenderEntitys{};
	std::shared_ptr<Quad> _Quad;
	std::map<std::string, std::shared_ptr<ENGINE::Shader>> Shaders{};
	std::map<std::string, std::shared_ptr<RenderTarget>>   RenderTargets{};
	std::vector< std::shared_ptr<FLight> > DirLights{};
	std::vector<std::shared_ptr<FLight>> PointLights{};

	// ���̴� �׽�Ʈ ���� ....
	bool TestShaderInit();
	void TestShaderRelease();
	void TestLightRotation();
	void RenderShadowMaps();
	void RenderGBuffer();
	void DeferredShading();
	void RenderScene(LPD3DXEFFECT effect, const D3DXMATRIX & viewproj);
	
	LPDIRECT3DTEXTURE9	marble = nullptr;
	LPDIRECT3DTEXTURE9	wood = nullptr;
	LPDIRECT3DTEXTURE9	wood_normal = nullptr;
	LPDIRECT3DTEXTURE9	sky = nullptr;
	LPD3DXMESH			skull = nullptr;
	LPD3DXMESH			box = nullptr;

	Vector4 MoonLightTarget{0,0,0,1};
	Vector4 CurstomEye= { 0,0,0,1 };
	float DXScreenQuadVerticesFFP[24] = {
		// NOTE: viewport must be added
		-0.5f, -0.5f, 0, 1,		0, 1,
		-0.5f, -0.5f, 0, 1,		0, 0,
		-0.5f, -0.5f, 0, 1,		1, 1
		- 0.5f, -0.5f, 0, 1,		1, 0,
	};
};
END

#endif // _RENDERER_H_