#ifndef __EFF_MAKAIBUTTERFLY_H__
#define __EFF_MAKAIBUTTERFLY_H__
#include "Effect.h"

class MakaiButterfly final : public Effect
{
public:
	enum VARIATION : uint32
	{
		STAY = 0u,
		MOVE_FORWARD,

		MAX_VARIATION_IDX
	};
	void SetVariationIdx(MakaiButterfly::VARIATION Idx);

	static void ResetTotalCnt();

private:
	MakaiButterfly::VARIATION _VariationIdx = STAY;

	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> _ALBMTex{};
	std::shared_ptr<ENGINE::Texture> _ALPTex{};
	std::shared_ptr<ENGINE::Texture> _NRMTex{};

	enum { MAX_SUBSET_IDX = 14u };
	float _SubsetIdx = 0.f;	// 애니메이션 프레임 같은 역할

	float _SliceAmount = 1.f;

	Vector3 _BezierStartOffsetPos = Vector3(0.f, 0.f, 0.f);
	Vector3 _BezierEndOffsetPos = Vector3(0.f, 0.f, 0.f);
	Vector3 _BezierDeltaOffsetPos = Vector3(0.f, 0.f, 0.f);

	bool _IsAlive = false;

	weak_ptr<CapsuleCollider> _Collider;
	
	static uint32 _TotalCnt;

	weak_ptr<class SecretVisionMagicCircle>	_SVMC{};

private:
	explicit MakaiButterfly() = default;
	virtual ~MakaiButterfly() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void RenderReady() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderDebug(const DrawInfo& _Info);
public:
	static MakaiButterfly* Create();
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
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
public:
	virtual void PlayStart(const float PlayingSpeed = 25.f) override;
	virtual void Reset() override;
};
#endif // !__EFF_MAKAIBUTTERFLY_H__