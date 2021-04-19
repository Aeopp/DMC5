#include "stdafx.h"
#include "Em100.h"
#include "Em100Hand.h"

void Em100Hand::Free()
{
}

Em100Hand* Em100Hand::Create()
{
	return new Em100Hand;
}

std::string Em100Hand::GetName()
{
	return std::string();
}

void Em100Hand::Fight(const float _fDeltaTime)
{
}

void Em100Hand::State_Change(const float _fDeltaTime)
{
}

void Em100Hand::Skill_CoolTime(const float _fDeltaTime)
{
}

HRESULT Em100Hand::Ready()
{
	RenderInit();

	

	return S_OK;
}

HRESULT Em100Hand::Awake()
{
	m_pParentBone = m_pEm100Mesh.lock()->GetToRootMatrixPtr(m_bLeft ? "L_Hand" : "R_Hand");
	m_pEm100Trans = m_pEm100.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	if (m_bLeft)
	{
		m_pCollider.lock()->SetRadius(0.4f);
		m_pCollider.lock()->SetCenter({ 0.4, 0.f, 0.0f });
	}
	else
	{
		m_pCollider.lock()->SetRadius(0.4f);
		m_pCollider.lock()->SetCenter({ -0.4, 0.f, 0.1f });
	}
	return S_OK;
}

HRESULT Em100Hand::Start()
{
	return S_OK;
}

UINT Em100Hand::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	m_ParentWorld = m_pEm100Trans.lock()->GetWorldMatrix();
	m_pTransform.lock()->SetWorldMatrix(*m_pParentBone * m_ParentWorld);
	
	return 0;
}

UINT Em100Hand::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Em100Hand::Editor()
{
	GameObject::Editor();
	if (false == bEdit)
		return;
}

void Em100Hand::OnEnable()
{
}

void Em100Hand::OnDisable()
{
}

void Em100Hand::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (GAMEOBJECTTAG::Player == _pOther.lock()->m_nTag)
	{
		if (m_pEm100.expired())
			return;

		m_pEm100.lock()->OnTriggerEnter(_pOther);
	}
}

void Em100Hand::RenderInit()
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

void Em100Hand::Rotate(const float _fDeltaTime)
{
}

void Em100Hand::Update_Angle()
{
}
