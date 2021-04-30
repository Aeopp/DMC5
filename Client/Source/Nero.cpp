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
#include "BtlPanel.h"
#include "GT_Overture.h"
#include "GT_Rockman.h"
#include "Monster.h"
#include "OvertureHand.h"
#include "Liquid.h"
#include "Cbs_Short.h"
#include "Cbs_Middle.h"
#include "Cbs_Long.h"
Nero::Nero()
	:m_iCurAnimationIndex(ANI_END)
	, m_iPreAnimationIndex(ANI_END)
	, m_iCurWeaponIndex(NeroCom_Cbs_Short)
	, m_iJumpDirIndex(Basic)
	, m_fRedQueenGage(0.f)
	, m_iCurDirIndex(Dir_Front)
	, m_iPreDirIndex(Dir_Front)
	, m_IsMajin(false)
{
	m_nTag = Player;
	m_BattleInfo.iMaxHp = 100;
	m_BattleInfo.iHp = 100;
}
void Nero::Free()
{
	Unit::Free();
	m_pFSM = nullptr;
}

void Nero::Set_PlayingTime(float NewTime)
{
	m_pMesh->SetPlayingTime(NewTime);
}


void Nero::Set_Weapon_Coll(NeroComponentID _eNeroComID, bool _ActiveOrNot)
{
	if (_ActiveOrNot)
	{
		std::list<std::weak_ptr<Monster>> Monsters = GetAllMonster();
		for (auto& pMonster : Monsters)
			pMonster.lock()->Set_Coll(true);
	}
	switch (_eNeroComID)
	{
	case Nero::NeroCom_RedQueen:
		m_pRedQueen.lock()->GetComponent<CapsuleCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pCbsShort.lock()->GetComponent<SphereCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_Cbs_Middle:
		break;
	case Nero::NeroCom_Cbs_Long:
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->GetComponent<SphereCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_BusterArm:
		m_pBusterArm.lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_WireArm:
		m_pWireArm.lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::Set_Weapon_AttType(NeroComponentID _eNeroComID, ATTACKTYPE _eAttDir)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_RedQueen:
		m_pRedQueen.lock()->SetAttType(_eAttDir);
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->Set_AttackType(_eAttDir);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pCbsShort.lock()->SetAttType(_eAttDir);
		break;
	case Nero::NeroCom_Cbs_Middle:
		break;
	case Nero::NeroCom_Cbs_Long:
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::Set_Weapon_State(NeroComponentID _eNeroComID, UINT _StateIndex)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_RedQueen:
		m_pRedQueen.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pCbsShort.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_Cbs_Middle:
		break;
	case Nero::NeroCom_Cbs_Long:
		break;
	case Nero::NeroCom_All_Weapon:
		m_pRedQueen.lock()->SetWeaponState(_StateIndex);
		m_pCbsShort.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::CreateOvertureEff(EffDircetion eDir)
{
	m_pEffOverture.lock()->PlayStart();
	Matrix World = m_pTransform.lock()->GetWorldMatrix();
	Vector3 Pos,Rot;
	memcpy(&Pos, World.m[3], sizeof(Vector3));
	switch (eDir)
	{
	case EffDircetion::EffDir_Front:
		Pos += (m_pTransform.lock()->GetLook()) * -0.25f;
		Pos.y += 0.08f;
		Rot = { 0.f,0.f,0.f };
		break;
	case EffDircetion::EffDir_Up:
		Rot = { 90.f,180.f,0.f };
		Pos += (m_pTransform.lock()->GetLook()) * -0.08f;
		Pos.y += 0.23f;
		break;
	case EffDircetion::EffDir_Down:
		Rot = { -90.f,0.f,0.f };
		break;
	default:
		break;
	}
	Vector3 MyRotation = (Transform::QuaternionToEuler(m_pTransform.lock()->GetQuaternion()));
	
	m_pEffOverture.lock()->SetRotation(MyRotation + Rot);
	m_pEffOverture.lock()->SetPosition(Pos);
}

std::list<std::weak_ptr<Monster>> Nero::GetAllMonster()
{
	return FindGameObjectsWithType<Monster>();
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
	Unit::Ready();
	RenderInit();

	m_pTransform.lock()->SetScale({ 0.001f,0.001f,0.001f });
	m_pTransform.lock()->SetPosition(Vector3{-4.8f, 3.f, -5.02f});

	PushEditEntity(m_pTransform.lock().get());

	m_pRedQueen = AddGameObject<RedQueen>();
	m_pLWing = AddGameObject<Nero_LWing>();
	m_pRWing = AddGameObject<Nero_RWing>();
	m_pBusterArm = AddGameObject<Buster_Arm>();
	m_pWireArm = AddGameObject<Wire_Arm>();
	m_pWingArm_Left = AddGameObject <WIngArm_Left>();
	m_pWingArm_Right = AddGameObject<WingArm_Right>();
	m_pOverture = AddGameObject<GT_Overture>();
	m_pEffOverture = AddGameObject<OvertureHand>();
	m_pCbsShort = AddGameObject<Cbs_Short>();
	m_pCbsMiddle = AddGameObject<Cbs_Middle>();
	m_pCbsLong = AddGameObject<Cbs_Long>();
	//m_pRockman = AddGameObject<GT_Rockman>();
	m_pBlood = AddGameObject<Liquid>();
	m_pBlood.lock()->SetScale(0.007f);
	m_pBlood.lock()->SetVariationIdx(Liquid::BLOOD_0);

	m_pFSM.reset(NeroFSM::Create(static_pointer_cast<Nero>(m_pGameObject.lock())));

	m_iCurAnimationIndex = ANI_END;
	m_iPreAnimationIndex = ANI_END;

	return S_OK;
}


HRESULT Nero::Awake()
{
	Unit::Awake();
	m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);
	m_pCollider.lock()->SetCenter(D3DXVECTOR3(0.f, 0.08f, 0.f));
	m_pCollider.lock()->SetRadius(0.04f);
	m_pCollider.lock()->SetHeight(0.1f);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);


	PushEditEntity(m_pCollider.lock().get());

	vDegree = D3DXVECTOR3(0.f, 0.f, 0.f);
	vRotationDegree = D3DXVECTOR3(0.f, 0.f, 0.f);
	vAccumlatonDegree = D3DXVECTOR3(0.f, 0.f, 0.f);

	return S_OK;
}

