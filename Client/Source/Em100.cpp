#include "stdafx.h"
#include "Em100.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include <filesystem>

void Em100::Free()
{
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

	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);


	//몬스터 움직이는 방향 정해주는 놈
	int iRandom = FMath::Random<int>(1, 6);
	if (m_bMove && m_bIng == false)
	{
		m_bIng = true;
		//플레이어 방향으로 돌게 만듬
		m_bInteraction = true;
		Update_Angle();
		////////////////////////////

		if (iRandom == 1)
			m_eState = Walk_Left_Start;
		else if (iRandom == 2)
			m_eState = Walk_Right_Start;
		else
			m_eState = Walk_Front_Start;
	}
	if (m_bMove && m_bIng == true)
	{
		m_bInteraction = true;
		Update_Angle();
		//플레이어 앞으로 오면 움직임 멈춤.
		if (fDir <= 6.f && iRandom == 1)
			m_eState = Walk_Front_End;
	}
	//플레이어랑 어느정도 가까워 졌으면 공격.	
	if (fDir <= 6.f)
	{
		if (m_bAttack && m_bIng == false)
		{
			//체력이50% 이상일땐 Attack_A, Attack_D 둘중 하나 이거 두개는 그냥 넉백 히트
			//50%보다 떨어지면 Attack_Hard 추가해서 플레이어 넘어트리는 공격 추가
			int iRandom = FMath::Random<int>(1, 2);
			m_bIng = true;
			if (iRandom == 1)
				m_eState = Attack_A;
			else if (iRandom == 2)
				m_eState = Attack_D;
		}
	}
}

void Em100::State_Change(const float _fDeltaTime)
{

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
			m_pMesh->PlayAnimation("Attack_A", false, {}, 1.f, 50.f);
			{
				if (m_pMesh->CurPlayAnimInfo.Name == "Attack_A" && m_pMesh->PlayingTime() >= 0.9f)
				{
					m_eState = idle;
					m_bIng = false;
					m_bAttack = false;
				}
			}
		}
		break;
	case Em100::Attack_D:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_D", false, {}, 1.f, 50.f);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_D" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bAttack = false;
			}
		}
		break;
	case Em100::Attack_Hard:
		break;
	case Em100::Dead:
		break;
	case Em100::Hit_Air:
		break;
	case Em100::Hit_Back:
		break;
	case Em100::Hit_End:
		break;
	case Em100::Hit_Finish:
		break;
	case Em100::Hit_Front:
		break;
	case Em100::Hit_L:
		break;
	case Em100::Hit_R:
		break;
	case Em100::Walk_Front_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Front_End", false, {}, 1.f, 50.f);

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
			m_pMesh->PlayAnimation("Walk_Front_Loop", true, {}, 1.f, 50.f);
		break;
	case Em100::Walk_Front_Start:
		//m_bIng == 행동중이다. m_bInteraction == false면 다 돌았다. 다돌고 움직이자.
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Front_Start", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Front_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Front_Loop;
		}
		break;
	case Em100::Walk_Left_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Left_End", false, {}, 1.f, 50.f);

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
			m_pMesh->PlayAnimation("Walk_Left_Loop", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Left_Loop" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Left_End;
		}
		break;
	case Em100::Walk_Left_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Left_Start", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Left_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Left_Loop;
		}
		break;
	case Em100::Walk_Right_Stop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Right_Stop", false, {}, 1.f, 50.f);

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
			m_pMesh->PlayAnimation("Walk_Right_Loop", false, {}, 1.f, 50.f);
			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Right_Loop" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Right_Stop;
		}
		break;
	case Em100::Walk_Right_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Walk_Right_Start", false, {}, 1.f, 50.f);

			if (m_pMesh->CurPlayAnimInfo.Name == "Walk_Right_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Walk_Right_Loop;
		}
		break;
	case Em100::idle:
		m_pMesh->PlayAnimation("idle", true, {}, 1.f, 50.f);
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
	//GameObject를 받아오려면 각자 태그가 있어야함.
	m_nTag = Monster100;

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

HRESULT Em100::Awake()
{
	m_pPlayer = std::static_pointer_cast<TestObject>(FindGameObjectWithTag(Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();


	return S_OK;
}

HRESULT Em100::Start()
{
	return S_OK;
}

UINT Em100::Update(const float _fDeltaTime)
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

UINT Em100::LateUpdate(const float _fDeltaTime)
{

	return 0;

}

void Em100::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{

	}

}

void Em100::OnEnable()
{
}

void Em100::OnDisable()
{
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
	RenderInterface::Initialize(_InitRenderProp);
	Mesh::InitializeInfo _InitInfo{};
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em100\\Em100.fbx", _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
	//몬스터 초기상태 idle
	m_pMesh->PlayAnimation("idle", true);
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

void Em100::Update_Angle()
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
