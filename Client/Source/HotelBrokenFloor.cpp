#include "stdafx.h"
#include "HotelBrokenFloor.h"
#include "Subset.h"
#include "Renderer.h"
#include "Smoke.h"
#include "SoundSystem.h"
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

	// smoke
	m_pSmoke[0] = AddGameObject<Smoke>();
	if (auto Sp = m_pSmoke[0].lock(); Sp)
	{
		Sp->SetVariationIdx(Smoke::VARIATION::SMOKE_1_SHORT);
		Sp->SetScale(0.006f);
		Sp->SetPosition({ 0.f, 1.46f, 16.5f });
		Sp->SetRotation({ 84.255f, 0.f, 61.277f });
		Sp->SetActive(false);
	}
	m_pSmoke[1] = AddGameObject<Smoke>();
	if (auto Sp = m_pSmoke[1].lock(); Sp)
	{
		Sp->SetVariationIdx(Smoke::VARIATION::SMOKE_1);
		Sp->SetScale(0.009f);
		Sp->SetPosition({ -1.15f, 0.98f, 15.5f });
		Sp->SetRotation({ 0.f, 131.128f, 0.f });
		Sp->SetActive(false);
	}

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

	if (0.16f <= m_pMesh->PlayingTime())
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
	SoundSystem::GetInstance()->RandSoundKeyPlay("HitStone", {1,3}, 0.7f, false);
	++m_iCollCount;
	if (m_iCollCount > 4)
	{
		m_pMesh->ContinueAnimation();
		
		if (auto Sp = m_pSmoke[0].lock(); Sp)
		{
			Sp->SetVariationIdx(Smoke::VARIATION::SMOKE_1);
			Sp->SetScale(0.009f);
			Sp->SetPosition({ 0.3f, 0.98f, 15.4f });
			Sp->SetRotation({ 0.f, 46.f, 0.f });
			Sp->PlayStart(33.f);
		}
		if (auto Sp = m_pSmoke[1].lock(); Sp)
		{
			Sp->SetActive(true);
			Sp->PlayStart(33.f);
		}
		SoundSystem::GetInstance()->Play("BrokenFloor", 0.7f, false);
		SoundSystem::GetInstance()->Play("Stone2", 0.7f, false);
	}
	else
	{
		if (auto Sp = m_pSmoke[0].lock(); Sp)
		{
			if (!Sp->IsPlaying())
			{
				Sp->SetActive(true);
				Sp->PlayStart(10.f);
			}
		}
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
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	_InitInfo.bJoinIdenticalVertices = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Map\\Object\\HotelBrokenFloor\\HotelBrokenFloor.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	m_pMesh->PlayAnimation(0, false);
	m_pMesh->Update(0.0016f);
	m_pMesh->StopAnimation();

	PushEditEntity(m_pMesh.get());
}