HRESULT Nero::Start()
{
	Unit::Start();
	m_pCamera = std::static_pointer_cast<MainCamera>(FindGameObjectWithTag(TAG_Camera).lock());
	m_pBtlPanel = std::static_pointer_cast<BtlPanel>(FindGameObjectWithTag(UI_BtlPanel).lock());

	return S_OK;
}

UINT Nero::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	Update_Majin(_fDeltaTime);


	//if (Input::GetKeyDown(DIK_0))
	//	Hit(m_BattleInfo);
	
	if (nullptr != m_pFSM && m_bDebugButton)
		m_pFSM->UpdateFSM(_fDeltaTime);

	auto [Scale,Rot,Pos] =m_pMesh->Update(_fDeltaTime);
	Pos.y = 0.f;

	vAccumlatonDegree += Transform::QuaternionToEuler(Rot);

	m_pTransform.lock()->SetRotation(vDegree + vRotationDegree + vAccumlatonDegree);

	D3DXMATRIX matRot;
	D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
	D3DXMatrixRotationQuaternion(&matRot, &tQuat);
	D3DXVec3TransformCoord(&Pos, &Pos, &matRot);

	m_pTransform.lock()->Translate(Pos * m_pTransform.lock()->GetScale().x);

	//테스트
	//if(!m_IsMajin)
		m_pBtlPanel.lock()->AccumulateTDTGauge(0.0005f);
	return 0;
}

