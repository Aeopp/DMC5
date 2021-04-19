#include "stdafx.h"
#include "..\Header\Em0000_Weapon.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Em0000.h"



void Em0000Weapon::Free()
{
	GameObject::Free();
}

std::string Em0000Weapon::GetName()
{
	return "Em0000Weapon";
};

Em0000Weapon* Em0000Weapon::Create()
{
	return new Em0000Weapon{};
}


void Em0000Weapon::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

HRESULT Em0000Weapon::Ready()
{
	GameObject::Ready();

	RenderInit();

	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.005f,0.005f,0.005f});
	PushEditEntity(InitTransform.lock().get());


	m_pTransform = GetComponent<ENGINE::Transform>();




	return S_OK;
};

HRESULT Em0000Weapon::Awake()
{
	GameObject::Awake();
	m_pEm0000Trasform = m_pEm0000.lock()->GetComponent<Transform>();

	m_pParentBone = m_pEm0000Mesh.lock()->GetToRootMatrixPtr("R_WeaponHand");

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());


	m_pCollider.lock()->SetGravity(false);

	m_pCollider.lock()->SetCenter({ -1.f,0.f,-1.7f });
	m_pCollider.lock()->SetRadius(0.8f);
	

	return S_OK;
}

HRESULT Em0000Weapon::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT Em0000Weapon::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	//////////////무기 붙이기////////////////////////////
	//오른손에만 붙였는데 왼손도 딱 맞게 붙음.
	//m_ParentBone = m_pEm0000Mesh.lock()->GetNodeToRoot("R_WeaponHand");
	//if (!m_ParentBone)
	//	return 0;
	m_ParentWorld = m_pEm0000Trasform.lock()->GetWorldMatrix();
	m_Result = (*m_pParentBone * m_ParentWorld);
	m_pTransform.lock()->SetWorldMatrix(m_Result);
	//////////////////////////////////////////////////
	

	return 0;
}

UINT Em0000Weapon::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void Em0000Weapon::Editor()
{
	GameObject::Editor();
}


void Em0000Weapon::OnEnable()
{

}

void Em0000Weapon::OnDisable()
{

}

void Em0000Weapon::RenderInit()
{
	SetRenderEnable(true);
	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_ds",
		[this](const DrawInfo& _Info)
			{
				RenderGBuffer(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"Shadow" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadow(_Info);
		}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
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
	m_pStaticMesh = Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em0000\\Weapon\\Weapon.fbx");
	PushEditEntity(m_pStaticMesh.get());
}

void Em0000Weapon::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em0000Weapon::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em0000Weapon::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}


