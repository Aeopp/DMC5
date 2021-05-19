#ifndef __EFF_QLIPHOTH_BLOCK_H__
#define __EFF_QLIPHOTH_BLOCK_H__
#include "Effect.h"

class QliphothBlock final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _BaseMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _BaseInnerMesh{};

	std::shared_ptr<ENGINE::Texture> _BaseInnerTex{};
	std::shared_ptr<ENGINE::Texture> _NoiseTex{};

	float _SliceAmount = 1.f;
	bool _IsAlive = false;

	std::weak_ptr<BoxCollider> _Collider{};

private:
	explicit QliphothBlock() = default;
	virtual ~QliphothBlock() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void RenderReady() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static QliphothBlock* Create();
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
	virtual void	PlayStart(const float PlayingSpeed = 1.f) override;
	virtual void	Reset() override;
};
#endif // !__EFF_QLIPHOTH_BLOCK_H__