UINT Nero::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Nero::OnEnable()
{
	Unit::OnEnable();
}

void Nero::OnDisable()
{
	Unit::OnDisable();
}

void Nero::Hit(BT_INFO _BattleInfo, void* pArg)
{
	m_pBlood.lock()->SetPosition(Get_NeroBoneWorldPos("Waist"));
	m_pBlood.lock()->PlayStart(40.f);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;
	float fHpRatio = float(float(m_BattleInfo.iHp) / float(m_BattleInfo.iMaxHp));
	m_pBtlPanel.lock()->SetPlayerHPRatio(fHpRatio);
	switch (_BattleInfo.eAttackType)
	{
	case Attack_Front:
		m_pFSM->ChangeState(NeroFSM::HIT_FRONT);
		break;
	//case Attack_Down:
	//	m_pFSM->ChangeState(NeroFSM::HIT_FRONT);
	//	break;
	//case Attack_Stun:
	//	m_pFSM->ChangeState(NeroFSM::HIT_FRONT);
	//	break;
	//case Attack_KnocBack:
	//	m_pFSM->ChangeState(NeroFSM::HIT_FRONT);
	//	break;
	//case Attack_END:
	//	break;
	default:
		m_pFSM->ChangeState(NeroFSM::HIT_FRONT);
		break;
	}
}

void Nero::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	GAMEOBJECTTAG eTag = GAMEOBJECTTAG(_pOther.lock()->m_nTag);
	switch (eTag)
	{
	case MonsterWeapon:
		if (!static_pointer_cast<Unit>(_pOther.lock())->Get_Coll())
			return;
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		static_pointer_cast<Unit>(_pOther.lock())->Set_Coll(false);
		break;
	default:
		break;
	}
}

void Nero::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
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
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
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
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
    // ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
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

	// ���̷��� �޽� �ε� ... 
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Player.fbx", _InitInfo);

	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Animation");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Player.Animation");

	m_pMesh->EnableToRootMatricies();

	PushEditEntity(m_pMesh.get());
}

