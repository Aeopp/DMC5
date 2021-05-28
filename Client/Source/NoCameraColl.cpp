#include "stdafx.h"
#include "NoCameraColl.h"
#include "MainCamera.h"

NoCameraColl::NoCameraColl()
{
	m_nTag = TAG_NoCameraColl;
}

void NoCameraColl::Free()
{
	GameObject::Free();
}

NoCameraColl* NoCameraColl::Create()
{
	return new NoCameraColl;
}

HRESULT NoCameraColl::Ready()
{
	GameObject::Ready();
	RenderInit();
	
	m_pTransform.lock()->SetPosition({ -5.564f,-1.28f,47.5f });
	

	PushEditEntity(m_pTransform.lock().get());

	return S_OK;
}

HRESULT NoCameraColl::Awake()
{
	GameObject::Awake();
	m_pMainCamera = static_pointer_cast<MainCamera>(FindGameObjectWithTag(TAG_Camera).lock());


	m_pCollider = AddComponent<BoxCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetSize({ 2.6f,2.f,2.76f });
	PushEditEntity(m_pCollider.lock().get());
	

	return S_OK;
}

HRESULT NoCameraColl::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT NoCameraColl::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	return 0;
}

UINT NoCameraColl::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void NoCameraColl::OnEnable()
{
}

void NoCameraColl::OnDisable()
{
}

void NoCameraColl::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}
}

std::string NoCameraColl::GetName()
{
	return "NoCameraColl";
}

void NoCameraColl::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	//if (GAMEOBJECTTAG::Player != _pOther.lock()->m_nTag)
	//	return;

	//m_pMainCamera.lock()->SetCollEnable(false);
}

void NoCameraColl::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
	//if (GAMEOBJECTTAG::Player != _pOther.lock()->m_nTag)
	//	return;
	//m_pMainCamera.lock()->SetCollEnable(true);
}


void NoCameraColl::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void NoCameraColl::RenderInit()
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
