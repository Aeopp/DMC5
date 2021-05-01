#include "stdafx.h"
#include "Em200.h"
#include "Em200Hand.h"

void Em200Hand::Free()
{
	Unit::Free();
}

Em200Hand* Em200Hand::Create()
{
	return new Em200Hand;
}

std::string Em200Hand::GetName()
{
	return std::string();
}


HRESULT Em200Hand::Ready()
{
	Unit::Ready();
	RenderInit();

	m_BattleInfo.iAttack = 5;

	m_nTag = MonsterWeapon;
	

	return S_OK;
}

HRESULT Em200Hand::Awake()
{
	Unit::Awake();


	m_pParentBone = m_pEm200Mesh.lock()->GetToRootMatrixPtr(m_bLeft ? "L_Hand" : "R_Hand");
	m_pEm200Trans = m_pEm200.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	if (m_bLeft)
	{
		m_pCollider.lock()->SetRadius(0.08f);
		m_pCollider.lock()->SetCenter({ 0.04, 0.f, 0.0f });
	}
	else
	{
		m_pCollider.lock()->SetRadius(0.08f);
		m_pCollider.lock()->SetCenter({ -0.04, 0.f, 0.01f });
	}
	return S_OK;
}

HRESULT Em200Hand::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em200Hand::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	m_ParentWorld = m_pEm200Trans.lock()->GetWorldMatrix();
	m_pTransform.lock()->SetWorldMatrix(*m_pParentBone * m_ParentWorld);
	
	return 0;
}

UINT Em200Hand::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em200Hand::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em200Hand::OnEnable()
{
	Unit::OnEnable();
}

void Em200Hand::OnDisable()
{
	Unit::OnDisable();
}

void Em200Hand::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em200Hand::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Em200Hand::RenderInit()
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

void Em200Hand::RenderReady()
{
}

