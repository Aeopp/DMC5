#ifndef __COMPONENT_H__
#define __COMPONENT_H__
#include "Object.h"
BEGIN(ENGINE)
class GameObject;
class ENGINE_DLL Component abstract : public Object
{
	friend GameObject;
protected:
	std::weak_ptr<GameObject>	m_pGameObject;
	std::weak_ptr<Component>	m_pThis;
	bool						m_bActive;
protected:
	explicit Component(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~Component() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() PURE;
	virtual void Editor()override;
public:
	std::weak_ptr<GameObject>	GetGameObject();
	bool						IsActive();
	void						SetActive(const bool _bActive);
private:
	void	SetWeakPtr(std::weak_ptr<Component> _pThis);
};
END
#endif // !__COMPONENT_H__