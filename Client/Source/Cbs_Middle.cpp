#include "stdafx.h"
#include "Cbs_Middle.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

Cbs_Middle::Cbs_Middle()
	:m_pParentMat(nullptr)
{
	m_nTag = Tag_Cbs_Middle;
	m_BattleInfo.eAttackType = Attack_Front;
	m_BattleInfo.iAttack = 15;
	D3DXMatrixIdentity(&m_MyRenderMat);
}

void Cbs_Middle::Free()
{
	Unit::Free();
}

Cbs_Middle* Cbs_Middle::Create()
{
	return new Cbs_Middle;
}

HRESULT Cbs_Middle::Ready()
{
	Unit::Ready();
	RenderInit();
	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);

	m_vecMyBoneMat.reserve(2);

	return S_OK;
}

HRESULT Cbs_Middle::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	m_pParentMat = m_pNero.lock()->Get_BoneMatrixPtr("R_WeaponHand");

	m_vecMyBoneMat.emplace_back(m_pMesh->GetToRootMatrixPtr("pole02"));
	m_vecMyBoneMat.emplace_back(m_pMesh->GetToRootMatrixPtr("pole03"));

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetRadius(0.14f);
	m_pCollider.lock()->SetCenter({ 0.f,0.05f,0.f });

	m_pCollider.lock()->SetActive(false);

	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT Cbs_Middle::Start()
{
	Unit::Start();


	return S_OK;
}

UINT Cbs_Middle::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);


	return 0;
}

UINT Cbs_Middle::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);

	Matrix								ParentWorldMatrix, FinalWorld;

	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

	if (nullptr != m_pParentMat)
	{
		FinalWorld = *m_pParentMat * ParentWorldMatrix;
		m_MyRenderMat = FinalWorld;
		m_pTransform.lock()->SetWorldMatrix(*m_vecMyBoneMat[m_iBoneIndex] * FinalWorld);
	}

	return 0;
}

void Cbs_Middle::OnEnable()
{
	Unit::OnEnable();
	_RenderProperty.bRender = true;
}

void Cbs_Middle::OnDisable()
{
	Unit::OnDisable();
	_RenderProperty.bRender = false;
}

void Cbs_Middle::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Cbs_Middle::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void Cbs_Middle::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Cbs_Middle::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify,const bool bOverlap)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify,1.f,1.f,bOverlap);
}

void Cbs_Middle::ChangeColliderSize(float _fSize)
{
	m_pCollider.lock()->SetRadius(_fSize);
}

std::string Cbs_Middle::GetName()
{
	return "Cbs_Middle";
}

float Cbs_Middle::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float Cbs_Middle::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

void Cbs_Middle::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		//_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
		_RenderUpdateInfo.World = m_MyRenderMat;
		if (m_pMesh)
		{
			const uint32  Numsubset = m_pMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = m_pMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
}

void Cbs_Middle::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
	}
}

void Cbs_Middle::RenderInit()
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
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Cerberos\\Cbs_Middle.fbx");
	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Cerberos\\Cbs_Middle_Ani");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void Cbs_Middle::RenderGBufferSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Cbs_Middle::RenderShadowSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Cbs_Middle::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Cbs_Middle::RenderDebugSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

Matrix* Cbs_Middle::Get_BoneMatrixPtr(std::string _BoneName)
{
	return m_pMesh->GetToRootMatrixPtr(_BoneName);;
}