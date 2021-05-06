#ifndef WingSword_h__
#define WingSword_h__

#include "Unit.h"
#include "RenderInterface.h"
#include "Renderer.h"
#include "Subset.h"
#include "Nero.h"
class WingSword abstract : public Unit,
	public ENGINE::RenderInterface
{
protected:
	explicit WingSword();
	virtual ~WingSword() = default;
	virtual void Free() PURE;
public:
	virtual HRESULT Ready()								PURE;
	virtual HRESULT Awake()								PURE;
	virtual HRESULT Start()								PURE;
	virtual UINT	Update(const float _fDeltaTime)		PURE;
	virtual	UINT	LateUpdate(const float _fDeltaTime)	PURE;
	virtual void	OnEnable()							PURE;
	virtual void	OnDisable()							PURE;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) PURE;

public:
	virtual void RenderReady() override;
	// ·»´õ¸µ ÇÔ¼ö ...
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

public:
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
	void StopAnimation();
	void ContinueAnimation();
protected:
	std::weak_ptr<Nero>					m_pNero;
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;

	Matrix* m_pParentMat = nullptr;

	float m_fLookOffset = 0.f;
	float m_fRightOffset = 0.f;
	float m_fUpOffset = 0.f;
	float m_fRotationZ = 0.f;
};



#endif // WingSword_h__