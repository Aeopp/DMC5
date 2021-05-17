#include "stdafx.h"
#include "BrokenPeople.h"
#include "Subset.h"
#include "Renderer.h"
BrokenPeople::BrokenPeople()
{
}

void BrokenPeople::Free()
{
	GameObject::Free();
}

BrokenPeople* BrokenPeople::Create()
{
	return new BrokenPeople;
}

HRESULT BrokenPeople::Ready()
{
	GameObject::Ready();

	RenderInit();
	m_pTransform.lock()->SetScale({ GScale, GScale, GScale });
	m_pTransform.lock()->SetPosition({ 0.f,0.2f,0.f });
	PushEditEntity(m_pTransform.lock().get());

	return S_OK;
}

HRESULT BrokenPeople::Awake()
{
	GameObject::Awake();
	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->SetRadius(0.08f);
	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT BrokenPeople::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT BrokenPeople::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	if (0.7f <= m_pMesh->PlayingTime())
	{
		Destroy(m_pGameObject);
	}
	return 0;
}

UINT BrokenPeople::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void BrokenPeople::OnEnable()
{
	GameObject::OnEnable();
}

void BrokenPeople::OnDisable()
{
	GameObject::OnDisable();
}

void BrokenPeople::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}
}

std::string BrokenPeople::GetName()
{
	return "BrokenPeople";
}

void BrokenPeople::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	UINT ObjTag = _pOther.lock()->m_nTag;

	if (!(GAMEOBJECTTAG::TAG_RedQueen == ObjTag
		|| GAMEOBJECTTAG::Tag_Cbs_Short == ObjTag
		|| GAMEOBJECTTAG::Tag_Cbs_Middle == ObjTag
		|| GAMEOBJECTTAG::Tag_Cbs_Long == ObjTag))
		return;
	m_pCollider.lock()->SetActive(false);
	m_pMesh->PlayAnimation(0, false);
}

void BrokenPeople::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void BrokenPeople::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void BrokenPeople::RenderGBufferSK(const DrawInfo& _Info)
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

void BrokenPeople::RenderShadowSK(const DrawInfo& _Info)
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

void BrokenPeople::RenderDebugBone(const DrawInfo& _Info)
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

void BrokenPeople::RenderDebugSK(const DrawInfo& _Info)
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

void BrokenPeople::RenderInit()
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
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	_InitInfo.bJoinIdenticalVertices = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Map\\Object\\BrokenPeople\\BrokenPeople.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	Quaternion QuatIdentity{ 0.f,0.f,0.f,1.f };
	m_pMesh->GetRootNode()->NodeUpdate(FMath::Identity(), 0.f, "", {}, QuatIdentity);
	m_pMesh->UpdateToRootMatricies();
	m_pMesh->VTFUpdate();
	PushEditEntity(m_pMesh.get());
}
