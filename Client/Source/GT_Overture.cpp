#include "stdafx.h"
#include "GT_Overture.h"

GT_Overture::GT_Overture()
{
}

void GT_Overture::Free()
{
	Gauntlet::Free();
}

GT_Overture* GT_Overture::Create()
{
	return new GT_Overture;
}

HRESULT GT_Overture::Ready()
{
	Gauntlet::Ready();
	return S_OK;
}

HRESULT GT_Overture::Awake()
{
	Gauntlet::Awake();
	return S_OK;
}

HRESULT GT_Overture::Start()
{
	Gauntlet::Start();
	return S_OK;
}

UINT GT_Overture::Update(const float _fDeltaTime)
{
	Gauntlet::Update(_fDeltaTime);
	return 0;
}

UINT GT_Overture::LateUpdate(const float _fDeltaTime)
{
	Gauntlet::LateUpdate(_fDeltaTime);
	return 0;
}

void GT_Overture::OnEnable()
{
	Gauntlet::OnEnable();
}

void GT_Overture::OnDisable()
{
	Gauntlet::OnDisable();
}

void GT_Overture::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

std::string GT_Overture::GetName()
{
	return "Overture";
}

void GT_Overture::Editor()
{
}

void GT_Overture::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void GT_Overture::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}


void GT_Overture::RenderReady()
{
	Gauntlet::RenderReady();
}

void GT_Overture::RenderGBufferSK(const DrawInfo& _Info)
{
	Gauntlet::RenderGBufferSK(_Info);
}

void GT_Overture::RenderShadowSK(const DrawInfo& _Info)
{
	Gauntlet::RenderShadowSK(_Info);
}

void GT_Overture::RenderDebugBone(const DrawInfo& _Info)
{
	Gauntlet::RenderDebugBone(_Info);
}

void GT_Overture::RenderDebugSK(const DrawInfo& _Info)
{
	Gauntlet::RenderDebugSK(_Info);
}

void GT_Overture::RenderInit()
{
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
		// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 속성 전체 초기화 
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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

	// 스켈레톤 메쉬 로딩 ... 
	Mesh::InitializeInfo _InitInfo{};
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;

	//m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Static\\RedQueen\\RedQueen.fbx", _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());


}