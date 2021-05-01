#include "stdafx.h"
#include "Em1000.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include <filesystem>
#include "Nero.h"
#include "RedQueen.h"
#include "NeroFSM.h"
#include "Em1000Hand.h"

void Em1000::Free()
{
	Unit::Free();
}

std::string Em1000::GetName()
{
	return "Em1000";
}

Em1000* Em1000::Create()
{
	return new Em1000{};
}


void Em1000::Fight(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);

	if (fDir <= 0.6f)
	{
		if (m_bAttack && m_bIng == false)
		{
			m_bIng = true;
			m_eState = Attack_Ready_Floor;
			m_pHand.lock()->Set_Coll(true);
		}
	}
}

void Em1000::State_Change(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float fDot = D3DXVec3Dot(&vDir, &vLook);


	switch (m_eState)
	{
	case Em1000::Idle_Floor:
		m_pMesh->PlayAnimation("Idle_Floor", true, {}, 1.f, 20.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		break;
	case Em1000::Idle_Wall:
		break;
	case Em1000::Attack_Ready_Floor:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Ready_Floor", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Ready_Floor" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Floor;
		}
		break;
	case Em1000::Attack_Ready_Wall:
		break;
	case Em1000::Enter_Ground_Floor:
		break;
	case Em1000::Enter_Ground_Wall:
		break;
	case Em1000::Hit_Floor:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Hit_Floor", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Floor" && m_pMesh->IsAnimationEnd())
			{
				m_bIng = false;
				m_bHit = false;
				m_eState = Idle_Floor;
			}
		}
		break;
	case Em1000::Hit_Wall:
		break;
	case Em1000::Attack_Floor:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Floor", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Floor" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle_Floor;
				m_bIng = false;
				m_bAttack = false;
			}
		}
		break;
	case Em1000::Attack_Wall:
		break;
	case Em1000::Dead_Floor:
		break;
	case Em1000::Dead_Wall:
		break;
	case Em1000::State_END:
		break;
	default:
		break;
	}
}

void Em1000::Skill_CoolTime(const float _fDeltaTime)
{
	if (m_bAttack == false)
	{
		m_fAttackTime += _fDeltaTime;
		if (m_fAttackTime >= 5.f)
		{
			m_bAttack = true;
			m_fAttackTime = 0.f;
		}
	}
}

HRESULT Em1000::Ready()
{
	Unit::Ready();
	//GameObject를 받아오려면 각자 태그가 있어야함.
	m_nTag = Monster1000;

	m_BattleInfo.iMaxHp = 200;
	m_BattleInfo.iHp = 200;
	m_BattleInfo.iAttack = 20;

	m_pTransform.lock()->SetPosition({ -4.8f, -0.2f, -4.82f });
		
	RenderInit();
	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.0005,0.0005,0.0005 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());

	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);

	return S_OK;
}

HRESULT Em1000::Awake()
{
	Unit::Awake();

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);

	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(false);

	m_pCollider.lock()->SetRadius(0.08f);
	m_pCollider.lock()->SetHeight(0.15f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.15f, 0.f });

	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();
	m_pRedQueen = std::static_pointer_cast<RedQueen>(FindGameObjectWithTag(GAMEOBJECTTAG::TAG_RedQueen).lock());

	m_pHand = AddGameObject<Em1000Hand>();
	m_pHand.lock()->m_pEm1000 = static_pointer_cast<Em1000>(m_pGameObject.lock());
	m_pHand.lock()->m_pEm1000Mesh = m_pMesh;

	m_eState = Idle_Floor;

	return S_OK;
}