void Nero::Update_Majin(float _fDeltaTime)
{
	if (m_IsMajin)
	{
		m_pBtlPanel.lock()->ConsumeTDTGauge(_fDeltaTime);
	}

	if (m_pBtlPanel.lock()->GetTDTGauge() <= 0.f)
	{
		m_IsMajin = false;
		SetActive_NeroComponent(NeroCom_Wings, false);
		SetActive_NeroComponent(NeroCom_WIngArm_Left, false);
		SetActive_NeroComponent(NeroCom_WingArm_Right, false);
	}

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
	Unit::Editor();
	bool MyButton = true;
	float ZeroDotOne = 0.1f;
	if (bEdit)
	{
		// ������ .... 
		ImGui::Checkbox("KeyInput", &m_bDebugButton);
		if (ImGui::Button("ResetAnimation"))
		{
			ChangeAnimation("Idle_Battle", true, ANI_IDLE_BATTLE);
		}
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

Matrix Nero::Get_NeroBoneWorldMatrix(std::string _BoneName)
{
	return *m_pMesh->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix();
}

Vector3 Nero::Get_NeroBoneWorldPos(std::string _BoneName)
{
	Vector3 WorldPos;
	memcpy(&WorldPos, (*m_pMesh->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix()).m[3], sizeof(Vector3));
	return WorldPos;
}


void Nero::SetActive_NeroComponent(NeroComponentID _eNeroComID, bool ActiveOrNot)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_Wings:
		m_pLWing.lock()->SetActive(ActiveOrNot);
		m_pRWing.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_LWing:
		m_pLWing.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_RWing:
		m_pRWing.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_BusterArm:
		m_pBusterArm.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_WireArm:
		m_pWireArm.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_WIngArm_Left:
		m_pWingArm_Left.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_WingArm_Right:
		m_pWingArm_Right.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::SetAngleFromCamera(float _fAddAngle)
{
	if (m_pCamera.expired())
		return;

	m_fAngle = m_pCamera.lock()->Get_Angle(_fAddAngle);
	vDegree.y = m_fAngle;
	vRotationDegree.y = m_fRotationAngle;


}

void Nero::SetAddForce(Vector3 _vJumpPos)
{
	m_pCollider.lock()->AddForce(_vJumpPos);
}

void Nero::SetAddForce_Dir(NeroDirection _eDir,float _fPower)
{
	switch (_eDir)
	{
	case Nero::Dir_Front:
		m_pCollider.lock()->AddForce(m_pTransform.lock()->GetLook() * -1.f * _fPower);
		break;
	case Nero::Dir_Back:
		break;
	case Nero::Dir_Left:
		break;
	case Nero::Dir_Right:
		break;
	case Nero::Dir_Front_Down:
	{
		Vector3 vFrontDownDir = m_pTransform.lock()->GetLook() * -1.f;
		vFrontDownDir.y = -0.5f;
		m_pCollider.lock()->AddForce(vFrontDownDir * _fPower);
	}
		break;
	default:
		break;
	}
}

void Nero::SetLockOnMonster()
{
	if (!m_pTargetMonster.expired())
		return;

	std::list<std::weak_ptr<Monster>> MonsterList = FindGameObjectsWithType<Monster>();
	if (0 == MonsterList.size())
		return;

	Vector3 Dir = MonsterList.begin()->lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float Distance = D3DXVec3Length(&Dir);
	//여기서 조건 검사해야됨 너무 멀면 찾지도못하게
	m_pTargetMonster = MonsterList.begin()->lock();

	for (auto& pMonster : MonsterList)
	{
		Vector3 Direction = pMonster.lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
		float Temp = D3DXVec3Length(&Direction);
		//여기서 조건 검사해야됨 너무 멀면 찾지도못하게
		if (Distance >= Temp)
		{
			Distance = Temp;
			m_pTargetMonster = pMonster;
		}
	}
	m_pBtlPanel.lock()->SetTargetCursorActive(true);
	//RotateToTargetMonster();
}

void Nero::SetOffLockOnMonster()
{
	m_pBtlPanel.lock()->SetTargetCursorActive(false);
	m_pTargetMonster.reset();
}

void Nero::SetDashLoopDir()
{
	m_iDashLoopDir = m_iDashLoopDir == 1 ? -1 : 1;
}

void Nero::SetLinearVelocity(const D3DXVECTOR3 _vLinearVelocity)
{
	if (m_pCollider.expired())
		return;

	m_pCollider.lock()->SetLinearVelocity(_vLinearVelocity);
}

void Nero::Set_GrabEnd(bool _bGrabEnd)
{
	m_pWireArm.lock()->Set_GrabEnd(_bGrabEnd);
}

void Nero::SetCbsIdle()
{
	m_pCbsLong.lock()->SetActive(false);
	m_pCbsMiddle.lock()->SetActive(false);
	m_pCbsShort.lock()->SetActive(true);
	m_pCbsShort.lock()->SetWeaponState(Nero::WS_Idle);
	m_pCbsShort.lock()->ChangeAnimation("Cbs_Idle", true);
}

void Nero::CheckAutoRotate()
{
	std::list<std::weak_ptr<Monster>> MonsterList = FindGameObjectsWithType<Monster>();
	std::weak_ptr<Monster> LockOnMonster;
	if (0 == MonsterList.size())
		return;

	Vector3 Dir = MonsterList.begin()->lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float Distance = D3DXVec3Length(&Dir);
	LockOnMonster = MonsterList.begin()->lock();

	for (auto& pMonster : MonsterList)
	{
		Vector3 Direction = pMonster.lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
		float Temp = D3DXVec3Length(&Direction);
		if (Distance >= Temp)
		{
			Distance = Temp;
			LockOnMonster = pMonster;
		}
	}

	if (Distance <= RotateDistance)
	{
		Vector3 vMonsterPos = LockOnMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
		Vector3 vMyPos = m_pTransform.lock()->GetPosition();

		Vector3 vDir = vMonsterPos - vMyPos;
		vDir.y = 0;
		D3DXVec3Normalize(&vDir, &vDir);

		Vector3 vLook = -m_pTransform.lock()->GetLook();

		float fDot = D3DXVec3Dot(&vDir, &vLook);
		float fRadian = acosf(fDot);
		
		Vector3	vCross;
		D3DXVec3Cross(&vCross, &vLook, &vDir);

		if (vCross.y > 0)
			fRadian *= -1;
		m_fAngle += -D3DXToDegree(fRadian) + vRotationDegree.y;
		vDegree.y = m_fAngle;
		vRotationDegree.y = m_fRotationAngle = 0;
		Reset_RootRotation();
	}

}

bool Nero::CheckIsGround()
{
	return m_pCollider.lock()->IsGround();
}

void Nero::Locking()
{
	SetLockOnMonster();
	if (m_pTargetMonster.expired())
		return;
	float fHpRatio = float(float(m_pTargetMonster.lock()->Get_BattleInfo().iHp) / float(m_pTargetMonster.lock()->Get_BattleInfo().iMaxHp));
	m_pBtlPanel.lock()->SetTargetCursor(
		m_pTargetMonster.lock()->GetMonsterBoneWorldPos("Waist"),
		fHpRatio);
	
	
}

Nero::NeroDirection Nero::RotateToTargetMonster()
{
	if (m_pTargetMonster.expired())
		return NeroDirection::Dir_End;
	Vector3 vMonsterPos = m_pTargetMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir, vNewDir;
	vDir = vNewDir = vMonsterPos - vMyPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vNewDir, &vNewDir);

	Vector3 vLook = -m_pTransform.lock()->GetLook();

	float fDot = D3DXVec3Dot(&vDir, &vLook);
	float fRadian = acosf(fDot);

	Vector3	vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);

	if (vCross.y > 0)
		fRadian *= -1;
	m_fAngle += -D3DXToDegree(fRadian) + vRotationDegree.y;
	vDegree.y = m_fAngle;
	vRotationDegree.y = m_fRotationAngle = 0;
	Reset_RootRotation();

	m_pTransform.lock()->SetRotation(vDegree + vRotationDegree + vAccumlatonDegree);
	//와이어팔 회전할 각도 구해주는곳
	
	vLook = -m_pTransform.lock()->GetLook();
	fDot = D3DXVec3Dot(&vNewDir, &vLook);
	fRadian = acosf(fDot);
	float fDegree = D3DXToDegree(fRadian);
	if (85.f <= fDegree || fDegree <= -85.f)
	{
		m_pWireArm.lock()->Set_RadianForRotX(0.f);
	}
	else
	{
		if (vNewDir.y < 0.f)
			fRadian *= -1.f;
		m_pWireArm.lock()->Set_RadianForRotX(fRadian);
	}

	if (fDegree > 5.f)
	{
		return NeroDirection::Dir_Down;
	}
	else if (fDegree < -5.f)
	{
		return  NeroDirection::Dir_Up;
	}
	return NeroDirection::Dir_Front;
}

