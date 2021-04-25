#include "stdafx.h"
#include "Wire_Arm_Grab.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"
#include "Wire_Arm.h"
#include "Monster.h"

Wire_Arm_Grab::Wire_Arm_Grab()
	:m_bIsRender(false)
{
	m_nTag = TAG_WireArm_Grab;
	m_vDir = {0.f,0.f,0.f};
}

void Wire_Arm_Grab::Free()
{
	Unit::Free();
}

Wire_Arm_Grab* Wire_Arm_Grab::Create()
{
	return new Wire_Arm_Grab;
}

HRESULT Wire_Arm_Grab::Ready()
{
	Unit::Ready();
	RenderInit();

	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());
	SetActive(false);

	return S_OK;
}

HRESULT Wire_Arm_Grab::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRadius(0.11f);
	m_pCollider.lock()->SetCenter({ 0.f,0.1f,0.f });
	PushEditEntity(m_pCollider.lock().get());


	return S_OK;
}

HRESULT Wire_Arm_Grab::Start()
{
	Unit::Start();


	return S_OK;
}

UINT Wire_Arm_Grab::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	

	Vector3 NeroPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	Vector3 vLength = NeroPos - m_pTransform.lock()->GetPosition();
	float fLength = D3DXVec3Length(&vLength);

	if(!m_pGrabedMonster.expired())
		m_pGrabedMonster.lock()->GetComponent<Transform>().lock()->Translate(m_vDir * 0.07f);

	m_pTransform.lock()->Translate(m_vDir * 0.07f);

	if (m_pMesh->IsAnimationEnd())
	{
		SetActive(false);
	}
	if (0.17f >= fLength && m_bPlayOnce)
	{
		m_bPlayOnce = false;
		m_pMesh->PlayAnimation("Wire_Arm_End_Long", false);
		m_pMesh->ContinueAnimation();
		m_vDir = { 0.f, 0.f,0.f };
		m_pTransform.lock()->Translate({ 0.f,-0.01f,0.f });
		static_pointer_cast<Monster>(m_pGrabedMonster.lock())->Set_Snatch(false);
	}

	return 0;
}

UINT Wire_Arm_Grab::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Wire_Arm_Grab::OnEnable()
{
	Unit::OnEnable();
	_RenderProperty.bRender = true;

	m_pMesh->PlayAnimation("Wire_Arm_End_Long", false);
	m_pMesh->SetPlayingTime(0.05f);
	m_pMesh->StopAnimation();

	Vector3 NeroPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	NeroPos += m_pNero.lock()->GetComponent<Transform>().lock()->GetRight() * -0.05f;
	m_vDir = NeroPos - m_pTransform.lock()->GetPosition();
	D3DXVec3Normalize(&m_vDir, &m_vDir);

	m_pTransform.lock()->SetQuaternion(m_pNero.lock()->GetComponent<Transform>().lock()->GetQuaternion());

	m_bPlayOnce = true;
	static_pointer_cast<Monster>(m_pGrabedMonster.lock())->Set_Snatch(true);
}

void Wire_Arm_Grab::OnDisable()
{
	Unit::OnDisable();
	_RenderProperty.bRender = false;
	m_bPlayOnce = false;
}

void Wire_Arm_Grab::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	//UINT ObjTag = _pOther.lock()->m_nTag;
	//if (GAMEOBJECTTAG::Player == ObjTag)
	//{
	//	m_pMesh->PlayAnimation("Wire_Arm_End_Long", false);
	//	m_pMesh->ContinueAnimation();
	//	m_vDir = { 0.f,-0.01f,0.f };
	//}
}

void Wire_Arm_Grab::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void Wire_Arm_Grab::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Wire_Arm_Grab::SetGrabedMonster(std::weak_ptr<GameObject> _GrabedMonster)
{
	if (!_GrabedMonster.expired())
		m_pGrabedMonster = _GrabedMonster;
}

void Wire_Arm_Grab::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

std::string Wire_Arm_Grab::GetName()
{
	return "Wire_Arm_Grab";
}

float Wire_Arm_Grab::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float Wire_Arm_Grab::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

void Wire_Arm_Grab::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
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

void Wire_Arm_Grab::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		// ������ .... 
	}
}

void Wire_Arm_Grab::RenderInit()
{
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = m_bIsRender;
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
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wire_Arm\\Wire_Arm.fbx");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wire_Arm\\Wire_Arm.Animation");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void Wire_Arm_Grab::RenderGBufferSK(const DrawInfo& _Info)
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

void Wire_Arm_Grab::RenderShadowSK(const DrawInfo& _Info)
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

void Wire_Arm_Grab::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Wire_Arm_Grab::RenderDebugSK(const DrawInfo& _Info)
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
