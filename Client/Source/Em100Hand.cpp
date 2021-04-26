#include "stdafx.h"
#include "Em100.h"
#include "Em100Hand.h"

void Em100Hand::Free()
{
	Unit::Free();
}

Em100Hand* Em100Hand::Create()
{
	return new Em100Hand;
}

std::string Em100Hand::GetName()
{
	return std::string();
}


HRESULT Em100Hand::Ready()
{
	Unit::Ready();
	RenderInit();

	m_BattleInfo.iAttack = 5;

	m_nTag = MonsterWeapon;
	

	return S_OK;
}

HRESULT Em100Hand::Awake()
{
	Unit::Awake();


	m_pParentBone = m_pEm100Mesh.lock()->GetToRootMatrixPtr(m_bLeft ? "L_Hand" : "R_Hand");
	m_pEm100Trans = m_pEm100.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	if (m_bLeft)
	{
		m_pCollider.lock()->SetRadius(0.04f);
		m_pCollider.lock()->SetCenter({ 0.04, 0.f, 0.0f });
	}
	else
	{
		m_pCollider.lock()->SetRadius(0.04f);
		m_pCollider.lock()->SetCenter({ -0.04, 0.f, 0.01f });
	}
	return S_OK;
}

HRESULT Em100Hand::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em100Hand::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	m_ParentWorld = m_pEm100Trans.lock()->GetWorldMatrix();
	m_pTransform.lock()->SetWorldMatrix(*m_pParentBone * m_ParentWorld);
	
	return 0;
}

UINT Em100Hand::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em100Hand::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em100Hand::OnEnable()
{
	Unit::OnEnable();
}

void Em100Hand::OnDisable()
{
	Unit::OnDisable();
}

void Em100Hand::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em100Hand::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (GAMEOBJECTTAG::Player == _pOther.lock()->m_nTag)
		m_bCollEnable = false;
}

void Em100Hand::RenderInit()
{
	SetRenderEnable(true);

	// 렌더 속성 전체 초기화 
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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

void Em100Hand::RenderReady()
{
}

