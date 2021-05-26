#include "stdafx.h"
#include "Cbs_Long.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

Cbs_Long::Cbs_Long()
	:m_pParentMat(nullptr)
{
	m_nTag = Tag_Cbs_Long;
	m_BattleInfo.eAttackType = Attack_Front;
	m_BattleInfo.iAttack = 10;
}

void Cbs_Long::Free()
{
	Unit::Free();
}

Cbs_Long* Cbs_Long::Create()
{
	return new Cbs_Long;
}

HRESULT Cbs_Long::Ready()
{
	Unit::Ready();
	RenderInit();
	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);
	return S_OK;
}

HRESULT Cbs_Long::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	m_pParentMat = m_pNero.lock()->Get_BoneMatrixPtr("R_WeaponHand");

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetRadius(0.05f);
	m_pCollider.lock()->SetHeight(0.11f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.05f, 0.f });

	m_pCollider.lock()->SetActive(false);

	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT Cbs_Long::Start()
{
	Unit::Start();


	return S_OK;
}

UINT Cbs_Long::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	Quaternion QuatIdentity{ 0.f,0.f,0.f,1.f };
	m_pMesh->GetRootNode()->NodeUpdate(FMath::Identity(), 0.f, "", {}, QuatIdentity);
	m_pMesh->UpdateToRootMatricies();
	m_pMesh->VTFUpdate();

	SetActive(true);
	return 0;
}

UINT Cbs_Long::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);

	Matrix								ParentWorldMatrix, FinalWorld;

	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

	Matrix RotX;
	D3DXMatrixRotationX(&RotX, D3DXToRadian(-90));
	if (nullptr != m_pParentMat)
	{
		FinalWorld = RotX * *m_pParentMat * ParentWorldMatrix;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}

	return 0;
}

void Cbs_Long::OnEnable()
{
	Unit::OnEnable();

	_RenderProperty.bRender = true;
}

void Cbs_Long::OnDisable()
{
	Unit::OnDisable();

	_RenderProperty.bRender = false;
}

void Cbs_Long::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Cbs_Long::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void Cbs_Long::Hit(BT_INFO _BattleInfo, void* pArg )
{
}

std::string Cbs_Long::GetName()
{
	return "Cbs_Long";
}

Matrix* Cbs_Long::Get_BoneMatrixPtr(std::string _BoneName)
{
	return m_pMesh->GetToRootMatrixPtr(_BoneName);
}

void Cbs_Long::ChangeColliderSize(float _fSize)
{
	m_pCollider.lock()->SetRadius(_fSize);
}

void Cbs_Long::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
	}
}

void Cbs_Long::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
	}
}

void Cbs_Long::RenderInit()
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

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Cerberos\\Cbs_Long.fbx", _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void Cbs_Long::RenderGBufferSK(const DrawInfo& _Info)
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

void Cbs_Long::RenderShadowSK(const DrawInfo& _Info)
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

void Cbs_Long::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Cbs_Long::RenderDebugSK(const DrawInfo& _Info)
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
