#include "stdafx.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"
#include "NeroFSM.h"
#include "RedQueen.h"
#include "Nero_LWing.h"
#include "Nero_RWing.h"
#include "Buster_Arm.h"
#include "Wire_Arm.h"
#include "WIngArm_Left.h"
#include "WingArm_Right.h"
#include "MainCamera.h"
Nero::Nero()
	:m_iCurAnimationIndex(ANI_END)
	, m_iPreAnimationIndex(ANI_END)
	, m_iCurWeaponIndex(RQ)
	, m_iJumpDirIndex(Basic)
	, m_fRedQueenGage(0.f)
{
	m_nTag = Player;
}
void Nero::Free()
{
	m_pFSM = nullptr;
}

void Nero::Set_RQ_State(UINT _StateIndex)
{
	m_pRedQueen.lock()->SetWeaponState(_StateIndex);
}

void Nero::Set_PlayingTime(float NewTime)
{
	m_pMesh->SetPlayingTime(NewTime);
}

std::string Nero::GetName()
{
	return "Nero";
}

Nero* Nero::Create()
{
	return new Nero;
}

HRESULT Nero::Ready()
{
	RenderInit();

	m_pTransform.lock()->SetScale({ 0.01f,0.01f,0.01f });
	//m_pTransform.lock()->SetPosition(Vector3{- 0.,32.14068f,9.48852f});
	PushEditEntity(m_pTransform.lock().get());

	m_pRedQueen = AddGameObject<RedQueen>();
	m_pLWing = AddGameObject<Nero_LWing>();
	m_pRWing = AddGameObject<Nero_RWing>();
	m_pBusterArm = AddGameObject<Buster_Arm>();
	m_pWireArm = AddGameObject<Wire_Arm>();
	m_pWingArm_Left = AddGameObject <WIngArm_Left>();
	m_pWingArm_Right = AddGameObject<WingArm_Right>();

	m_pFSM.reset(NeroFSM::Create(static_pointer_cast<Nero>(m_pGameObject.lock())));

	m_iCurAnimationIndex = ANI_END;
	m_iPreAnimationIndex = ANI_END;

	return S_OK;
}


HRESULT Nero::Awake()
{
	m_pFSM->ChangeState(NeroFSM::IDLE);

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetCenter(D3DXVECTOR3(0.f, 0.8f, 0.f));
	m_pCollider.lock()->SetRadius(0.4f);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);


	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT Nero::Start()
{
	m_pCamera = std::static_pointer_cast<MainCamera>(FindGameObjectWithTag(TAG_Camera).lock());
	return S_OK;
}

UINT Nero::Update(const float _fDeltaTime)
{
	//GameObject::Update(_fDeltaTime);
	if (Input::GetKeyDown(DIK_0))
		m_bDebugButton = !m_bDebugButton;
	
	if (nullptr != m_pFSM && m_bDebugButton)
		m_pFSM->UpdateFSM(_fDeltaTime);

	auto [Scale,Rot,Pos] =m_pMesh->Update(_fDeltaTime);
	Matrix RotY;
	D3DXMatrixRotationY(&RotY, D3DXToRadian(m_fAngle));
	D3DXVec3TransformCoord(&Pos, &Pos, &RotY);

	m_pTransform.lock()->Translate(Pos * m_pTransform.lock()->GetScale().x);
	//m_pTransform.lock()->SetPosition(m_pTransform.lock()->GetPosition() + Pos * m_pTransform.lock()->GetScale().x);
	

	return 0;
}

UINT Nero::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Nero::OnEnable()
{
}

void Nero::OnDisable()
{
}

void Nero::RenderGBufferSK(const DrawInfo& _Info)
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

void Nero::RenderShadowSK(const DrawInfo& _Info)
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

void Nero::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Nero::RenderDebugSK(const DrawInfo& _Info)
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

void Nero::RenderInit()
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
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Player.fbx", _InitInfo);

	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Animation");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Player.Animation");

	m_pMesh->EnableToRootMatricies();

	PushEditEntity(m_pMesh.get());
}

void Nero::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}


void Nero::Editor()
{
	GameObject::Editor();
	bool MyButton = true;
	float ZeroDotOne = 0.1f;
	if (bEdit)
	{
		// 에디터 .... 
		ImGui::InputScalar("RotY", ImGuiDataType_Float, &m_fRotationAngle, MyButton ? &ZeroDotOne : NULL);
	}

}

float Nero::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float Nero::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

optional<Matrix> Nero::Get_BoneMatrix_ByName(std::string _BoneName)
{
	return m_pMesh->GetNodeToRoot(_BoneName);
}

Matrix* Nero::Get_BoneMatrixPtr(std::string _BoneName)
{
	return m_pMesh->GetToRootMatrixPtr(_BoneName);;
}

void Nero::SetActive_Wings(bool ActiveOrNot)
{
	m_pLWing.lock()->SetActive(ActiveOrNot);
	m_pRWing.lock()->SetActive(ActiveOrNot);
}

void Nero::SetActive_Buster_Arm(bool ActiveOrNot)
{
	m_pBusterArm.lock()->SetActive(ActiveOrNot);
}

void Nero::SetActive_Wire_Arm(bool ActiveOrNot)
{
	m_pWireArm.lock()->SetActive(ActiveOrNot);
}

void Nero::SetActive_WingArm_Right(bool ActiveOrNot)
{
	m_pWingArm_Right.lock()->SetActive(ActiveOrNot);
}

void Nero::SetActive_WingArm_Left(bool ActiveOrNot)
{
	m_pWingArm_Left.lock()->SetActive(ActiveOrNot);
}

void Nero::SetAngleFromCamera()
{
	if (m_pCamera.expired())
		return;
	m_fAngle = m_pCamera.lock()->Get_Angle();
	m_fAngle += m_fRotationAngle;
	m_pTransform.lock()->SetRotation(Vector3(0.f, m_fAngle, 0.f));
}

void Nero::StopAnimation()
{
	m_pMesh->StopAnimation();
}

void Nero::ContinueAnimiation()
{
	m_pMesh->ContinueAnimation();
}

bool Nero::IsAnimationEnd()
{
	return m_pMesh->IsAnimationEnd();
}

void Nero::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const UINT AnimationIndex, const AnimNotify& _Notify)
{
	m_iPreAnimationIndex = m_iCurAnimationIndex;
	m_iCurAnimationIndex = AnimationIndex;
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

void Nero::ChangeAnimationIndex(const UINT AnimationIndex)
{
	m_iPreAnimationIndex = m_iCurAnimationIndex;
	m_iCurAnimationIndex = AnimationIndex;
}

void Nero::ChangeWeapon(UINT _iWeaponIndex)
{
	m_iCurWeaponIndex = _iWeaponIndex;
}
void Nero::Change_BusterArm_Animation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pBusterArm.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
}
void Nero::Change_WireArm_Animation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pWireArm.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
}
void Nero::Change_WingArm_Left_Animation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pWingArm_Left.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
}
void Nero::Change_WingArm_Right_Animation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pWingArm_Right.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
}
//if (Input::GetKeyDown(DIK_LCONTROL))
//	m_iCurWeaponIndex = m_iCurWeaponIndex == RQ ? Cbs : RQ;