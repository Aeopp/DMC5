#ifndef Gauntlet_h__
#define Gauntlet_h__

#include "Unit.h"
#include "RenderInterface.h"
#include "Renderer.h"
#include "Subset.h"
#include "Nero.h"
class Gauntlet abstract : public Unit,
	public ENGINE::RenderInterface
{
protected:
	explicit Gauntlet();
	virtual ~Gauntlet() = default;
	virtual void Free() PURE;
public:
	virtual HRESULT Ready()								PURE;
	virtual HRESULT Awake()								PURE;
	virtual HRESULT Start()								PURE;
	virtual UINT	Update(const float _fDeltaTime)		PURE;
	virtual	UINT	LateUpdate(const float _fDeltaTime)	PURE;
	virtual void    Editor()                        override;
	virtual void	OnEnable()							PURE;
	virtual void	OnDisable()							PURE;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) PURE;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther)PURE;
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther)PURE;
public:
	virtual void RenderReady() override;
	// ·»´õ¸µ ÇÔ¼ö ...
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

protected:
	std::weak_ptr<Nero>					m_pNero;
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;

	Matrix*					m_pParentMat = nullptr;
};



#endif // Gauntlet_h__