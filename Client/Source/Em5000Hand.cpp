#include "stdafx.h"
#include "Em5000.h"
#include "Em5000Hand.h"
#include "ShockWave.h"
#include "Smoke.h"

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


HRESULT Em5000Hand::Ready()
{
	Unit::Ready();
	RenderInit();

	m_BattleInfo.iAttack = 25;

	return S_OK;
}

HRESULT Em5000Hand::Awake()
{
	Unit::Awake();


	m_nTag = MonsterWeapon;
	m_pParentBone = m_pEm5000Mesh.lock()->GetToRootMatrixPtr(m_bLeft ? "L_Hand" : "R_Hand");
	m_pEm5000Trans = m_pEm5000.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	if (m_bLeft)
	{
		m_pCollider.lock()->SetCenter({ 0.f, 0.f, 0.f });
		m_pCollider.lock()->SetRadius(0.2f);
	}
	else
	{
		m_pCollider.lock()->SetCenter({ 0.f, 0.f, 0.f });
		m_pCollider.lock()->SetRadius(0.2f);
	}

	m_pWave = AddGameObject<ShockWave>();

	for (int i = 0; i < 2; ++i)
		m_pSmoke[i] = AddGameObject<Smoke>();

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

void Em5000Hand::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Em5000Hand::RenderReady()
{
}



void Em5000Hand::RenderInit()
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


