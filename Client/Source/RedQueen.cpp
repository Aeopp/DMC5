#include "stdafx.h"
#include "RedQueen.h"
#include "Renderer.h"
#include "Subset.h"
#include "Nero.h"
#include "Glint.h"
RedQueen::RedQueen()
{
	m_nTag = TAG_RedQueen;
	m_BattleInfo.iAttack = 10;
	m_BattleInfo.eAttackType = Attack_END;
}

void RedQueen::Free()
{
	Unit::Free();
}

RedQueen* RedQueen::Create()
{
	return new RedQueen;
}

HRESULT RedQueen::Ready()
{
	Unit::Ready();
	RenderInit();

	m_pTransform.lock()->SetScale({ 0.001f,0.001f,0.001f });

	
	PushEditEntity(m_pTransform.lock().get());

	m_vecParentMat.reserve(2);

	_RenderProperty.bRender = true;
	
	
	return S_OK;
}

HRESULT RedQueen::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_vecParentMat.emplace_back(m_pNero.lock()->Get_BoneMatrixPtr("WeaponConst"));
	m_vecParentMat.emplace_back(m_pNero.lock()->Get_BoneMatrixPtr("L_WeaponHand"));

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);

	m_pCollider.lock()->SetRadius(0.1f);
	m_pCollider.lock()->SetHeight(0.55f);
	m_pCollider.lock()->SetCenter({ 0.f, 1.f, 0.f });

	PushEditEntity(m_pCollider.lock().get());

	return S_OK;
}

HRESULT RedQueen::Start()
{
	Unit::Start();
	m_pGlint = std::static_pointer_cast<Glint>(FindGameObjectWithTag(GAMEOBJECTTAG::Eff_Glint).lock());
	return S_OK;
}

UINT RedQueen::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	Quaternion QuatIdentity{0.f,0.f,0.f,1.f};
	m_pMesh->GetRootNode()->NodeUpdate(FMath::Identity(), 0.f, "", {} , QuatIdentity);
	m_pMesh->UpdateToRootMatricies();
	m_pMesh->VTFUpdate();
	
	return 0;
}

UINT RedQueen::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	Matrix								ParentWorldMatrix,FinalWorld,RotX;

	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();
	
	D3DXMatrixRotationX(&RotX, D3DXToRadian(-90));
	if (nullptr != m_vecParentMat[m_iStateIndex])
	{
		FinalWorld = RotX * *m_vecParentMat[m_iStateIndex] * ParentWorldMatrix;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}

	//m_pTransform.lock()->UpdateTransform();

	return 0;
}

void RedQueen::OnEnable()
{
	Unit::OnEnable();
	_RenderProperty.bRender = true;
}

void RedQueen::OnDisable()
{
	Unit::OnDisable();
	_RenderProperty.bRender = false;
}

void RedQueen::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void RedQueen::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void RedQueen::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

std::string RedQueen::GetName()
{
	return "RedQueen";
}

void RedQueen::RenderReady()
{
	// bRender 끄면 렌더 호출 안됨 .


	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void RedQueen::RenderGBufferSK(const DrawInfo& _Info)
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

void RedQueen::RenderShadowSK(const DrawInfo& _Info)
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

void RedQueen::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void RedQueen::RenderDebugSK(const DrawInfo& _Info)
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

void RedQueen::RenderInit()
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

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Static\\RedQueen\\RedQueen.fbx", _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void RedQueen::Editor()
{
	Unit::Editor();
}
