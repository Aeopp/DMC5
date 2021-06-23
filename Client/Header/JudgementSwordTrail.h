#ifndef __JUDGEMENTSWORDTRAIL_H_
#define __JUDGEMENTSWORDTRAIL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"
#include "FLight.h"
#include "DynamicLight.h"

class JudgementSwordTrail : public ENGINE::GameObject,
							public ENGINE::RenderInterface
{
public:
	enum  Mode : uint32
	{
		Non,
		Judgement
	};
private:
	static constexpr uint32 BoneCnt = 8u;

	Matrix BoneWorld{ FMath::Identity() };
	Matrix PrevBoneWorld{ FMath::Identity() };


	Vector3 CurModeLowOffset()
	{
		return CurMode == Mode::Non ? LowOffset : JudgementLowOffset;
	};

	Vector3 CurModeHighOffset()
	{
		return CurMode == Mode::Non ? HighOffset : JudgementHighOffset;
	};

	Vector3 LowOffset{ 0.f,0.f,125.f };
	Vector3 HighOffset{ 0.f,0.f,-125.f };

	Vector3 JudgementLowOffset{ 0.f,0.f,125.f };
	Vector3 JudgementHighOffset{ 0.f,0.f,-125.f };

	std::array<std::string, BoneCnt> BoneLowNames
	{
		"_000",
		"_000_2",
		"_000_3",
		"_000_4",
		"_000_5",
		"_000_6",
		"_000_7",
		"_000_8"
	};

	std::array<std::string, BoneCnt> BoneHighNames
	{
		"_001",
		"_001_2",
		"_001_3",
		"_001_4",
		"_001_5",
		"_001_6",
		"_001_7",
		"_001_8"
	};

	Vector3 CurrentTrailLocation{ 0.f,0.f,0.f };

	std::array< IDirect3DVertexBuffer9*, BoneCnt> VtxBuffers{};
	std::array< IDirect3DIndexBuffer9*, BoneCnt> IdxBuffers{};
	IDirect3DVertexDeclaration9* VtxDecl{ nullptr };
	IDirect3DDevice9* Device{ nullptr };

	std::shared_ptr<Texture> TrailMap{};
	std::shared_ptr<Texture> NoiseMap{};

	uint32  CurMode = static_cast<uint32>(Mode::Non);
	Vector4 _Color{ 1.f,0.f,0.f,1.f };

	float DistortionIntencity = 0.004301f;
	float UV0Multiply = 1.f;
	float CurveT = 0.5f;
	float ColorIntencity = 0.1f;

	float JudgementDayParticleDelta = 0.016f;
	float CurJudgementDayParticleDelta = 0.0f;

	Vector3 Scale{ 2.097902f ,1.398601f,1.398601f };
	Vector3 ScrollSpeed{ 0.5f,0.699301f,1.398601f };

	Vector2 NoiseDistortion0{ 0.896861f,0.446429f };
	Vector2 NoiseDistortion1{ 1.793722f,1.785714f };
	Vector2 NoiseDistortion2{ 4.484305f,4.910714f };

	TrailDesc _Desc{};
	float     T = 0.0f;

	std::array<std::vector<Vertex::Index32>, BoneCnt>     _IdxLog{};
	std::array<std::vector<Vertex::TrailVertex>, BoneCnt> _TrailVtxWorldLocations{};
	std::array<std::pair<Vector3, Vector3>, BoneCnt>       LatelyLocations{};

	DynamicLight _DynamicLight{};
	Vector4 LightColorLow
		{200.f/255.f, 43.f/255.f, 43.f/255.f,1.f };
	Vector4 LightColorHigh
		{182.f, 45.f/255.f, 45.f/255.f,1.f};

	float FluxLow{ 0.15f };
	float FluxHigh{ 0.3f };

	float RadiusLow{ 20.f };
	float RadiusHigh { 20.f };
private:
	explicit JudgementSwordTrail()  ;
	virtual ~JudgementSwordTrail() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static JudgementSwordTrail* Create();
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
	void PlayStart(const Mode _Mode);
	void PlayEnd();
	void ParticleUpdate(const float DeltaTime);
	void PlayParticle();
private:
	void BufferUpdate(const float DeltaTime);
	void VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr);
	void VtxUVCalc(Vertex::TrailVertex* const VtxPtr);
	void VertexBufUpdate();
	void IndexBufUpdate();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderTrail(const DrawInfo& _Info);
	Vector3 GetTrailLocation();
};
#endif //
