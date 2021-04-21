#include "stdafx.h"
#include "Em100.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include <filesystem>
#include "Em100Hand.h"
#include "Nero.h"
#include "RedQueen.h"

void Em100::Free()
{
	Unit::Free();
}

std::string Em100::GetName()
{
	return "Em100";
}

Em100* Em100::Create()
{
	return new Em100{};
}


void Em100::Fight(const float _fDeltaTime)
{
	if (m_bMove == false)
	{
		m_fMoveTime += _fDeltaTime;
		if (m_fMoveTime >= 5.f)
		{
			m_bMove = true;
			m_fMoveTime = 0.f;
		}
	}
	if (m_bAttack == false)
	{
		m_fAttackTime += _fDeltaTime;
		if (m_fAttackTime >= 5.f)
		{
			m_bAttack = true;
			m_fAttackTime = 0.f;
		}
	}
	if (m_bHardAttack == false && m_BattleInfo.iHp <= 100)
	{
		m_fHardAttackTime += _fDeltaTime;
		if (m_fHardAttackTime >= 7.f)
		{
			m_bHardAttack = true;
			m_fHardAttackTime = 0.f;
		}
	}

	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);

	if (m_BattleInfo.iHp <= 0.f)
	{
		m_eState = Dead;
		m_bIng = true;
		return;
	}

	//몬스터 움직이는 방향 정해주는 놈
	if (fDir >= 3.f)
	{
		int iRandom = FMath::Random<int>(1, 6);
		if (m_bMove && m_bIng == false)
		{
			m_bIng = true;
			//플레이어 방향으로 돌게 만듬
			m_bInteraction = true;
			Update_Angle(_fDeltaTime);
			////////////////////////////

			if (iRandom == 1)
			{
				m_eState = Walk_Left_Start;
				return;
			}
			else if (iRandom == 2)
			{
				m_eState = Walk_Right_Start;
				return;
			}
			else
			{
				m_eState = Walk_Front_Start;
				return;
			}
		}
	}
	//플레이어랑 어느정도 가까워 졌으면 공격.	
	else
	{
		if (m_bHardAttack && m_bIng == false)
		{
			m_bIng = true;
			m_eState = Attack_Hard;
			return;
		}
		if (m_bAttack && m_bIng == false)
		{
			//체력이50% 이상일땐 Attack_A, Attack_D 둘중 하나 이거 두개는 그냥 넉백 히트
			//50%보다 떨어지면 Attack_Hard 추가해서 플레이어 넘어트리는 공격 추가
			int iRandom = FMath::Random<int>(1, 2);
			m_bIng = true;
			if (iRandom == 1)
			{
				m_eState = Attack_A;
				return;
			}
			else if (iRandom == 2)
			{
				m_eState = Attack_D;
				return;
			}
		}
	}
}

