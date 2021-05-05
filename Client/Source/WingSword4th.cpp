#include "stdafx.h"
#include "WingSword4th.h"

WingSword4th::WingSword4th()
{
	m_fRightOffset = -80.f;
	m_fUpOffset = 0.1f;
	m_fRotationZ = 135.f;
}

void WingSword4th::Free()
{
	WingSword::Free();
}

WingSword4th* WingSword4th::Create()
{
	return new WingSword4th;
}

HRESULT WingSword4th::Ready()
{
	WingSword::Ready();

	RenderInit();

	m_pTransform.lock()->SetScale({ 0.013f,0.013f,0.013f });

	PushEditEntity(m_pTransform.lock().get());
	//SetActive(false);
	return S_OK;
}

HRESULT WingSword4th::Awake()
{
	WingSword::Awake();
	return S_OK;
}

HRESULT WingSword4th::Start()
{
	WingSword::Start();
	return S_OK;
}

UINT WingSword4th::Update(const float _fDeltaTime)
{
	WingSword::Update(_fDeltaTime);
	return 0;
}

UINT WingSword4th::LateUpdate(const float _fDeltaTime)
{
	WingSword::LateUpdate(_fDeltaTime);
	return 0;
}

void WingSword4th::OnEnable()
{
	WingSword::OnEnable();
}

void WingSword4th::OnDisable()
{
	WingSword::OnDisable();
}

void WingSword4th::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

std::string WingSword4th::GetName()
{
	return "WingSword_4th";
}

void WingSword4th::Editor()
{
	Unit::Editor();

	if (bEdit)
	{

	}
}

void WingSword4th::RenderReady()
{
	WingSword::RenderReady();
}

void WingSword4th::RenderGBufferSK(const DrawInfo& _Info)
{
	WingSword::RenderGBufferSK(_Info);
}

void WingSword4th::RenderShadowSK(const DrawInfo& _Info)
{
	WingSword::RenderShadowSK(_Info);
}

void WingSword4th::RenderDebugBone(const DrawInfo& _Info)
{
	WingSword::RenderDebugBone(_Info);
}

void WingSword4th::RenderDebugSK(const DrawInfo& _Info)
{
	WingSword::RenderDebugSK(_Info);
}

void WingSword4th::RenderInit()
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
	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\4th");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Model.Animation");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}
