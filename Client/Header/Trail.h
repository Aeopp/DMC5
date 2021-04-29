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



	uint32 NewVtxCnt;
	float UpdateCycle;
	float CurUpdateCycle;
};

class Trail : public ENGINE::GameObject,
			  public ENGINE::RenderInterface
{
private:
	IDirect3DVertexBuffer9* VtxBuffer{nullptr};
	IDirect3DIndexBuffer9*  IdxBuffer{ nullptr };
	IDirect3DVertexDeclaration9* VtxDecl{ nullptr };
	IDirect3DDevice9* Device{ nullptr };
	TrailDesc _Desc{};
	/*uint32 IdxSize;
	D3DFORMAT IdxFmt;*/
	// uint32 FvF{};
	float T = 0.0f;
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
	void PlayStart(const std::optional<Vector3>& Location = std::nullopt);
private:
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderTrail(const DrawInfo& _Info);
};
#endif //