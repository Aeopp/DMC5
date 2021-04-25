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

	//if (m_BattleInfo.iHp <= 0.f)
	//{
	//	m_eState = Dead;
	//	m_bIng = true;
	//	return;
	//}

	//���� �����̴� ���� �����ִ� ��
	if (fDir >= 0.3f)
	{
		int iRandom = FMath::Random<int>(1, 6);
		if (m_bMove && m_bIng == false)
		{
			m_bIng = true;
			//�÷��̾� �������� ���� ����
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
	//�÷��̾�� ������� ����� ������ ����.	
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
			//ü����50% �̻��϶� Attack_A, Attack_D ���� �ϳ� �̰� �ΰ��� �׳� �˹� ��Ʈ
			//50%���� �������� Attack_Hard �߰��ؼ� �÷��̾� �Ѿ�Ʈ���� ���� �߰�
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

void Em1000::State_Change(const float _fDeltaTime)
{
	
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
	//GameObject�� �޾ƿ����� ���� �±װ� �־����.
	m_nTag = Monster1000;

	m_BattleInfo.iMaxHp = 200;
	m_BattleInfo.iHp = 200;
	m_BattleInfo.iAttack = 20;

	m_pTransform.lock()->SetPosition({ -3.5f, 1.f, 3.f });
		
	RenderInit();
	// Ʈ������ �ʱ�ȭ�ϸ� Edit �� ������ ǥ�õǵ��� Ǫ�� . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	PushEditEntity(InitTransform.lock().get());

	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());

	//���� ȸ�� �⺻ �ӵ�
	m_fAngleSpeed = D3DXToRadian(100.f);


	m_fPower = 100.f;
	m_vPower = D3DXVECTOR3(0.f, 1.f, 0.5f);
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


	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);

	m_pCollider.lock()->SetRadius(0.08f);
	m_pCollider.lock()->SetHeight(0.15f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.15f, 0.f });

	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();
	m_pRedQueen = std::static_pointer_cast<RedQueen>(FindGameObjectWithTag(GAMEOBJECTTAG::TAG_RedQueen).lock());


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
	// ���� �����ϰ� ȸ���� �ǹ̰� ���� DeltaPos �� Ʈ���������� ���� . 
	auto [DeltaScale, DeltaQuat, DeltaPos] = m_pMesh->Update(_fDeltaTime);
	Vector3 Axis = { 1,0,0 };
	  
	//ENGINE::AnimNotify _Notify{};
	////return true �� ���� ȣ�� ����, false�� �� ���� ���� ��� ȣ��.
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
		}
	
}

void Em1000::Buster(BT_INFO _BattleInfo, void* pArg)
{
}

void Em1000::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (!m_bCollEnable)
		return;

	m_bCollEnable = false;
	switch (_pOther.lock()->m_nTag)	
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		Buster(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		break;
	default:
		break;
	}
}

void Em1000::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
	switch (_pOther.lock()->m_nTag)
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
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
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em1000\\Em1000.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
	//���� �ʱ���� Idle
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