void Em100::State_Change(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);
	switch (m_eState)
	{
	case Em100::Air_End:
		break;
	case Em100::Air_Loop:
		break;
	case Em100::Air_Start:
		break;
	case Em100::Attack_A:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_A", false, {}, 1.f, 50.f, true);

			for (int i = 2; i < 0; ++i)
				m_pHand[i].lock()->Set_Coll(true);

			Update_Angle(_fDeltaTime);
			m_bInteraction = true;
			m_BattleInfo.eAttackType = Attack_Front;
			{
				if (m_pMesh->CurPlayAnimInfo.Name == "Attack_A" && m_pMesh->PlayingTime() >= 0.9f)
				{
					m_eState = idle;
					m_bIng = false;
					m_bAttack = false;

					for (int i = 2; i < 0; ++i)
						m_pHand[i].lock()->Set_Coll(false);
				}
			}
		}
		break;
	case Em100::Attack_D:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_D", false, {}, 1.f, 50.f, true);
			m_BattleInfo.eAttackType = Attack_Front;

			Update_Angle(_fDeltaTime);
			m_bInteraction = true;

			for (int i = 2; i < 0; ++i)
				m_pHand[i].lock()->Set_Coll(true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_D" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bAttack = false;

				for (int i = 2; i < 0; ++i)
					m_pHand[i].lock()->Set_Coll(false);
			}
		}
		break;
	case Em100::Attack_Hard:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Hard", false, {}, 1.f, 20.f, true);
			m_BattleInfo.eAttackType = Attack_KnocBack;

			Update_Angle(_fDeltaTime);
			m_bInteraction = true;
			
			for (int i = 2; i < 0; ++i)
				m_pHand[i].lock()->Set_Coll(true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bHardAttack = false;

				for (int i = 2; i < 0; ++i)
					m_pHand[i].lock()->Set_Coll(false);
			}
		}
		break;
	case Em100::Dead:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Death_Front", false, {}, 1.f, 20.f, true);
		}
		break;
	case Em100::Hit_Air:
		break;
	case Em100::Hit_Back:
		break;
	case Em100::Hit_Finish:
		break;
	case Em100::Hit_Front:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Front", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Front" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Hit_L:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_L", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_L" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Hit_R:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_R", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_R" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Hit_KnocBack:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Air", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = Hit_End;
				m_bHit = true;
				m_bIng = false;
			}
		}
		break;
	case Em100::Walk_Front_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Front_End", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Front_End" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bMove = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Walk_Front_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Front_Loop", true, {}, 1.f, 50.f, true);
			Update_Angle(_fDeltaTime);
			m_bInteraction = true;

			if (fDir <= 3.f)
			{
				m_eState = Walk_Front_End;
			}
		}
		break;
	case Em100::Walk_Front_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Front_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Front_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Front_Loop;
		}
		break;
	case Em100::Walk_Left_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Left_End", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Left_End" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bMove = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Walk_Left_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Left_Loop", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Left_Loop" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Left_End;
		}
		break;
	case Em100::Walk_Left_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Left_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Left_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Left_Loop;
		}
		break;
	case Em100::Walk_Right_Stop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Right_Stop", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Right_Stop" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bMove = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Walk_Right_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Right_Loop", false, {}, 1.f, 50.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Right_Loop" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Right_Stop;
		}
		break;
	case Em100::Walk_Right_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Right_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Right_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Right_Loop;
		}
		break;
	case Em100::idle:
		m_pMesh->PlayAnimation("idle", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		break;
	default:
		break;
	}


}

void Em100::Skill_CoolTime(const float _fDeltaTime)
{
}

HRESULT Em100::Ready()
{
	Unit::Ready();
	//GameObject를 받아오려면 각자 태그가 있어야함.
	m_nTag = Monster100;

	m_BattleInfo.iMaxHp = 200;
	m_BattleInfo.iHp = 200;
	m_BattleInfo.iAttack = 20;

	m_pTransform.lock()->SetPosition({ 5.f, 5.f, 0.f });
		
	RenderInit();
	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01,0.01,0.01 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());


	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);
	return S_OK;
}

HRESULT Em100::Awake()
{
	Unit::Awake();

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	for (int i = 0; i < 2; ++i)
	{
		m_pHand[i] = AddGameObject<Em100Hand>();
		m_pHand[i].lock()->m_pEm100 = static_pointer_cast<Em100>(m_pGameObject.lock());
		m_pHand[i].lock()->m_pEm100Mesh = m_pMesh;
		m_pHand[i].lock()->m_bLeft = (bool)i;
	}
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(false);

	//m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRadius(1.1f);
	m_pCollider.lock()->SetHeight(1.5f);
	m_pCollider.lock()->SetCenter({ 0.f, 1.5f, 0.f });

	//m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	//m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();
	//m_pRedQueen = std::static_pointer_cast<RedQueen>(FindGameObjectWithTag(GAMEOBJECTTAG::TAG_RedQueen).lock());

	return S_OK;
}

