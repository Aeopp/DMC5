#ifndef __EFF_BLOOD_H__
#define __EFF_BLOOD_H__
#include "Effect.h"

class Blood final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Blood0Mesh{};
	std::shared_ptr<ENGINE::StaticMesh> _Blood1Mesh{};
	std::shared_ptr<ENGINE::StaticMesh> _Blood2Mesh{};
	std::shared_ptr<ENGINE::StaticMesh> _Blood3Mesh{};
	std::shared_ptr<ENGINE::Texture> _BloodALB0Tex{};
	std::shared_ptr<ENGINE::Texture> _BloodNRMR0Tex{};

	uint32 _SubsetIdx = 0u;	// 애니메이션 프레임 같은 역할
	uint32 _SubsetMaxIdx = 0u;

	uint32 _VariationIdx = 0u;	// 총 베리에이션. 4종

private:
	explicit Blood() = default;
	virtual ~Blood() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void Reset() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderGBuffer(const DrawInfo& _Info);
public:
	static Blood* Create();
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
	void SetVariationIdx(const uint32 Idx);
};
#endif // !__EFF_BLOOD_H__