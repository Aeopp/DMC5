#ifndef __CBSMIDTRAIL_H_
#define __CBSMIDTRAIL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"
#include "FLight.h"

class CbsMidTrail : public ENGINE::GameObject,
			        public ENGINE::RenderInterface
{
public:
	struct EffectDesc
	{
		Vector3 LocationOffset{ 0.0f,0.0f,0.0f };
		float T = 0.0f;
		float PlayTime = 0.0f;
		bool  bPlayEnd = true;
		GAMEOBJECTTAG _Tag{};
		std::weak_ptr<class GameObject> _Effect{};
	};

	static constexpr int32 EffectParticleCount = 1000;

	std::array<CbsMidTrail::EffectDesc,EffectParticleCount> _PlayEffectDescs{};

	enum  Mode : uint32
	{
		Non,
	};
private:
	// F -> Low S -> High
	static  inline std::array<std::pair<Vector3,Vector3>,3u> Offset
	{
		// Non
		std::pair<Vector3,Vector3>{ Vector3{0.f,0.f,18.f}, Vector3{ 0.f,0.f,91.5f} },
		// IceAge
		std::pair<Vector3,Vector3>{ Vector3{0.f,0.f,18.f}, Vector3{ 0.f,0.f,91.5f} }
	};

	static constexpr uint32 BoneCnt = 3u;

	std::array<std::string, BoneCnt > BoneNames{"pole01","pole02","pole03"};
	Vector3 CurrentTrailLocation = { 0.f,0.f,0.f };
	std::array<Vector3, BoneCnt > BoneWorldLocationMap{};

	std::array< IDirect3DVertexBuffer9*, BoneCnt> VtxBuffers{};
	std::array< IDirect3DIndexBuffer9*, BoneCnt> IdxBuffers{};
	IDirect3DVertexDeclaration9* VtxDecl{ nullptr };
	IDirect3DDevice9* Device{ nullptr };

	std::shared_ptr<Texture> TrailMap{};
	std::shared_ptr<Texture> IceTrailMap{};
	std::shared_ptr<Texture> IceMap{};
	std::shared_ptr<Texture> EmissiveMskMap{};
	std::shared_ptr<Texture> NoiseMap{};

	uint32  CurMode = static_cast<uint32>(Mode::Non);
	Vector4 _Color{ 1.f,1.f,1.f,1.f/255.f};

	float NonDistortionIntencity = 1.f;
	float DistortionIntencity = 10000.f;
	float UV0Multiply = 1.f;
	float CurveT = 0.5f;
	float ColorIntencity = 0.5f;
	float EmissiveIntencity = 1.f;

	Vector3 Scale{ 48.f,49.f,80.f};
	Vector3 ScrollSpeed{ 15.f,28.f,20.f};

	Vector2 NoiseDistortion0{ 78.f,50.f};
	Vector2 NoiseDistortion1{ 22.5f,100.f};
	Vector2 NoiseDistortion2{ 55.f,10.f};

	float EffectEulerScale = 3.14f;
	float EffectLocationOffsetScale = 0.005f;

	TrailDesc _Desc{};
	float     T = 0.0f;

	std::array<std::vector<Vertex::Index32>, BoneCnt> _IdxLog{};
	std::array<std::vector<Vertex::TrailVertex>, BoneCnt> _TrailVtxWorldLocations{};

	// Low High
	std::array<std::pair<Vector3, Vector3>,BoneCnt>  LatelyOffsets{};

	float ParticleCycle = 0.35f;
	float CurParticleCycle = 0.0f;

	int32  CurEffectParticleIdx = 0;
	float EffectParticleCycle = 0.0007f;
	float CurEffectParticleCycle = 0.0f;
private:
	explicit CbsMidTrail()  ;
	virtual ~CbsMidTrail() = default;
	// GameObject??(??) ???? ??????
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static CbsMidTrail* Create();
public:
	virtual void    RenderReady()                          override;
public:
	void			RenderInit();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void PlayStart(
				const Mode _Mode ,
				const std::optional<Vector3>& Location = std::nullopt);
	void PlayEnd();
	std::optional<Vector3 > GetBoneWorldLocation(const uint32 BoneIdx);
private:
	void BufferUpdate(const float DeltaTime);
	void ParticleUpdate(const float DeltaTime);
	void EffectParticleUpdate(const float DeltaTime);
	void VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr);
	void VtxUVCalc(Vertex::TrailVertex* const VtxPtr);
	void VertexBufUpdate();
	void IndexBufUpdate();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderTrail(const DrawInfo& _Info);
	Vector3 GetTrailLocation();
private:
	Vector2 EffectLifeTimeRange{0.15f,0.25f};
	std::map<GAMEOBJECTTAG, float> EffectScale
	{
		{Eff_ElectricBranch,0.f},
		{Eff_ElectricOccur,0.f},
		{Eff_ElectricVortex,0.011f},
		{Eff_ThunderBolt,0.0012f},
		{Eff_ThunderBoltSecond,0.f}
	};
	float EffectSubsetDelta = 0.1f;
	std::pair<float, float > ThunderBoltVeloictyScale{ 0.0f,0.1f};
	float PtLightRadius = 1.f;
	float PtLightFlux = 0.05f;
	D3DXCOLOR PtLightColor{ 177.f/255.f,146.f/255.f,232.f/255.f,1.f };
	std::weak_ptr<class FLight> PtLight{};
};
#endif //