void Nero::NeroMove(NeroDirection _eDir, float _fPower)
{
	switch (_eDir)
	{
	case Nero::Dir_Front:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetLook() * -1.f * _fPower);
		break;
	case Nero::Dir_Back:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetLook() * _fPower);
		break;
	case Nero::Dir_Left:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetRight() * -1.f * _fPower);
		break;
	case Nero::Dir_Right:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetRight() * _fPower);
		break;
	case Nero::Dir_Front_Down:
		break;
	default:
		break;
	}
}

void Nero::DecreaseDistance(float _GoalDis, float _fDeltaTime)
{
	if (m_pCamera.expired())
		return;
	m_pCamera.lock()->DecreaseDistance(_GoalDis, _fDeltaTime);
}

void Nero::IncreaseDistance(float _GoalDis, float _fDeltaTime)
{
	if (m_pCamera.expired())
		return;
	m_pCamera.lock()->IncreaseDistance(_GoalDis, _fDeltaTime);
}

float Nero::Get_ExGauge()
{
	return m_pBtlPanel.lock()->GetExGauge();
}

uint32 Nero::Get_ExGaugeCount()
{
	return m_pBtlPanel.lock()->GetExGaugeCount();
}

void Nero::Add_ExGauge(float ExGauge)
{
	m_pBtlPanel.lock()->AddExGauge(ExGauge);
}

