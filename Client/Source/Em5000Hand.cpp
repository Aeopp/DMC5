#include "stdafx.h"
#include "Em5000.h"
#include "Em5000Hand.h"

void Em5000Hand::Free()
{
	GameObject::Free();
}

Em5000Hand* Em5000Hand::Create()
{
	return new Em5000Hand;
}

std::string Em5000Hand::GetName()
{
	return std::string();
}

void Em5000Hand::Fight(const float _fDeltaTime)
{
}

void Em5000Hand::State_Change(const float _fDeltaTime)
{
}

void Em5000Hand::Skill_CoolTime(const float _fDeltaTime)
{
}

HRESULT Em5000Hand::Ready()
{
	Unit::Ready();
	RenderInit();

	return S_OK;
}

HRESULT Em5000Hand::Awake()
{
	Unit::Awake();
	m_pParentBone = m_pEm5000Mesh.lock()->GetToRootMatrixPtr(m_bLeft ? "L_Hand" : "R_Hand");
	m_pEm5000Trans = m_pEm5000.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	if (m_bLeft)
	{
		m_pCollider.lock()->SetCenter({ -1.f, 0.f, -1.f });
		m_pCollider.lock()->SetRadius(2.f);
	}
	else
	{
		m_pCollider.lock()->SetCenter({ 1.f, 0.f, -1.f });
		m_pCollider.lock()->SetRadius(2.f);
	}

	return S_OK;
}

HRESULT Em5000Hand::Start()
{
	return S_OK;
}

UINT Em5000Hand::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	m_ParentWorld = m_pEm5000Trans.lock()->GetWorldMatrix();
	m_pTransform.lock()->SetWorldMatrix(*m_pParentBone * m_ParentWorld);
	
	return 0;
}

UINT Em5000Hand::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em5000Hand::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em5000Hand::OnEnable()
{
	Unit::OnDisable();
}

void Em5000Hand::OnDisable()
{
	Unit::OnDisable();
}

void Em5000Hand::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5000Hand::Buster(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5000Hand::Snatch(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5000Hand::RenderInit()
{
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = true;

	//
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };

	RenderInterface::Initialize(_InitRenderProp);
}

void Em5000Hand::Rotate(const float _fDeltaTime)
{
}

void Em5000Hand::Update_Angle()
{
}
