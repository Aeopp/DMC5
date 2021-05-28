#include "stdafx.h"
#include "JudgementShadow3.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

JudgementShadow3::JudgementShadow3()
{
	m_nTag = TAG_JudgementShadow3;
};

void JudgementShadow3::Free()
{
	GameObject::Free();
};

JudgementShadow3* JudgementShadow3::Create()
{
	return new JudgementShadow3;
};


HRESULT JudgementShadow3::Ready()
{
	GameObject::Ready();
	RenderInit();

	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);

	return S_OK;
}

HRESULT JudgementShadow3::Awake()
{
	GameObject::Awake();

	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	return S_OK;
}

HRESULT JudgementShadow3::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT JudgementShadow3::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	if (m_pMesh->IsAnimationEnd())
	{
		SetActive(false);
	}

	return 0;
}

UINT JudgementShadow3::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void JudgementShadow3::OnEnable()
{
	GameObject::OnEnable();
	_RenderProperty.bRender = true;
	m_pTransform.lock()->SetWorldMatrix(m_pNero.lock()->Get_NeroWorldMatrix());
}

void JudgementShadow3::OnDisable()
{
	GameObject::OnDisable();
	_RenderProperty.bRender = false;
}

void JudgementShadow3::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

void JudgementShadow3::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
	}
}

void JudgementShadow3::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}
}

std::string JudgementShadow3::GetName()
{
	return "JudgementShadow3";
}

void JudgementShadow3::RenderGBufferSK(const DrawInfo& _Info)
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

void JudgementShadow3::RenderShadowSK(const DrawInfo& _Info)
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

void JudgementShadow3::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void JudgementShadow3::RenderDebugSK(const DrawInfo& _Info)
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

void JudgementShadow3::RenderInit()
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
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<SkeletonMesh>(
		L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\ShinMajin\\JudgementShadow.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();

	PushEditEntity(m_pMesh.get());

	
}