void Nero::Use_ExGauge(const uint32 Count)
{
	m_pBtlPanel.lock()->UseExGauge(Count);
}

float Nero::Get_TDTGauge()
{
	return m_pBtlPanel.lock()->GetTDTGauge();
}

void Nero::AccumulateTDTGauge(const float Amount)
{
	m_pBtlPanel.lock()->AccumulateTDTGauge(Amount);
}

void Nero::ConsumeTDTGauge(const float Speed)
{
	m_pBtlPanel.lock()->ConsumeTDTGauge(Speed);
}

void Nero::AddRankScore(float Score)
{
	m_pBtlPanel.lock()->AddRankScore(Score);
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

void Nero::ChangeNeroDirection(UINT _NeroDirection)
{
	m_iPreDirIndex = m_iCurDirIndex;
	m_iCurDirIndex = _NeroDirection;
}

void Nero::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const UINT AnimationIndex, const AnimNotify& _Notify)
{
	m_iPreAnimationIndex = m_iCurAnimationIndex;
	m_iCurAnimationIndex = AnimationIndex;
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

void Nero::ChangeAnimation_Weapon(NeroComponentID _eNeroComID, const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify, const bool bOverlap)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_BusterArm:
		m_pBusterArm.lock()->SetActive(true);
		m_pBusterArm.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_WireArm:
		m_pWireArm.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_WIngArm_Left:
		m_pWingArm_Left.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_WingArm_Right:
		m_pWingArm_Right.lock()->SetActive(true);
		m_pWingArm_Right.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_RedQueen:
		break;
	case Nero::NeroCom_Cbs_Short:
		ChangeWeapon(Nero::NeroCom_Cbs_Short);
		m_pCbsShort.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify, bOverlap);
		break;
	case Nero::NeroCom_Cbs_Middle:
		ChangeWeapon(Nero::NeroCom_Cbs_Middle);
		m_pCbsMiddle.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify, bOverlap);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::ChangeWeapon(NeroComponentID _iWeaponIndex)
{
	m_iCurWeaponIndex = _iWeaponIndex;

	switch (m_iCurWeaponIndex)
	{
	case Nero::NeroCom_RedQueen:
		m_pRedQueen.lock()->SetActive(true);
		m_pCbsShort.lock()->SetActive(false);
		m_pCbsMiddle.lock()->SetActive(false);
		m_pCbsLong.lock()->SetActive(false);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pRedQueen.lock()->SetActive(false);
		m_pCbsShort.lock()->SetActive(true);
		m_pCbsMiddle.lock()->SetActive(false);
		m_pCbsLong.lock()->SetActive(false);
		break;
	case Nero::NeroCom_Cbs_Middle:
		m_pRedQueen.lock()->SetActive(false);
		m_pCbsShort.lock()->SetActive(false);
		m_pCbsMiddle.lock()->SetActive(true);
		m_pCbsLong.lock()->SetActive(false);
		break;
	case Nero::NeroCom_Cbs_Long:
		m_pRedQueen.lock()->SetActive(false);
		m_pCbsShort.lock()->SetActive(false);
		m_pCbsMiddle.lock()->SetActive(false);
		m_pCbsLong.lock()->SetActive(true);
		break;
	default:
		break;
	}
}
