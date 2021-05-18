#include "stdafx.h"
#include "HotelBrokenFloor.h"
#include "Subset.h"
#include "Renderer.h"
HotelBrokenFloor::HotelBrokenFloor()
{
}

void HotelBrokenFloor::Free()
{
	GameObject::Free();
}

HotelBrokenFloor* HotelBrokenFloor::Create()
{
	return new HotelBrokenFloor;
}

HRESULT HotelBrokenFloor::Ready()
{
	GameObject::Ready();

	RenderInit();
	m_pTransform.lock()->SetScale({ 0.0001f, 0.0001f, 0.0001f });
	PushEditEntity(m_pTransform.lock().get());

	return S_OK;
}

HRESULT HotelBrokenFloor::Awake()
{
	GameObject::Awake();
	m_pCollider = AddComponent<BoxCollider>();
	m_pCollider.lock()->SetSize({ 0.8f,0.2f,0.8f });
	m_pCollider.lock()->SetCenter({ -0.27f,1.23f,15.8f });
	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT HotelBrokenFloor::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT HotelBrokenFloor::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	if(0.16f <= m_pMesh->PlayingTime())
		m_pCollider.lock()->SetActive(false);
	return 0;
}

UINT HotelBrokenFloor::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void HotelBrokenFloor::OnEnable()
{
	GameObject::OnEnable();
}

void HotelBrokenFloor::OnDisable()
{
	GameObject::OnDisable();
}

void HotelBrokenFloor::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}

}

std::string HotelBrokenFloor::GetName()
{
	return "HotelBrokenFloor";
}

void HotelBrokenFloor::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	UINT ObjTag = _pOther.lock()->m_nTag;

	if (GAMEOBJECTTAG::TAG_RedQueen != ObjTag)
		return;
	++m_iCollCount;
	if (m_iCollCount > 4)
	{
		m_pMesh->ContinueAnimation();
	}
}

void HotelBrokenFloor::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void HotelBrokenFloor::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void HotelBrokenFloor::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void HotelBrokenFloor::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void HotelBrokenFloor::RenderDebugBone(const DrawInfo& _Info)
{
}

void HotelBrokenFloor::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void HotelBrokenFloor::RenderInit()
{
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
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	_InitInfo.bJoinIdenticalVertices = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Map\\Object\\HotelBrokenFloor\\HotelBrokenFloor.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	m_pMesh->PlayAnimation(0, false);
	m_pMesh->Update(0.0016f);
	m_pMesh->StopAnimation();

	PushEditEntity(m_pMesh.get());
}