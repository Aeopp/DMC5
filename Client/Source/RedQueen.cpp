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

	m_pGlint = AddGameObject<Glint>();
	
	
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
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetTrigger(true);

	m_pCollider.lock()->SetRadius(0.05f);
	m_pCollider.lock()->SetHeight(0.11f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.05f, 0.f });

	m_pCollider.lock()->SetContactOffset(2.f);

	m_pCollider.lock()->SetActive(false);

	PushEditEntity(m_pCollider.lock().get());
	m_pMyBoneMat = m_pMesh->GetToRootMatrixPtr("_001");
	return S_OK;
}

HRESULT RedQueen::Start()
{
	Unit::Start();

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

void RedQueen::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
}

void RedQueen::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (GAMEOBJECTTAG::Player == _pOther.lock()->m_nTag)
		return;

	Vector3 vGlintPos;
	memcpy(&vGlintPos, (*m_pMyBoneMat * m_pTransform.lock()->GetWorldMatrix()).m[3], sizeof(Vector3));

	// + m_pGlint.lock()->SetScale(적당히 작은 사이즈)
	m_pGlint.lock()->SetScale(0.045f);
	m_pGlint.lock()->SetPosition(vGlintPos);
	m_pGlint.lock()->PlayStart(3.5f);
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
	// bRender ���� ���� ȣ�� �ȵ� .


	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
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
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
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
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;
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

	// ���̷��� �޽� �ε� ... 
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Static\\RedQueen\\RedQueen.fbx", _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void RedQueen::Editor()
{
	Unit::Editor();
}

Matrix* RedQueen::Get_BoneMatrixPtr(std::string _BoneName)
{
	return m_pMesh->GetToRootMatrixPtr(_BoneName);;
}

Vector3 RedQueen::GetRedQueenBoneWorldPos(std::string _BoneName)
{
	Vector3 WorldPos;
	memcpy(&WorldPos, (*m_pMesh->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix()).m[3], sizeof(Vector3));
	return WorldPos;
}
