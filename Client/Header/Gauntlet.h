#ifndef Gauntlet_h__
#define Gauntlet_h__

#include "GameObject.h"
#include "RenderInterface.h"
class Nero;
class Gauntlet abstract : public GameObject,
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
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther)PURE;
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther) PURE;
};



#endif // Gauntlet_h__