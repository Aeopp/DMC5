#ifndef __CBSLONGTRAIL_H_
#define __CBSLONGTRAIL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"

class CbsLongTrail :  public ENGINE::GameObject,
					  public ENGINE::RenderInterface
{
private:
	IDirect3DVertexBuffer9* VtxBuffer{ nullptr };
	IDirect3DIndexBuffer9* IdxBuffer{ nullptr };
	IDirect3DVertexDeclaration9* VtxDecl{ nullptr };
	IDirect3DDevice9* Device{ nullptr };

	std::shared_ptr<ENGINE::Texture> TrailMap{};
	std::shared_ptr<ENGINE::Texture> ExplosionTrailMap{};
	std::shared_ptr<ENGINE::Texture> FireSpriteMap{};
	std::shared_ptr<ENGINE::Texture> EmissiveMskMap{};
	std::shared_ptr<ENGINE::Texture> NoiseMap{};

	Vector3 ScrollSpeed{ 1.f,1.f,1.f };
	Vector3 Scale{ 1.f,1.f,1.f };

	float SpriteRow;
	float SpriteCol;
	float SpriteRowIdx = 0.f;
	float SpriteColIdx = 0.f;

	float SpriteCurUpdateCycle = 0.0f;
	float SpriteUpdateCycle = 0.016f;

	Vector3 Offset{ 0.f,0.f,-430.f };
	Vector2 NoiseDistortion0{ 0.3f,1.f };
	Vector2 NoiseDistortion1{ 0.5f,0.77f };
	Vector2 NoiseDistortion2{ 0.77f,0.5f };
	Vector4 _Color{ 1.f,1.f,1.f,1.f / 255.f };

	float NonDistortionIntencity = 1.f;
	float DistortionIntencity = 10000.f;
	float UV0Multiply = 1.f;
	float CurveT = 0.5f;
	float ColorIntencity = 40.f;
	float EmissiveIntencity = 0.0f;
	
	TrailDesc _Desc{};
	float     T = 0.0f;

	std::vector<Vertex::Index32> _IdxLog{};
	std::vector<Vertex::TrailVertex> _VtxLog{};
private:
	explicit CbsLongTrail();
	virtual ~CbsLongTrail() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static CbsLongTrail* Create();
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
	void PlayStart();
	void PlayEnd();
private:
	void SpriteUpdate(const float DeltaTime);
	void BufferUpdate(const float DeltaTime);
	void ParticleUpdate(const float DeltaTime);
	void VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr);
	void VtxUVCalc(Vertex::TrailVertex* const VtxPtr);
	void VertexBufUpdate();
	void IndexBufUpdate();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderTrail(const DrawInfo& _Info);
private:
	Vector3 BoneWorldLocation{0,0,0};
	float ParticleDelta = 0.0016f;
	float CurParticleDelta = 0.0f;
	// first = Low second = High ;
	std::pair<Vector3, Vector3 >LatelyLocations{};
};
#endif //
