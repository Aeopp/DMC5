#include "stdafx.h"
#include "Unit.h"

Unit::Unit()
	:m_bCollEnable(false)
{
	ZeroMemory(&m_BattleInfo, sizeof(BT_INFO));
}

void Unit::Free()
{
	GameObject::Free();
}

HRESULT Unit::Ready()
{
	GameObject::Ready();
	return S_OK;
}

HRESULT Unit::Awake()
{
	GameObject::Awake();
	return S_OK;
}

HRESULT Unit::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT Unit::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	return 0;
}

UINT Unit::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void Unit::Editor()
{
	GameObject::Editor();
}

void Unit::OnEnable()
{
	GameObject::OnEnable();
}

void Unit::OnDisable()
{
	GameObject::OnDisable();
}

void Unit::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

BT_INFO Unit::Get_BattleInfo()
{
	return m_BattleInfo;
}

void Unit::Set_Hp()
{
	m_BattleInfo.iHp = 0;
}

bool Unit::Get_Coll()
{
	return m_bCollEnable;
}

void Unit::Set_Coll(const bool _bColl)
{
	m_bCollEnable = _bColl;
}

void Unit::Set_AttackType(ATTACKTYPE _Type)
{
	m_BattleInfo.eAttackType = _Type;
}
