#ifndef __TRAIL_H_
#define __TRAIL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"


class Trail : public ENGINE::GameObject,
			  public ENGINE::RenderInterface
{
public:
	enum class Mode : int32
	{
		Non,
		Explosion,
	};
private:
	IDirect3DVertexBuffer9* VtxBuffer{nullptr};
	IDirect3DIndexBuffer9*  IdxBuffer{ nullptr };
	IDirect3DVertexDeclaration9* VtxDecl{ nullptr };
	IDirect3DDevice9* Device{ nullptr };

	std::shared_ptr<Texture> TrailMap{};
	std::shared_ptr<Texture> ExplosionTrailMap{};
	std::shared_ptr<Texture> FireSpriteMap{};
	std::shared_ptr<Texture> EmissiveMskMap{};
	/*std::shared_ptr<Texture> NoiseMap{};*/

	Mode CurMode = Mode::Explosion;

	float SpriteRow;
	float SpriteCol;
	float SpriteRowIdx = 0.f;
	float SpriteColIdx = 0.f;

	float SpriteCurUpdateCycle = 0.0f;
	float SpriteUpdateCycle = 0.016f;

	Vector3 LowOffset{ 0.f,0.f,143.f};
	Vector3 HighOffset{ 0.f,0.f,-231.f };

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

	std::vector<Vertex::Index32> _IdxLog{};
	std::vector<Vertex::TrailVertex> _VtxLog{};
private:
	explicit Trail()  ;
	virtual ~Trail() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Trail* Create();
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
	void VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr);
	void VtxUVCalc(Vertex::TrailVertex* const VtxPtr);
	void VertexBufUpdate();
	void IndexBufUpdate();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderTrail(const DrawInfo& _Info);
};
#endif //
