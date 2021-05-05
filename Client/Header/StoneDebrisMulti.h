#ifndef __EFF_STONE_DEBRIS_MULTI_H__
#define __EFF_STONE_DEBRIS_MULTI_H__
#include "Effect.h"

class StoneDebrisMulti final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _DebrisMesh{};
	std::shared_ptr<ENGINE::Texture> _DebrisALBMTex{};
	std::shared_ptr<ENGINE::Texture> _DebrisNRMRTex{};

	enum { MAX_SUBSET_IDX = 19u };
	uint32 _SubsetIdx = 0u;	// 애니메이션 프레임 같은 역할

private:
	explicit StoneDebrisMulti() = default;
	virtual ~StoneDebrisMulti() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void RenderReady() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderGBuffer(const DrawInfo& _Info);
public:
	static StoneDebrisMulti* Create();
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
#endif // !__EFF_STONE_DEBRIS_MULTI_H__