HRESULT Em100::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em100::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	// 현재 스케일과 회전은 의미가 없음 DeltaPos 로 트랜스폼에서 통제 . 
	auto [DeltaScale, DeltaQuat, DeltaPos] = m_pMesh->Update(_fDeltaTime);
	Vector3 Axis = { 1,0,0 };
	  
	//ENGINE::AnimNotify _Notify{};
	////return true 면 이제 호출 안함, false면 저 루프 돌떄 계속 호출.
	//_Notify.Event[0.5] = [this]() {  AttackStart();  return false; };

	const float Length = FMath::Length(DeltaPos);
	D3DXMATRIX matRot;
	D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
	D3DXMatrixRotationQuaternion(&matRot, &tQuat);

	D3DXVec3TransformNormal(&DeltaPos, &DeltaPos, &matRot);
	

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(SpTransform->GetPosition() + DeltaPos * SpTransform->GetScale().x);
	}

	Rotate(_fDeltaTime);

	if (m_bHit)
		m_bIng = true;



	if (Input::GetKeyDown(DIK_T))
	{
		if (m_bTest == true)
			m_bTest = false;
		else
			m_bTest = true;
	}

	if (m_bTest == true)
	{
		Fight(_fDeltaTime);
		State_Change(_fDeltaTime);
	}
	if (Input::GetKeyDown(DIK_Y))
		m_BattleInfo.iHp -= 10;

	cout << m_BattleInfo.iHp << endl;

	return 0;
}

UINT Em100::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em100::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		ImGui::Text("Deg %3.4f", m_fRadian);
		ImGui::Text("Acc Deg %3.4f", m_fAccuangle);
	}
}

void Em100::OnEnable()
{
	Unit::OnEnable();
}

void Em100::OnDisable()
{
	Unit::OnDisable();
}

void Em100::Hit(BT_INFO _BattleInfo, void* pArg)
{
	m_BattleInfo.iHp -= _BattleInfo.iAttack;
	
	switch (_BattleInfo.eAttackType)
	{
	case ATTACKTYPE::Attack_L:
		m_eState = Hit_L;
		m_bHit = true;
		break;
	case ATTACKTYPE::Attack_R:
		m_eState = Hit_R;
		m_bHit = true;
		break;
	case ATTACKTYPE::Attack_Front:
		m_eState = Hit_Front;
		m_bHit = true;
		break;
	case ATTACKTYPE::Attack_KnocBack:
		m_eState = Hit_KnocBack;
		m_bHit = true;
		break;
	case ATTACKTYPE::Attack_Back:
		break;
	case ATTACKTYPE::Attack_END:
		break;
	default:
		m_bIng = true;
		break;
	}
	
	
}

void Em100::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (m_bCollEnable == true)
	{
		//레드퀸, 버스터암, 건틀릿들, 와이어암
		switch (_pOther.lock()->m_nTag)	
		{
		case GAMEOBJECTTAG::TAG_RedQueen:
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
			break;
		default:
			break;
		}
		m_bCollEnable = false;
	}
}

void Em100::RenderGBufferSK(const DrawInfo& _Info)
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

void Em100::RenderShadowSK(const DrawInfo& _Info)
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

void Em100::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em100::RenderDebugSK(const DrawInfo& _Info)
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

void Em100::RenderInit()
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
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em100\\Em100.fbx", _InitInfo);

	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em100\\Ani");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
	//몬스터 초기상태 idle
	m_pMesh->PlayAnimation("Idle", true);
}

void Em100::Rotate(const float _fDeltaTime)
{
	if (m_bInteraction == false)
		return;

	if (fabs(m_fRadian) <= fabs(m_fAccuangle))
	{
		float fAdd = m_fRadian - m_fAccuangle;

		m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(fAdd), 0.f });

		m_bInteraction = false;
		return;
	}
	m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fAngleSpeed * _fDeltaTime), 0.f });

	m_fAccuangle += m_fAngleSpeed * _fDeltaTime;
}

void Em100::Update_Angle(const float _fDeltaTime, bool _bTest)
{

	Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir = vPlayerPos - vMyPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);

	Vector3 vLook = -m_pTransform.lock()->GetLook();

	float fDot = D3DXVec3Dot(&vDir, &vLook);
	float fRadian = acosf(fDot);

	Vector3	vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);

	if (vCross.y > 0)
		fRadian *= -1;

	m_fRadian = fRadian;
	m_fAccuangle = 0.f;

	if (D3DXToDegree(m_fRadian) > -2.f && D3DXToDegree(m_fRadian) < 2.f)
		m_fRadian = 0.f;

	if (m_fRadian > 0)
		m_fAngleSpeed = fabs(m_fAngleSpeed);
	else
		m_fAngleSpeed = -fabs(m_fAngleSpeed);

}

void Em100::Update_Angle()
{
}
