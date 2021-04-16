#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
#include "Object.h"
#include "RenderProperty.h"


BEGIN(ENGINE)
class Scene;
class Component;
class Renderer;
class Transform;
class ENGINE_DLL GameObject abstract : public Object
								
{
	friend Scene;
private:
	typedef std::unordered_map<size_t, std::shared_ptr<Component>> COMPONENTS;
private:
	COMPONENTS	m_Components;
	Scene*		m_pScene;
	bool		m_bActive;
	bool		m_bStatic;
	bool		m_bDestroy;
	//GameObject가 속한 Scene의 LoopPool의 어느 단계에 있는지 보관하는 변수.
	UINT		m_nLoopIdx;
	bool		m_bRenderRegist;
protected:
	std::vector<Object*> m_pEditObjects{};
	std::weak_ptr<GameObject>	m_pGameObject;
	std::weak_ptr<Transform>	m_pTransform;

	//전투 관련된 변수
	BT_INFO		m_BattleInfo;
protected:
	explicit GameObject();
	virtual ~GameObject() = default;
	// Object을(를) 통해 상속됨
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
	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnCollisionStay(std::weak_ptr<GameObject> _pOther);
	virtual void	OnCollisionExit(std::weak_ptr<GameObject> _pOther);

	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
public:
	template <typename TYPE>
	std::weak_ptr<TYPE>	AddComponent();
	template <typename TYPE>
	std::weak_ptr<TYPE>	GetComponent();
	template <typename TYPE>
	std::weak_ptr<TYPE> AddGameObject();

	std::weak_ptr<GameObject>				FindGameObjectWithTag(const UINT & _nTag);
	std::list<std::weak_ptr<GameObject>>	FindGameObjectsWithTag(const UINT & _nTag);
public:
	void	Destroy(std::weak_ptr<GameObject> const _pGameObject);
	bool    IsRenderEnable();
	bool	IsActive();
	void    SetRenderEnable(const bool _bActiveRender)&;
	void	SetActive(const bool _bActive);
	bool	IsStatic();
	void	SetStatic(const bool _bStatic);
	void    PushEditEntity(Object * _EditObject);

	BT_INFO Get_BattleInfo();
private:
	void	SetScene(Scene* const _pScene);
	void	SetGameObject(std::weak_ptr<GameObject> _pGameObject);
	UINT	GetLoopIdx();
	void	SetLoopIdx(const UINT _nLoopIdx);
};
END
#include "GameObject.hpp"
#endif // !__GAMEOBJECT_H__