HRESULT Em1000::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em1000::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	// 현재 스케일과 회전은 의미가 없음 DeltaPos 로 트랜스폼에서 통제 . 
	auto [DeltaScale, DeltaQuat, DeltaPos] = m_pMesh->Update(_fDeltaTime);
	Vector3 Axis = { 1,0,0 };
	  
	//ENGINE::AnimNotify _Notify{};
	////return true 면 이제 호출 안함, false면 저 루프 돌떄 계속 호출.
	//_Notify.Event[0.5] = [this]() {  AttackStart();  return false; };

	const float Length = FMath::Length(DeltaPos);
	//D3DXMATRIX matRot;
	//D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
	//D3DXMatrixRotationQuaternion(&matRot, &tQuat);

	//D3DXVec3TransformNormal(&DeltaPos, &DeltaPos, &matRot);
	

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		D3DXQUATERNION tResult = SpTransform->GetQuaternion() * DeltaQuat;
		SpTransform->SetQuaternion(tResult);

		D3DXMATRIX matRot;
		D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
		D3DXMatrixRotationQuaternion(&matRot, &tQuat);

		D3DXVec3TransformNormal(&DeltaPos, &DeltaPos, &matRot);

		SpTransform->SetPosition(SpTransform->GetPosition() + DeltaPos * SpTransform->GetScale().x);
	}


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
		if(!m_bHit)
			Skill_CoolTime(_fDeltaTime);
		Fight(_fDeltaTime);
		State_Change(_fDeltaTime);
	}

	
	return 0;
}

UINT Em1000::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em1000::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		ImGui::Text("Deg %3.4f", m_fRadian);
		ImGui::Text("Acc Deg %3.4f", m_fAccuangle);

		ImGui::InputFloat("Power", &m_fPower);

		ImGui::InputFloat("vPowerX", &m_vPower.x);
		ImGui::InputFloat("vPowerY", &m_vPower.y);
		ImGui::InputFloat("vPowerZ", &m_vPower.z);


	}
}

void Em1000::OnEnable()
{
	Unit::OnEnable();
}

void Em1000::OnDisable()
{
	Unit::OnDisable();
}

void Em1000::Hit(BT_INFO _BattleInfo, void* pArg)
{
	m_BattleInfo.iHp -= _BattleInfo.iAttack;

	m_bHit = true;
	m_eState = Hit_Floor;
};

void Em1000::Buster(BT_INFO _BattleInfo, void* pArg)
{

}

void Em1000::Snatch(BT_INFO _BattleInfo, void* pArg)
{

}

void Em1000::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (!m_bCollEnable)
		return;
	if (m_eState == Dead_Floor || m_eState == Dead_Wall)
		return;

	m_bCollEnable = false;
	switch (_pOther.lock()->m_nTag)
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		m_pHand.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Overture:
		m_BattleInfo.iHp -= static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo().iAttack;
		m_bHit = true;
		m_bDown = true;
		m_eState = Hit_Floor;
		break;
	default:
		break;
	}
}

void Em1000::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
	if (!m_bCollEnable)
		return;
	if (m_eState == Dead_Floor || m_eState == Dead_Wall)
		return;

	m_bCollEnable = false;
	switch (_pOther.lock()->m_nTag)
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		m_pHand.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Overture:
		m_BattleInfo.iHp -= static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo().iAttack;
		m_bHit = true;
		m_bDown = true;
		m_eState = Hit_Floor;
		break;
	default:
		break;
	}
}

void Em1000::RenderGBufferSK(const DrawInfo& _Info)
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

void Em1000::RenderShadowSK(const DrawInfo& _Info)
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

void Em1000::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em1000::RenderDebugSK(const DrawInfo& _Info)
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

void Em1000::RenderInit()
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
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em1000\\test.fbx", _InitInfo);

	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em1000\\Ani");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em1000\\Em1000.Animation");

	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
	//몬스터 초기상태 Idle
	m_pMesh->PlayAnimation("Idle_Floor", true);
}

void Em1000::Rotate(const float _fDeltaTime)
{
}

void Em1000::Update_Angle(const float _fDeltaTime, bool _bTest)
{
}

void Em1000::Update_Angle()
{
}
