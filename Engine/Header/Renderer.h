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

BEGIN(ENGINE)
class ENGINE_DLL Renderer final : public Object
{
	DECLARE_SINGLETON(Renderer)
public:
	struct FadeEffect
	{
		enum class Mode : uint8
		{
			Out,
			In,
			OutAfterIn,
			None
		};
		FadeEffect::Mode CurMode{FadeEffect::Mode::None};
		float T = 0.0f;
		float Time = 0.0f;
		bool bEnable = false;
		float GetLerp()const& ;
	};
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
	void    ReadyPtLightPool();
	void    RenderFadeEffect();
public:
	HRESULT Render()&;
	HRESULT Update(const float DeltaTime)&;
	HRESULT OptRender()&;
	void    Editor()&;

	// ������Ʈ�� ���� ������ �����ִٸ� RenderInterface �������̽��� �˻��ϰ� ��ƼƼ�� �߰� .
	void Push(const std::weak_ptr<GameObject>&_RenderEntity)&;
	const Frustum* GetCameraFrustum()const& { return CameraFrustum.get(); };
	bool bLightRender = false;
	bool bEnvironmentRender = false;
	void LightSave(std::filesystem::path path);
	void LightLoad(const std::filesystem::path & path);
	const std::vector< std::shared_ptr<FLight> >& GetDirLights() { return DirLights; };

	std::weak_ptr<FLight> RefRemainingDynamicLight();
	std::weak_ptr<Quad> GetQuad();
	void RequestShadowMapBake();
	bool IsBlackOut()const&;
public:
	std::weak_ptr<FLight> RefPointLights(const uint32 Idx);

	void SkyDistortionStart(
		const float NoiseWrap = 8.173007f,
		const float TimeCorr= 0.304011f);
	void SkyDistortionEnd();
	// ������ ������ ���������� ��ȭ. (���������� �ǰ� �������� ���� )
	void FadeOutStart(const float Time);
	// ������ ���������� ������ ������ ������
	void FadeInStart(const float Time);
	// ���̵�ƿ� -> ���̵� �� ��Ʈ 
	void FadeOutAfterIn(const float Time);

	void LateSceneInit();
	void SceneChangeRender();
	void SetSkyOriginColor(const Vector4& SkyOriginColor);
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
	void ClearVelocityBlur()&;
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
	HRESULT BlendVelocityBlur();
private:
	void EnableDepthBias()&;
	void DisableDepthBias()&;
public:
	bool bEdit = false;
	RenderInformation _RenderInfo{};
	RenderInformation _PrevRenderInfo{};
public:
	std::optional<float> FixedExposure{ std::nullopt };
	std::shared_ptr<Texture> CurSkysphereTex{};
	std::shared_ptr<Texture> SkyTexMission02Sun{};
	std::shared_ptr<Texture> SkyTexMission02Sunset{};
	std::shared_ptr<Texture> SkyTexMission03{};

	std::map<std::string, std::shared_ptr<ENGINE::Shader>> Shaders{};
	std::map<std::string, std::shared_ptr<RenderTarget>>   RenderTargets{};
	

	FLight* CurDirLight{ nullptr };

	bool    bDistortion = true;
	bool    bVelocityBlur = true;
	int     VelocityBlurSamples = 32;
	float   DistortionIntencity = 0.05f;
	float   VelocityBlurIntencity = 1.f;
	float	BlurLengthMin = 0.000001f;
	float   exposure = 1.f;
	float   SoftParticleDepthScale = 0.0f;
	float   ao = 0.010f;
	float   StarScale = 4.f;
	float   StarFactor = 0.9f;

	bool bShadowMapBake = false;
	
	
	Vector4 DistortionColor { 246.f/255.f,10.f/255.f,10.f/255.f,1.f };
	float   SkyDistortionIntencity = 1.f;
	float   SkyNoisewrap = 8.173007f;
	float   SkyTimecorr  = 0.304011f;
	bool    SkyDistortion = false;
	float   SkyIntencity = 0.111f;
	float   SkyRotationSpeed = 0.001f;	
	Vector4 SkyOriginColor{ 1.f,1.f,1.f,1.f };
	float   SkysphereScale = 0.078f;
	Vector3 SkysphereRot{ 0.f,0.f ,0.f};
	Vector3 SkysphereLoc{ 0.f,-4.762f,0.f };
private:
	bool bStars = true;
	FadeEffect _FadeEffect{};
	bool bPtLightScrRtTest = false;
	Vector3 FogColor{ 0.5f,0.5f,0.5f };
	float FogStart = 1.f;
	float FogDistance = 100.f;
	float FogDensity = 1.f;
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
	// 0 ���� ���õǸ� ������ ���� . 
	float averageluminance = 0.1f;
	// 0 ���� ���õǸ� ������ ���� . 
	float adaptedluminance = 0.1f;

	IDirect3DSurface9* BackBuffer{ nullptr };
	IDirect3DSurface9* BackBufferZBuffer{ nullptr };

	std::shared_ptr<Frustum> CameraFrustum{};
	std::shared_ptr<Frustum> CurShadowFrustum{};
	LPDIRECT3DDEVICE9	Device{ nullptr };
	// �����н��� ���̴� Ű 
	// ���̴� Ű�� �ش� ���̴��� ȣ���Լ�,��ü��
	using RenderEntityType = std::pair< RenderInterface*, RenderProperty::CallType>;
	std::map<RenderProperty::Order,
		std::unordered_map<std::string,
		std::vector<RenderEntityType>>>
		RenderEntitys{};

	std::shared_ptr<Texture> DistortionTex{};
	std::set<RenderInterface* > RenderEntitySet{};
	std::shared_ptr<Quad> _Quad;

	std::vector< std::shared_ptr<FLight> > DirLights{};
	std::vector<std::shared_ptr<FLight>> PointLights{};
	std::vector<std::shared_ptr<FLight>> DynamicPointLights{};
	std::shared_ptr<Texture> SkyNoiseMap{};
	std::shared_ptr<StaticMesh> SkysphereMesh{};
	std::shared_ptr<Texture> sky{};


	// ���̴� �׽�Ʈ ���� ....
	bool TestShaderInit();
	void TestShaderRelease();
	void TestLightRotation();
	void RenderShadowMaps();

	void RenderGBuffer();
	void DeferredShading();
	void ShadowCacheBake();

	LPD3DXMESH				skull = nullptr;
	LPD3DXMESH skymesh = nullptr;
	LPDIRECT3DCUBETEXTURE9	environment = nullptr;		
	LPDIRECT3DCUBETEXTURE9	irradiance1 = nullptr;
	LPDIRECT3DCUBETEXTURE9	irradiance2 = nullptr;		
	LPDIRECT3DTEXTURE9		brdfLUT = nullptr;		

	float DXScreenQuadVerticesFFP[24] = {
		// NOTE: viewport must be added
		-0.5f, -0.5f, 0, 1,		0, 1,
		-0.5f, -0.5f, 0, 1,		0, 0,
		-0.5f, -0.5f, 0, 1,		1, 1
		- 0.5f, -0.5f, 0, 1,		1, 0,
	};
public:
	static inline Vector4 ColliderRenderDefaultColor { 255.f / 255.f,240.f / 255.f,140.f / 255.f,0.1f };
};
END

#endif // _RENDERER_H_