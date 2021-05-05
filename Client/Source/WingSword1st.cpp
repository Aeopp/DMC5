#include "stdafx.h"
#include "WingSword1st.h"

WingSword1st::WingSword1st()
{
	m_fRightOffset = 80.f;
	m_fUpOffset = 0.2f;
	m_fRotationZ = -45.f;
}

void WingSword1st::Free()
{
	WingSword::Free();
}

WingSword1st* WingSword1st::Create()
{
	return new WingSword1st;
}

HRESULT WingSword1st::Ready()
{
	WingSword::Ready();

	RenderInit();

	m_pTransform.lock()->SetScale({ 0.013f,0.013f,0.013f });

	PushEditEntity(m_pTransform.lock().get());
	//SetActive(false);
	return S_OK;
}

HRESULT WingSword1st::Awake()
{
	WingSword::Awake();
	return S_OK;
}

HRESULT WingSword1st::Start()
{
	WingSword::Start();
	return S_OK;
}

UINT WingSword1st::Update(const float _fDeltaTime)
{
	WingSword::Update(_fDeltaTime);
	return 0;
}

UINT WingSword1st::LateUpdate(const float _fDeltaTime)
{
	WingSword::LateUpdate(_fDeltaTime);
	return 0;
}

void WingSword1st::OnEnable()
{
	WingSword::OnEnable();
}

void WingSword1st::OnDisable()
{
	WingSword::OnDisable();
}

void WingSword1st::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

std::string WingSword1st::GetName()
{
	return "WingSword_1st";
}

void WingSword1st::Editor()
{
	Unit::Editor();

	static bool MyButton = true;
	static float ZeroDotOne = 0.1f;
	if (bEdit)
	{
		ImGui::InputScalar("Distance", ImGuiDataType_Float, &m_fRightOffset, MyButton ? &ZeroDotOne : NULL);
	}
}

void WingSword1st::RenderReady()
{
	WingSword::RenderReady();
}

void WingSword1st::RenderGBufferSK(const DrawInfo& _Info)
{
	WingSword::RenderGBufferSK(_Info);
}

void WingSword1st::RenderShadowSK(const DrawInfo& _Info)
{
	WingSword::RenderShadowSK(_Info);
}

void WingSword1st::RenderDebugBone(const DrawInfo& _Info)
{
	WingSword::RenderDebugBone(_Info);
}

void WingSword1st::RenderDebugSK(const DrawInfo& _Info)
{
	WingSword::RenderDebugSK(_Info);
}

void WingSword1st::RenderInit()
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
	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\1st");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Model.Animation");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}
