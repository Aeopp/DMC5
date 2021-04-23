#ifndef __EFF_DUST_H__
#define __EFF_DUST_H__
#include "Effect.h"

class Dust final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _SmokeALB0Tex{};
	Vector2 _SmokeMinTexUV = Vector2(0.f, 0.f);
	Vector2 _SmokeMaxTexUV = Vector2(1.f, 1.f);

private:
	explicit Dust() = default;
	virtual ~Dust() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static Dust* Create();
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
#endif // !__EFF_DUST_H__