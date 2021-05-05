#include "stdafx.h"
#include "WingSword2nd.h"

WingSword2nd::WingSword2nd()
{
	m_fRightOffset = -80.f;
	m_fUpOffset = 0.2f;
	m_fRotationZ = 45.f;
}

void WingSword2nd::Free()
{
	WingSword::Free();
}

WingSword2nd* WingSword2nd::Create()
{
	return new WingSword2nd;
}

HRESULT WingSword2nd::Ready()
{
	WingSword::Ready();

	RenderInit();

	m_pTransform.lock()->SetScale({ 0.013f,0.013f,0.013f });

	PushEditEntity(m_pTransform.lock().get());
	//SetActive(false);
	return S_OK;
}

HRESULT WingSword2nd::Awake()
{
	WingSword::Awake();
	return S_OK;
}

HRESULT WingSword2nd::Start()
{
	WingSword::Start();
	return S_OK;
}

UINT WingSword2nd::Update(const float _fDeltaTime)
{
	WingSword::Update(_fDeltaTime);
	return 0;
}

UINT WingSword2nd::LateUpdate(const float _fDeltaTime)
{
	WingSword::LateUpdate(_fDeltaTime);
	return 0;
}

void WingSword2nd::OnEnable()
{
	WingSword::OnEnable();
}

void WingSword2nd::OnDisable()
{
	WingSword::OnDisable();
}

void WingSword2nd::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

std::string WingSword2nd::GetName()
{
	return "WingSword_2nd";
}

void WingSword2nd::Editor()
{
	Unit::Editor();
	static bool MyButton = true;
	static float ZeroDotOne = 0.1f;
	if (bEdit)
	{
		ImGui::InputScalar("Distance", ImGuiDataType_Float, &m_fLookOffset, MyButton ? &ZeroDotOne : NULL);
	}
}

void WingSword2nd::RenderReady()
{
	WingSword::RenderReady();
}

void WingSword2nd::RenderGBufferSK(const DrawInfo& _Info)
{
	WingSword::RenderGBufferSK(_Info);
}

void WingSword2nd::RenderShadowSK(const DrawInfo& _Info)
{
	WingSword::RenderShadowSK(_Info);
}

void WingSword2nd::RenderDebugBone(const DrawInfo& _Info)
{
	WingSword::RenderDebugBone(_Info);
}

void WingSword2nd::RenderDebugSK(const DrawInfo& _Info)
{
	WingSword::RenderDebugSK(_Info);
}

void WingSword2nd::RenderInit()
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
	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\2nd");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Model.Animation");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}
