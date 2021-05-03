#ifndef __EFF_SHAPE_PARTICLE_H__
#define __EFF_SHAPE_PARTICLE_H__
#include "Effect.h"

class ShapeParticle final : public Effect
{
public:
	enum SHAPE : uint32
	{
		SPHERE = 0u,
		PIPE00,
		PIPE01,

		MAX_SHAPE_IDX
	};
	void SetShapeIdx(ShapeParticle::SHAPE Idx);

	enum COLOR : uint32
	{
		RED = 0u,
		GREEN,
		WHITE,

		MAX_COLOR_IDX
	};
	void SetColorIdx(ShapeParticle::COLOR Idx);

	enum CONTROLPT : uint32
	{
		ZERO = 0u,	// 로컬의 (0, 0, 0)
		HALF,		// start와 end의 중간지점

		MAX_CONTRLPT_IDX
	};
	void SetCtrlIdx(ShapeParticle::CONTROLPT Idx);

private:
	ShapeParticle::SHAPE _ShapeIdx = SHAPE::PIPE01;
	ShapeParticle::COLOR _ColorIdx = COLOR::RED;
	ShapeParticle::CONTROLPT _CtrlPt = CONTROLPT::ZERO;

	std::vector<shared_ptr<ENGINE::StaticMesh>> _ShapeVec{};

	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _DustSingleTex{};

	float _SliceAmount = 0.f;
	Vector3 _ExtraColor = Vector3(1.f, 1.f, 1.f);

	Matrix _DustSingleChildWorldMatrix = Matrix();
	Vector2 _DustSingleMinTexUV = Vector2(0.f, 0.f);
	Vector2 _DustSingleMaxTexUV = Vector2(1.f, 1.f);

	struct BezierCurveDesc
	{
		Vector3 StartPos;
		Vector3 EndPos;
		Vector3 DeltaPos;
	};
	std::vector<BezierCurveDesc> _BezierCurveDescVec{};

	bool _ChangedShape = true;

private:
	explicit ShapeParticle() = default;
	virtual ~ShapeParticle() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void RenderReady() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static ShapeParticle* Create();
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
#endif // !__EFF_SHAPE_PARTICLE_H__