#ifndef __CBSTRAIL_H_
#define __CBSTRAIL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"

struct TrailDesc
{
	uint32 VtxSize{};
	uint32 VtxCnt{};
	uint32 TriCnt{};
	uint32 IdxSize{};
	D3DFORMAT IdxFmt{};

	int32 DrawTriCnt;
	uint32 NewVtxCnt;
	float UpdateCycle;
	float CurVtxUpdateCycle;
};

class CbsTrail : public ENGINE::GameObject,
			     public ENGINE::RenderInterface
{
public:
	enum  Mode : uint32
	{
		Non,
		Ice
	};
private:
	// F -> Low S -> High
	static  inline std::array<std::pair<Vector3,Vector3>,3u> Offset
	{
		// Non
		std::pair<Vector3,Vector3>{ Vector3{0.f,0.f,18.f}, Vector3{ 0.f,0.f,122.f} },
		// Ice
		std::pair<Vector3,Vector3>{ Vector3{0.f,0.f,18.f}, Vector3{ 0.f,0.f,122.f} }
	};
	static constexpr uint32 TrailCnt = 2u;
	static constexpr uint32 BoneCnt = 3u;

	std::array<std::string, BoneCnt > BoneNames{  "pole01","pole02","pole03"};
	std::array< IDirect3DVertexBuffer9*,TrailCnt> VtxBuffers{};
	std::array< IDirect3DIndexBuffer9*,TrailCnt> IdxBuffers{};
	IDirect3DVertexDeclaration9* VtxDecl{ nullptr };
	IDirect3DDevice9* Device{ nullptr };

	std::shared_ptr<Texture> TrailMap{};
	std::shared_ptr<Texture> ExplosionTrailMap{};
	std::shared_ptr<Texture> FireSpriteMap{};
	std::shared_ptr<Texture> EmissiveMskMap{};
	/*std::shared_ptr<Texture> NoiseMap{};*/

	uint32  CurMode = static_cast<uint32>(Mode::Non);

	float SpriteRow;
	float SpriteCol;
	float SpriteRowIdx = 0.f;
	float SpriteColIdx = 0.f;

	float SpriteCurUpdateCycle = 0.0f;
	float SpriteUpdateCycle = 0.000001f;

	
	Vector4 _Color{ 1.f,1.f,1.f,1.f/255.f};

	float NonDistortionIntencity = 1.f;
	float DistortionIntencity = 10000.f;
	float UV0Multiply = 1.f;
	float CurveT = 0.5f;
	float ColorIntencity = 40.f;
	float EmissiveIntencity= 0.0f;

	Vector3 Scale{ 1.f,2.f,3.f };
	Vector3 ScrollSpeed{ 1.f,2.f,3.f };

	TrailDesc _Desc{};
	float     T = 0.0f;

	std::array<std::vector<Vertex::Index32>,TrailCnt> _IdxLog{};
	std::array<std::vector<Vertex::TrailVertex>, TrailCnt> _VtxLog{};

	// Low High
	std::array<std::pair<Vector3, Vector3>, BoneCnt >  LatelyOffsets{};

	float ParticleCycle = 0.1f;
	float CurParticleCycle = 0.0f;
private:
	explicit CbsTrail()  ;
	virtual ~CbsTrail() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static CbsTrail* Create();
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
};
#endif //
