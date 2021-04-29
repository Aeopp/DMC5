#include "stdafx.h"
#include "Cbs_Short.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

Cbs_Short::Cbs_Short()
	:m_iStateIndex(0)
{
	m_nTag = Tag_Cbs_Short;
	m_BattleInfo.eAttackType = Attack_Front;
	m_BattleInfo.iAttack = 10;
}

void Cbs_Short::Free()
{
	Unit::Free();
}

Cbs_Short* Cbs_Short::Create()
{
	return new Cbs_Short;
}

HRESULT Cbs_Short::Ready()
{
	Unit::Ready();
	RenderInit();
	m_vecParentMat.reserve(3);
	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());

	return S_OK;
}

HRESULT Cbs_Short::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_vecParentMat.emplace_back(m_pNero.lock()->Get_BoneMatrixPtr("WeaponConst"));
	m_vecParentMat.emplace_back(m_pNero.lock()->Get_BoneMatrixPtr("R_WeaponHand"));
	m_vecParentMat.emplace_back(m_pNero.lock()->Get_BoneMatrixPtr("L_WeaponHand"));

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRadius(0.11f);
	m_pCollider.lock()->SetCenter({ 0.f,0.1f,0.f });

	m_pCollider.lock()->SetActive(false);

	PushEditEntity(m_pCollider.lock().get());
	m_pMesh->PlayAnimation("Cbs_Idle", true);

	return S_OK;
}

HRESULT Cbs_Short::Start()
{
	Unit::Start();


	return S_OK;
}

UINT Cbs_Short::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	if (Input::GetKey(DIK_Y))
	{
		m_vecParentMat[1] = m_pNero.lock()->Get_BoneMatrixPtr("R_WeaponHand");
	}
	if(Input::GetKey(DIK_U))
		m_vecParentMat[1] = m_pNero.lock()->Get_BoneMatrixPtr("L_WeaponHand");
	if (Input::GetKey(DIK_I))
		m_vecParentMat[1] = m_pNero.lock()->Get_BoneMatrixPtr("WeaponConst");

	//if (Input::GetKey(DIK_Y))
	//{
	//	m_vTest = m_pNero.lock()->Get_NeroBoneWorldPos("R_WeaponHand");
	//}
	//if (Input::GetKey(DIK_U))
	//	m_vTest = m_pNero.lock()->Get_NeroBoneWorldPos("L_WeaponHand");
	//if (Input::GetKey(DIK_I))
	//	m_vTest = m_pNero.lock()->Get_NeroBoneWorldPos("WeaponConst");

	return 0;
}

UINT Cbs_Short::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);

	Matrix								ParentWorldMatrix, FinalWorld;

	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

	if (nullptr != m_vecParentMat[m_iStateIndex])
	{
		FinalWorld = *m_vecParentMat[m_iStateIndex] * ParentWorldMatrix;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}

	//Matrix		Scale,RotX,RotY, Pos;

	//D3DXMatrixScaling(&Scale, GScale, GScale, GScale);
	//D3DXMatrixRotationX(&RotX, D3DXToRadian(90.f));
	//D3DXMatrixRotationY(&RotY, D3DXToRadian(180.f));
	//D3DXMatrixTranslation(&Pos, m_vTest.x, m_vTest.y, m_vTest.z);

	//m_pTransform.lock()->SetWorldMatrix(Scale * RotX * RotY * Pos);

	return 0;
}

void Cbs_Short::OnEnable()
{
	Unit::OnEnable();
}

void Cbs_Short::OnDisable()
{
	Unit::OnDisable();
}

void Cbs_Short::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Cbs_Short::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void Cbs_Short::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Cbs_Short::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify, const bool _bOverlap)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify,1.f,1.f,_bOverlap);
}

std::string Cbs_Short::GetName()
{
	return "Cbs_Short";
}

float Cbs_Short::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float Cbs_Short::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

void Cbs_Short::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
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

void Cbs_Short::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
	}
}

void Cbs_Short::RenderInit()
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
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Cerberos\\Cbs_Short.fbx");
	//m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wire_Arm\\Wire_Arm.Animation");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void Cbs_Short::RenderGBufferSK(const DrawInfo& _Info)
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

void Cbs_Short::RenderShadowSK(const DrawInfo& _Info)
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

void Cbs_Short::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Cbs_Short::RenderDebugSK(const DrawInfo& _Info)
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
