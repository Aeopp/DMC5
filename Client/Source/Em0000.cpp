#include "stdafx.h"
#include "Em0000.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include <filesystem>

#include "Em0000_Weapon.h"

void Em0000::Free()
{
}

std::string Em0000::GetName()
{
	return "Em0000";
}

Em0000* Em0000::Create()
{
	return new Em0000{};
}




void Em0000::Fight(const float _fDeltaTime)
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
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);
	
	/////이놈은 움직임이 한방향 밖에 없어서. 앞으로 갈지 아니면 백스탭 이후에 강한 공격할지
	//Move가 On일때 1/4 확률로 백스텝을 하고 백스텝을 한 이후에는 바로 달려가면서 강한 공격
	int iRandom = FMath::Random<int>(1, 4);

	if (m_bMove && m_bIng == false)
	{
		m_bIng = true;	
		m_bInteraction = true;
		Update_Angle();
		///////////////////////
		if (iRandom == 1)
			m_eState = Step_Back;
		else
			m_eState = Move_Front_Start;
		//////////////////////
	}
	if (m_bMove && m_bIng == true)
	{
		m_bInteraction = true;
		Update_Angle();

		if (fDir <= 6.f)
			m_eState = Move_Front_End;
	}

	if (fDir <= 6.f)
	{
		if (m_bAttack && m_bIng == false)
		{
			int iRandom = FMath::Random<int>(1, 2);
			m_bIng = true;
			if (iRandom == 1)
				m_eState = Attack_1;
			else
				m_eState = Attack_2;
		}
	}

}

void Em0000::State_Change(const float _fDeltaTime)
{
	switch (m_eState)
	{
	case Em0000::Attack_1:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_1", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_1" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bAttack = false;
			}
		}
		break;
	case Em0000::Attack_2:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_2", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_2" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bAttack = false;
			}
		}
		break;
	case Em0000::Attack_Hard:
		if (m_bIng == true)
		{
			//이건 마지막 피니시 모션까지 계속 플레이어를 향해서 뜀.
			m_bInteraction = true;
			Update_Angle();
			m_pMesh->PlayAnimation("Attack_Hard", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->PlayingTime() >= 0.95f)
			{
				m_bIng = false;
				m_bAttack = false;
				m_eState = idle;
			}
		}
		break;
	case Em0000::Buster_End:
		break;
	case Em0000::Buster_Start:
		break;
	case Em0000::Dead:
		break;
	case Em0000::Guard_End:
		break;
	case Em0000::Guard_Hit1:
		break;
	case Em0000::Guard_Hit2:
		break;
	case Em0000::Guard_Loop:
		break;
	case Em0000::Guard_Start:
		break;
	case Em0000::Hit_Back:
		break;
	case Em0000::Hit_Front:
		break;
	case Em0000::Hit_L:
		break;
	case Em0000::Hit_R:
		break;
	case Em0000::Move_Front_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Front_End", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Front_End" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bMove = false;
			}
		}
		break;
	case Em0000::Move_Front_Loop:
		if (m_bIng == true)
			m_pMesh->PlayAnimation("Move_Front_Loop", true, {}, 1.f, 50.f);
		break;
	case Em0000::Move_Front_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Front_Start", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Front_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Move_Front_Loop;
		}
		break;
	case Em0000::Step_Back:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Step_Back", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Step_Back" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = Attack_Hard;
				m_bMove = false;
			}

		}
		break;
	case Em0000::Stun_End:
		break;
	case Em0000::Stun_Start:
		break;
	case Em0000::idle:
		m_pMesh->PlayAnimation("idle", true, {}, 1.f, 50.f);
		break;
	case Em0000::Snatch_Start:
		break;
	case Em0000::Snatch_End:
		break;
	case Em0000::Air_Start:
		break;
	case Em0000::Air_Loop:
		break;
	case Em0000::Air_End:
		break;
	case Em0000::State_END:
		break;
	default:
		break;
	}
}

void Em0000::Skill_CoolTime(const float _fDeltaTime)
{
}

HRESULT Em0000::Ready()
{
	m_nTag = Monster0000;

	RenderInit();

// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());


	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);

	return S_OK;
}

HRESULT Em0000::Awake()
{
	m_pPlayer = std::static_pointer_cast<TestObject>(FindGameObjectWithTag(Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();

	auto pWeapon = AddGameObject<Em0000Weapon>();

	pWeapon.lock()->SetMesh(m_pMesh);
	pWeapon.lock()->SetOwner(std::static_pointer_cast<Em0000>(m_pGameObject.lock()));

	return S_OK;
}

HRESULT Em0000::Start()
{
	return S_OK;
}

UINT Em0000::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
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
	//DeltaPos = FMath::RotationVecNormal(DeltaPos, Axis, FMath::ToRadian(90.f)) * Length;

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(SpTransform->GetPosition() + DeltaPos * SpTransform->GetScale().x);
	}
	//플레이어가 사라졌는지 판단
	/*if (false == m_pPlayer.expired())
	{
		std::cout << "Player Dead" << std::endl;
	}*/


	

	if (Input::GetKeyDown(DIK_SPACE))
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

	/*if (Input::GetKeyDown(DIK_T))
		Update_Angle();

	if (Input::GetKeyDown(DIK_Y))
	{
		if (m_bInteraction == true)
			m_bInteraction = false;
		else
			m_bInteraction = true;
	}*/


	Rotate(_fDeltaTime);


	return 0;
}

UINT Em0000::LateUpdate(const float _fDeltaTime)
{
	
	return 0;

}

void Em0000::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{

	}

}

void Em0000::OnEnable()
{
}

void Em0000::OnDisable()
{
}

void Em0000::RenderGBufferSK(const DrawInfo& _Info)
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

void Em0000::RenderShadowSK(const DrawInfo& _Info)
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

void Em0000::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em0000::RenderDebugSK(const DrawInfo& _Info)
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

void Em0000::RenderInit()
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
	RenderInterface::Initialize(_InitRenderProp);
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em0000\\Em0000.fbx", _InitInfo);
	PushEditEntity(m_pMesh.get());
	m_pMesh->EnableToRootMatricies();
	//몬스터 초기상태 idle
	m_pMesh->PlayAnimation("idle", true);
}

void Em0000::Rotate(const float _fDeltaTime)
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

void Em0000::Update_Angle()
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

	if (m_fRadian > 0)
		m_fAngleSpeed = fabs(m_fAngleSpeed);
	else
		m_fAngleSpeed = -fabs(m_fAngleSpeed);
}
