#ifndef Monster_h__
#define Monster_h__

#include "GameObject.h"
#include "RenderInterface.h"
#include "SkeletonMesh.h"

class Monster : public ENGINE::GameObject,
				public ENGINE::RenderInterface
{
protected:
	explicit Monster() = default;
	virtual ~Monster() = default;

	virtual void Free() override PURE;
	//에디터 쓰고 싶은 놈들만
	virtual std::string GetName() override PURE;
public:
	virtual void    RenderReady()override;

	virtual void	Fight(const float _fDeltaTime) PURE;
	virtual void	State_Change(const float _fDeltaTime) PURE;
	virtual void	Skill_CoolTime(const float _fDeltaTime) PURE;

	std::shared_ptr<ENGINE::SkeletonMesh> Get_Mesh() { return m_pMesh; }
	

public:
	virtual HRESULT Ready() override PURE;
	virtual HRESULT Awake() override PURE;
	virtual HRESULT Start() override PURE;
	virtual UINT    Update(const float _fDeltaTime) override PURE;
	virtual UINT    LateUpdate(const float _fDeltaTime) override PURE;
	virtual void    Editor()override PURE;
	virtual void	OnEnable() override PURE;
	virtual void    OnDisable() override PURE;


protected:
	virtual void   Rotate(const float _fDeltaTime) PURE;
	virtual void   Update_Angle() PURE;
protected:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh = nullptr;

	bool	m_bIng = false;

	//플레이어 방향 회전하기 용
	bool	m_bInteraction = false;;
	float	m_fRadian = 0.f;
	float	m_fAccuangle = 0.f;
	float	m_fAngleSpeed = 0.f;
	/////////////////////////////
};



#endif // Monster_h__