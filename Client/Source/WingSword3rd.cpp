#include "stdafx.h"
#include "WingSword3rd.h"

WingSword3rd::WingSword3rd()
{
	m_fRightOffset = 80.f;
	m_fUpOffset = 0.1f;
	m_fRotationZ = -135.f;
}

void WingSword3rd::Free()
{
	WingSword::Free();
}

WingSword3rd* WingSword3rd::Create()
{
	return new WingSword3rd;
}

HRESULT WingSword3rd::Ready()
{
	WingSword::Ready();

	RenderInit();

	m_pTransform.lock()->SetScale({ 0.013f,0.013f,0.013f });

	PushEditEntity(m_pTransform.lock().get());
	//SetActive(false);
	return S_OK;
}

HRESULT WingSword3rd::Awake()
{
	WingSword::Awake();
	return S_OK;
}

HRESULT WingSword3rd::Start()
{
	WingSword::Start();
	return S_OK;
}

UINT WingSword3rd::Update(const float _fDeltaTime)
{
	WingSword::Update(_fDeltaTime);
	return 0;
}

UINT WingSword3rd::LateUpdate(const float _fDeltaTime)
{
	WingSword::LateUpdate(_fDeltaTime);
	return 0;
}

void WingSword3rd::OnEnable()
{
	WingSword::OnEnable();
}

void WingSword3rd::OnDisable()
{
	WingSword::OnDisable();
}

void WingSword3rd::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

std::string WingSword3rd::GetName()
{
	return "WingSword_3rd";
}

void WingSword3rd::Editor()
{
	Unit::Editor();

	if (bEdit)
	{

	}
}

void WingSword3rd::RenderReady()
{
	WingSword::RenderReady();
}

void WingSword3rd::RenderGBufferSK(const DrawInfo& _Info)
{
	WingSword::RenderGBufferSK(_Info);
}

void WingSword3rd::RenderShadowSK(const DrawInfo& _Info)
{
	WingSword::RenderShadowSK(_Info);
}

void WingSword3rd::RenderDebugBone(const DrawInfo& _Info)
{
	WingSword::RenderDebugBone(_Info);
}

void WingSword3rd::RenderDebugSK(const DrawInfo& _Info)
{
	WingSword::RenderDebugSK(_Info);
}

void WingSword3rd::RenderInit()
{	
	WingSword::RenderInit();
	SetRenderEnable(true);


	ENGINE::RenderProperty _InitRenderProp;

	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_dsSK",
		[this](const DrawInfo& _Info)
			{
				RenderGBufferSK(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"ShadowSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadowSK(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::DebugBone]
		=
	{
		{"DebugBone" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugBone(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"DebugSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugSK(_Info);
		}
	} };
	RenderInterface::Initialize(_InitRenderProp);

	Mesh::InitializeInfo _InitInfo{};

	_InitInfo.bLocalVertexLocationsStorage = false;

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Model.fbx", _InitInfo);
	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\3rd");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Model.Animation");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}
