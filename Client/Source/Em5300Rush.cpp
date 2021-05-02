#include "stdafx.h"
#include "Em5300.h"
#include "Em5300Rush.h"

void Em5300Rush::Free()
{
	GameObject::Free();
}

Em5300Rush* Em5300Rush::Create()
{
	return new Em5300Rush;
}

std::string Em5300Rush::GetName()
{
	return std::string();
}


HRESULT Em5300Rush::Ready()
{
	Unit::Ready();
	RenderInit();

	return S_OK;
}

HRESULT Em5300Rush::Awake()
{
	Unit::Awake();
	m_nTag = MonsterWeapon;
	m_pEm5300Trans = m_pEm5300.lock()->GetComponent<ENGINE::Transform>();
	m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("Head");

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	m_pCollider.lock()->SetCenter({ 0.f, 0.f, 0.f });
	m_pCollider.lock()->SetRadius(0.2f);

	return S_OK;
}

HRESULT Em5300Rush::Start()
{
	return S_OK;
}

UINT Em5300Rush::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	m_ParentWorld = m_pEm5300Trans.lock()->GetWorldMatrix();
	m_pTransform.lock()->SetWorldMatrix(*m_pParentBone * m_ParentWorld);
	
	return 0;
}

UINT Em5300Rush::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em5300Rush::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em5300Rush::OnEnable()
{
	Unit::OnDisable();
}

void Em5300Rush::OnDisable()
{
	Unit::OnDisable();
}

void Em5300Rush::RenderReady()
{
}

void Em5300Rush::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5300Rush::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}



void Em5300Rush::RenderInit()
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


