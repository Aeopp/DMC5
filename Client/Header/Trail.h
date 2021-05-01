#ifndef __TRAIL_H_
#define __TRAIL_H_
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

class Trail : public ENGINE::GameObject,
			  public ENGINE::RenderInterface
{
private:
	IDirect3DVertexBuffer9* VtxBuffer{nullptr};
	IDirect3DIndexBuffer9*  IdxBuffer{ nullptr };
	IDirect3DVertexDeclaration9* VtxDecl{ nullptr };
	IDirect3DDevice9* Device{ nullptr };

	std::shared_ptr<Texture> TrailMap{};
	std::shared_ptr<Texture> FireSpriteMap{};

	float SpriteRow;
	float SpriteCol;
	float SpriteRowIdx = 0.f;
	float SpriteColIdx = 0.f;

	float SpriteCurUpdateCycle = 0.0f;
	float SpriteUpdateCycle = 0.05f;

	Vector3 LowOffset{ 0.f,0.f,21.f};
	Vector3 HighOffset{ 0.f,0.f,-100.f };

	Vector4 _Color{ 1.f,1.f,1.f,1.f};
	/*std::vector<Vertex::Index32> _IdxLog{};
	std::vector<Vertex::TrailVertex> _VtxLog{};*/
	float DistortionIntencity = 0.01f;
	float UV0Multiply = 1.f;
	float CurveT = 0.5f;
	float ColorIntencity = 1.f; 
	float SpriteUpdateTime = 0.0f;

	TrailDesc _Desc{};
	float     T = 0.0f;
private:
	explicit Trail()  ;
	virtual ~Trail() = default;
	// GameObject��(��) ���� ��ӵ�
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
	void PlayStart(const std::optional<Vector3>& Location = std::nullopt);
	void PlayEnd();
private:
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderTrail(const DrawInfo& _Info);
};
#endif //
