#ifndef __EFF_SMOKE_H__
#define __EFF_SMOKE_H__
#include "Effect.h"

class Smoke final : public Effect
{
public:
	enum VARIATION : uint32
	{
		SMOKE_0 = 0u,

		MAX_VARIATION_IDX
	};
	void SetVariationIdx(Smoke::VARIATION Idx);

private:
	Smoke::VARIATION _VariationIdx = SMOKE_0;	// �� �������̼�

	//std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _SmokeMesh{};
	std::shared_ptr<ENGINE::Texture> _SmokeALB0Tex{};
	Vector2 _SmokeMinTexUV = Vector2(0.f, 0.f);
	Vector2 _SmokeMaxTexUV = Vector2(1.f, 1.f);

private:
	explicit Smoke() = default;
	virtual ~Smoke() = default;
	// Effect��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static Smoke* Create();
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
#endif // !__EFF_SMOKE_H__