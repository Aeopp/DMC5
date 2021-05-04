#ifndef __EFF_STONE_DEBRIS_H__
#define __EFF_STONE_DEBRIS_H__
#include "Effect.h"

class StoneDebris final : public Effect
{
public:
	enum VARIATION : uint32
	{
		REDORB_0 = 0u,
		REDORB_1,
		REDORB_2,
		REDORB_3,
		GREENORB_0,
		GREENORB_1,
		GREENORB_2,
		GREENORB_3,
		MAX_VARIATION_IDX
	};
	void SetVariationIdx(StoneDebris::VARIATION Idx);

	void SetVelocity(const Vector3& v0) { _Velocity0 = v0; }	// _BeginPos.y 0.05f ���� y == 0.075f ��ó�� ����

private:
	StoneDebris::VARIATION _VariationIdx = REDORB_0;

	std::shared_ptr<ENGINE::StaticMesh> _DebrisMesh{};
	std::shared_ptr<ENGINE::Texture> _RedOrbALBMTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _GreenOrbALBMTex{};
	std::shared_ptr<ENGINE::Texture> _GreenOrbNRMRTex{};
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _SmokeTex{};
	std::shared_ptr<ENGINE::Texture> _DustSingleTex{};

	enum { MAX_SUBSET_IDX = 4u };
	uint32 _SubsetIdx = 0u;	// 4������ ������� �ϳ��� ���

	Matrix _SmokeChildWorldMatrix = Matrix();
	float _SmokeSpriteIdx = 0.f;
	Vector2 _SmokeMinTexUV = Vector2(0.f, 0.f);
	Vector2 _SmokeMaxTexUV = Vector2(1.f, 1.f);
	float _SmokeSliceAmount = 0.f;
	Vector3 _SmokeExtraColor = Vector3(1.f, 1.f, 1.f);
	float _SmokeDeltaPosY = 0.f;

	Matrix _DustSingleChildWorldMatrix = Matrix();
	Vector2 _DustSingleMinTexUV = Vector2(0.f, 0.f);
	Vector2 _DustSingleMaxTexUV = Vector2(1.f, 1.f);
	Vector3 _DustSingleVelocity = Vector3(0.f, 0.f, 0.f);
	Vector3 _DustSingleDeltaPos = Vector3(0.f, 0.f, 0.f);

	Vector3 _BeginPos = Vector3(0.f, 0.05f, 0.f);
	Vector3 _Velocity0 = Vector3(0.f, 0.075f, 0.f);

private:
	explicit StoneDebris() = default;
	virtual ~StoneDebris() = default;
	// Effect��(��) ���� ��ӵ�
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
	virtual void SetPosition(const Vector3& Pos) override;
};
#endif // !__EFF_STONE_DEBRIS_H__