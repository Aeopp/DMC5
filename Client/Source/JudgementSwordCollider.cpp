#include "stdafx.h"
#include "JudgementSwordCollider.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"
JudgementSwordCollider::JudgementSwordCollider()
	:m_pParentBoneMat(nullptr)
{
	m_BattleInfo.eAttackType = ATTACKTYPE::Attack_Front;
	m_BattleInfo.iAttack = 1;
}
void JudgementSwordCollider::Free()
{
	GameObject::Free();
}

JudgementSwordCollider* JudgementSwordCollider::Create()
{
	return new JudgementSwordCollider;
}

HRESULT JudgementSwordCollider::Ready()
{
	GameObject::Ready();

	m_nTag = TAG_JudgementSwordCollider;
	RenderInit();

	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);

	return S_OK;
}

HRESULT JudgementSwordCollider::Awake()
{
	GameObject::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetTrigger(true);

	m_pCollider.lock()->SetRadius(0.05f);
	m_pCollider.lock()->SetHeight(0.11f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.0f, 0.f });

	m_pCollider.lock()->SetContactOffset(2.f);

	m_pCollider.lock()->SetActive(false);

	PushEditEntity(m_pCollider.lock().get());

	return S_OK;
}

HRESULT JudgementSwordCollider::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT JudgementSwordCollider::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	return 0;
}

UINT JudgementSwordCollider::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	Matrix								ParentWorldMatrix, FinalWorld;

	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

	if (nullptr != m_pParentBoneMat)
	{
		FinalWorld = *m_pParentBoneMat * ParentWorldMatrix;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}

	return 0;
}

void JudgementSwordCollider::OnEnable()
{
	GameObject::OnEnable();
	_RenderProperty.bRender = true;
}

void JudgementSwordCollider::OnDisable()
{
	GameObject::OnDisable();
	_RenderProperty.bRender = false;
}

void JudgementSwordCollider::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void JudgementSwordCollider::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}
}

std::string JudgementSwordCollider::GetName()
{
	return "JudgementSwordCollider";
}

void JudgementSwordCollider::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void JudgementSwordCollider::RenderGBufferSK(const DrawInfo& _Info)
{
}

void JudgementSwordCollider::RenderShadowSK(const DrawInfo& _Info)
{
}

void JudgementSwordCollider::RenderDebugBone(const DrawInfo& _Info)
{
}

void JudgementSwordCollider::RenderDebugSK(const DrawInfo& _Info)
{
}

void JudgementSwordCollider::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
}

void JudgementSwordCollider::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
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
