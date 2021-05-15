#include "stdafx.h"
#include "BrokenTable.h"
#include "Subset.h"
#include "Renderer.h"
BrokenTable::BrokenTable()
{
}

void BrokenTable::Free()
{
	GameObject::Free();
}

BrokenTable* BrokenTable::Create()
{
	return new BrokenTable;
}

HRESULT BrokenTable::Ready()
{
	GameObject::Ready();

	RenderInit();
	m_pTransform.lock()->SetScale({ GScale, -GScale, GScale });
	m_pTransform.lock()->SetRotation({ 15.f,0.f,0.f });
	m_pTransform.lock()->SetPosition({ 0.f,0.1f,0.f });
	PushEditEntity(m_pTransform.lock().get());

	return S_OK;
}

HRESULT BrokenTable::Awake()
{
	GameObject::Awake();
	m_pCollider = AddComponent<BoxCollider>();
	m_pCollider.lock()->SetSize({0.35f,0.1f,0.1f });
	m_pCollider.lock()->SetCenter({ 0.f,0.05f,0.f });
	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT BrokenTable::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT BrokenTable::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	return 0;
}

UINT BrokenTable::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void BrokenTable::OnEnable()
{
	GameObject::OnEnable();
}

void BrokenTable::OnDisable()
{
	GameObject::OnDisable();
}

void BrokenTable::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}
}

std::string BrokenTable::GetName()
{
	return "BrokenTable";
}

void BrokenTable::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
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

void BrokenTable::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void BrokenTable::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void BrokenTable::RenderGBufferSK(const DrawInfo& _Info)
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

void BrokenTable::RenderShadowSK(const DrawInfo& _Info)
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

void BrokenTable::RenderDebugBone(const DrawInfo& _Info)
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

void BrokenTable::RenderDebugSK(const DrawInfo& _Info)
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

void BrokenTable::RenderInit()
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
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Map\\Object\\BrokenTable\\BrokenTable.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	m_pMesh->PlayAnimation(0, false);
	m_pMesh->Update(0.0016f);
	m_pMesh->StopAnimation();
	
	PushEditEntity(m_pMesh.get());
}
