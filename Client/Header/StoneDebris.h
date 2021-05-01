#ifndef __EFF_STONE_DEBRIS_H__
#define __EFF_STONE_DEBRIS_H__
#include "Effect.h"

class StoneDebris final : public Effect
{
public:
	enum VARIATION : uint32
	{
		STONE_0 = 0u,
		REDORB_0,
		GREENORB_0,
		MAX_VARIATION_IDX
	};
	void SetVariationIdx(StoneDebris::VARIATION Idx);

private:
	StoneDebris::VARIATION _VariationIdx = STONE_0;

	std::shared_ptr<ENGINE::StaticMesh> _DebrisMesh{};
	std::shared_ptr<ENGINE::Texture> _DebrisALBMTex{};
	std::shared_ptr<ENGINE::Texture> _DebrisNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbALBMTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _GreenOrbALBMTex{};
	std::shared_ptr<ENGINE::Texture> _GreenOrbNRMRTex{};
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _DustSingleTex{};

	enum { MAX_SUBSET_IDX = 4u };
	uint32 _SubsetIdx = 0u;	// 4종류의 서브셋중 하나만 사용

private:
	explicit StoneDebris() = default;
	virtual ~StoneDebris() = default;
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
	static StoneDebris* Create();
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
#endif // !__EFF_STONE_DEBRIS_H__