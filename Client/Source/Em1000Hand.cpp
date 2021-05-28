#include "stdafx.h"
#include "Em1000.h"
#include "Em1000Hand.h"

void Em1000Hand::Free()
{
	Unit::Free();
}

Em1000Hand* Em1000Hand::Create()
{
	return new Em1000Hand;
}

std::string Em1000Hand::GetName()
{
	return std::string();
}


HRESULT Em1000Hand::Ready()
{
	Unit::Ready();
	RenderInit();

	m_nTag = MonsterWeapon;
	m_BattleInfo.iAttack = 5;
	
	m_nTag = GAMEOBJECTTAG::MonsterWeapon;
	

	return S_OK;
}

HRESULT Em1000Hand::Awake()
{
	Unit::Awake();


	m_pParentBone = m_pEm1000Mesh.lock()->GetToRootMatrixPtr("Needle01");
	m_pEm1000Trans = m_pEm1000.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	m_pCollider.lock()->SetRadius(0.06f);
	return S_OK;
}

HRESULT Em1000Hand::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em1000Hand::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	m_ParentWorld = m_pEm1000Trans.lock()->GetWorldMatrix();
	m_pTransform.lock()->SetWorldMatrix(*m_pParentBone * m_ParentWorld);
	
	return 0;
}

UINT Em1000Hand::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em1000Hand::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em1000Hand::OnEnable()
{
	Unit::OnEnable();
}

void Em1000Hand::OnDisable()
{
	Unit::OnDisable();
}

void Em1000Hand::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em1000Hand::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{

	if (GAMEOBJECTTAG::Player == _pOther.lock()->m_nTag)
	{
		if (m_pEm1000.expired())
			return;

		
	}
}

void Em1000Hand::RenderInit()
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

void Em1000Hand::RenderReady()
{

}

