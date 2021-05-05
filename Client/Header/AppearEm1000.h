#ifndef __EFF_APPEAR_EM1000_H__
#define __EFF_APPEAR_EM1000_H__
#include "Effect.h"

class AppearEm1000 final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _DebrisMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _DebrisALBMTex{};
	std::shared_ptr<ENGINE::Texture> _DebrisNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _DustMultiTex{};
	std::shared_ptr<ENGINE::Texture> _DustSpurtTex{};

	enum { MAX_SUBSET_IDX = 19u };
	float _DebrisSubsetIdx = -1.f;	// _DebrisMesh
	Matrix _DebrisChildWorldMatrix = Matrix();

	//enum { MAX_SPRITE_IDX = 32u };
	float _DustSpriteIdx = 0.f;
	Matrix _DustSpriteChildWorldMatrix = Matrix();
	Vector2 _DustMinTexUV = Vector2(0.f, 0.f);
	Vector2 _DustMaxTexUV = Vector2(1.f, 1.f);
	bool _DustNextSpriteStart = false;

	float _SliceAmount = 0.f;	// _DustSpurtTex

private:
	explicit AppearEm1000() = default;
	virtual ~AppearEm1000() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void RenderReady() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static AppearEm1000* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	virtual void Reset() override;
};
#endif // !__EFF_APPEAR_EM1000_H__