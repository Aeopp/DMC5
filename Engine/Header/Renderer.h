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

// TODO :: 절두체 컬링 (포인트 라이트도 ) 
// TODO :: 시저렉트 계산 
BEGIN(ENGINE)
class ENGINE_DLL Renderer final : public Object
{
	DECLARE_SINGLETON(Renderer)
private:
	explicit Renderer();
	virtual ~Renderer() = default;
	virtual void Free()	override;
public:
	HRESULT ReadyRenderSystem(LPDIRECT3DDEVICE9 const _pDevice);
private:
	void    ReadyRenderTargets();
	void	ReadyShader(const std::filesystem::path & TargetPath);
	void    ReadySky();
	void    ReadyLights();
	void    ReadyRenderInfo();
	void    ReadyFrustum();
	void    ReadyQuad();
	void    ReadyTextures();
public:
	HRESULT Render()&;
	HRESULT OptRender()&;
	void    Editor()&;
	// 오브젝트의 렌더 세팅이 켜져있다면 RenderInterface 인터페이스를 검사하고 엔티티에 추가 .
	void Push(const std::weak_ptr<GameObject>&_RenderEntity)&;
	const Frustum* GetCameraFrustum()const& { return CameraFrustum.get(); };
	bool bLightRender = false;
	bool bEnvironmentRender = false;
	void LightSave(std::filesystem::path path);
	void LightLoad(const std::filesystem::path & path);
	const std::vector< std::shared_ptr<FLight> >& GetDirLights() { return DirLights; }
private:
	void RenderReady()&;
	void RenderBegin()&;
	void RenderReadyEntitys()&;
	void RenderEnd()&;
	void RenderEntityClear()&;
private:
	void ResetState()&;
private:
	void ClearDistortion()&;
private:
	HRESULT RenderDebug()&;
	HRESULT RenderDebugBone()&;
	HRESULT ImguiRender()&;
	HRESULT RenderTargetDebugRender()&;
	HRESULT RenderSky()&;
	HRESULT RenderSkySphere()&;
	HRESULT RenderEnvironment()&;
	HRESULT Tonemapping()&;
	HRESULT AlphaBlendEffectRender()&;
	HRESULT UIRender()&;
	HRESULT UIRenderAfterPostProcessing()&;
	HRESULT RendererCollider()&;
	HRESULT LightFrustumRender()&;
	HRESULT RenderInsulatorMetal()&;
	HRESULT RenderMeasureLuminance();
	HRESULT AdaptLuminance(const float DeltaTime)&;
	HRESULT BrightPass()&;
	HRESULT DownSample();
	HRESULT Stars();
	HRESULT Bloom();
	HRESULT LensFlare();
	HRESULT ToneMap();
	HRESULT RenderEmissive();
	HRESULT RenderUV();
	HRESULT BlendDistortion();
private:
	void EnableDepthBias()&;
	void DisableDepthBias()&;
public:
	bool bEdit = false;
	RenderInformation _RenderInfo{};
	RenderInformation _PrevRenderInfo{};
public:
	std::shared_ptr<Texture> CurSkysphereTex{};
	std::shared_ptr<Texture> SkyTexMission02Sun{};
	std::shared_ptr<Texture> SkyTexMission02Sunset{};
	bool    bDistortion = false;
	float   DistortionIntencity = 0.05f;
	float   exposure = 1.f;
	float   SoftParticleDepthScale = 0.0f;
	float   ao = 0.010f;
	
	
	Vector4 DistortionColor { 246.f/255.f,10.f/255.f,10.f/255.f,1.f };
	float   SkyDistortionIntencity = 1.f;
	float   SkyNoisewrap = 8.173007f;
	float   SkyTimecorr  = 0.304011f;
	bool    SkyDistortion = true;
	float   SkyRotationSpeed = 0.001f;
	float   SkyIntencity = 0.111f;
	float   SkysphereScale = 0.078f;
	Vector3 SkysphereRot{ 0.f,0.f ,0.f};
	Vector3 SkysphereLoc{ 0.f,-4.762f,0.f };
private:
	
	bool bPtLightScrRtTest = false;
	Vector3 FogColor{ 0.5f,0.5f,0.5f };
	float FogDistance = 100.f;
	float SlpoeScaleDepthBias = 0.0f;
	float DepthBias = 0.0f;
	float DefaultDepthBias = 0.0f;
	
	static const inline std::array <float, 6> def_adaptedluminance_var
	{
		0.98f,
		50.0f,
		100.0f,
		12.5f,
		1.2f ,
		1.0f
	};

	std::array <float, 6> adaptedluminance_var = def_adaptedluminance_var;
	bool bSRGBAlbm = true;
	bool bSRGBNRMR = false;
	bool drawafterimage = false;
	int currentafterimage = 0;
	// 0 으로 세팅되면 미정의 동작 . 
	float averageluminance = 0.1f;
	// 0 으로 세팅되면 미정의 동작 . 
	float adaptedluminance = 0.1f;


	IDirect3DSurface9* BackBuffer{ nullptr };
	IDirect3DSurface9* BackBufferZBuffer{ nullptr };

	std::shared_ptr<Frustum> CameraFrustum{};
	std::shared_ptr<Frustum> CurShadowFrustum{};
	LPDIRECT3DDEVICE9	Device{ nullptr };
	// 렌더패스와 쉐이더 키 
	// 쉐이더 키와 해당 쉐이더의 호출함수,객체들
	using RenderEntityType = std::pair< RenderInterface*, RenderProperty::CallType>;
	std::map<RenderProperty::Order,
		std::unordered_map<std::string,
		std::vector<RenderEntityType>>>
		RenderEntitys{};

	std::shared_ptr<Texture> DistortionTex{};
	std::set<RenderInterface* > RenderEntitySet{};
	std::shared_ptr<Quad> _Quad;
	std::map<std::string, std::shared_ptr<ENGINE::Shader>> Shaders{};
	std::map<std::string, std::shared_ptr<RenderTarget>>   RenderTargets{};
	std::vector< std::shared_ptr<FLight> > DirLights{};
	std::vector<std::shared_ptr<FLight>> PointLights{};

	std::shared_ptr<Texture> SkyNoiseMap{};
	std::shared_ptr<StaticMesh> SkysphereMesh{};
	std::shared_ptr<Texture> sky{};
	

	// 쉐이더 테스트 시작 ....
	bool TestShaderInit();
	void TestShaderRelease();
	void TestLightRotation();
	void RenderShadowMaps();
	void RenderGBuffer();
	void DeferredShading();

	//LPD3DXEFFECT			skyeffect = nullptr;
	//LPD3DXEFFECT			metaleffect = nullptr;
	//LPD3DXEFFECT			insulatoreffect = nullptr;
	//LPD3DXEFFECT			measureeffect = nullptr;
	//LPD3DXEFFECT			hdreffects = nullptr;
	//LPD3DXEFFECT			effect = nullptr;

	LPD3DXMESH				skull = nullptr;
	LPD3DXMESH skymesh = nullptr;
	LPDIRECT3DCUBETEXTURE9	environment = nullptr;		// HDR environment
	LPDIRECT3DCUBETEXTURE9	irradiance1 = nullptr;		// preintegrated diffuse irradiance
	LPDIRECT3DCUBETEXTURE9	irradiance2 = nullptr;		// preintegrated specular irradiance
	LPDIRECT3DTEXTURE9		brdfLUT = nullptr;		// preintegrated BRDF lookup texture

	Vector4 MoonLightTarget{0,0,0,1};

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