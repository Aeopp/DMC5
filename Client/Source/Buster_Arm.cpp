#include "stdafx.h"
#include "Buster_Arm.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"
#include "Monster.h"
#include "NeroFSM.h"

Buster_Arm::Buster_Arm()
	:m_bIsRender(false)
	,m_bHitOnce_Em5000(true)
{
	m_nTag = TAG_BusterArm_Right;
	m_BattleInfo.iAttack = 0;
	//m_BattleInfo.iAttack = 20;
}

void Buster_Arm::Free()
{
	Unit::Free();
}

Buster_Arm* Buster_Arm::Create()
{
	return new Buster_Arm;
}

HRESULT Buster_Arm::Ready()
{
	Unit::Ready();
	RenderInit();

	m_NRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Buster_Arm\\pl0010_BusterArm_NRMR.tga");
	m_ATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Buster_Arm\\pl0010_BusterArm_ATOS.tga");
	//m_GradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Buster_Arm\\pl0010_20_Gradation_MSK1.tga");
	m_GradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\grad.png");

	m_pTransform.lock()->SetScale({ 0.03f,0.03f,0.03f });
	
	PushEditEntity(m_pTransform.lock().get());
	SetActive(false);

	return S_OK;
}

HRESULT Buster_Arm::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	
	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetCenter({ 0.f,0.13f,-0.05f });
	m_pCollider.lock()->SetRadius(0.05f);
	m_pCollider.lock()->SetActive(false);
	PushEditEntity(m_pCollider.lock().get());

	return S_OK;
}

HRESULT Buster_Arm::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Buster_Arm::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	Matrix NeroWorld = m_pNero.lock()->Get_NeroWorldMatrix();
	std::optional<Matrix> R_HandLocal = m_pNero.lock()->Get_BoneMatrix_ByName("root");
	Matrix R_HandWorld = *R_HandLocal * NeroWorld;

	memcpy(NeroWorld.m[3], R_HandWorld.m[3], sizeof(Vector3));

	Vector3 PlayerLook = m_pNero.lock()->GetComponent<Transform>().lock()->GetLook();
	NeroWorld._41 += PlayerLook.x * -0.02f;
	NeroWorld._43 += PlayerLook.z * -0.02f;

	m_pTransform.lock()->SetWorldMatrix(NeroWorld);


	if ("em0000_Buster_End" == m_pMesh->AnimName && 0.2f <= m_pMesh->PlayingTime())
		SetActive(false);

	if ("em5000_Buster_End" == m_pMesh->AnimName && 0.2f <= m_pMesh->PlayingTime())
		SetActive(false);

	if (m_pMesh->IsAnimationEnd())
	{
		if ("em0000_Buster_Start" != m_pMesh->AnimName)
			SetActive(false);
	}

	//
	m_fAccTime += _fDeltaTime;

	return 0;
}

UINT Buster_Arm::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Buster_Arm::OnEnable()
{
	Unit::OnEnable();
	m_bIsRender = true;

	Matrix NeroWorld = m_pNero.lock()->Get_NeroWorldMatrix();
	std::optional<Matrix> R_HandLocal = m_pNero.lock()->Get_BoneMatrix_ByName("root");
	Matrix R_HandWorld = *R_HandLocal * NeroWorld;

	memcpy(NeroWorld.m[3], R_HandWorld.m[3], sizeof(Vector3));

	Vector3 PlayerLook = m_pNero.lock()->GetComponent<Transform>().lock()->GetLook();
	NeroWorld._41 += PlayerLook.x * -0.02f;
	NeroWorld._43 += PlayerLook.z * -0.02f;

	m_pTransform.lock()->SetWorldMatrix(NeroWorld);

	_RenderProperty.bRender = m_bIsRender;


	if (m_pCollider.lock())
		m_pCollider.lock()->SetActive(true);

}

void Buster_Arm::OnDisable()
{
	Unit::OnDisable();
	m_bIsRender = false;
	m_pMesh->SetPlayingTime(0);

	_RenderProperty.bRender = m_bIsRender;

	if(m_pCollider.lock())
		m_pCollider.lock()->SetActive(false);
}

void Buster_Arm::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Buster_Arm::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (nullptr == dynamic_pointer_cast<Monster>(_pOther.lock()))
		return;
	UINT MonsterTag = _pOther.lock()->m_nTag;
	UINT CurNeroAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	switch (MonsterTag)
	{
	case Monster100:
		m_pNero.lock()->SetNoHit(true);
		switch (CurNeroAnimationIndex)
		{
		case Nero::ANI_BUSTER_START:
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::BUSTER_STRIKE_COMMON);
			break;
		case Nero::ANI_BUSTER_AIR_CATCH:
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::BUSTER_STRIKE_COMMON_AIR);
			break;
		default:
			break;
		}
		break;
	case Monster0000:
		m_pNero.lock()->SetNoHit(true);
		switch (CurNeroAnimationIndex)
		{
		case Nero::ANI_BUSTER_START:
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::EM0000_BUSTER_START);
			break;
		case Nero::ANI_BUSTER_AIR_CATCH:
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::EM0000_BUSTER_AIR);
			break;
		default:
			break;
		}
		break;
	case Monster200:
		m_pNero.lock()->SetNoHit(true);
		switch (CurNeroAnimationIndex)
		{
		case Nero::ANI_BUSTER_START:
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::EM200_BUSTER_START);
			break;
		case Nero::ANI_BUSTER_AIR_CATCH:
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::EM200_BUSTER_AIR_START);
			break;
		default:
			break;
		}
		break;
	case Monster5000:
		//그로기 상태일때만
		if (m_bHitOnce_Em5000 &&
			static_pointer_cast<Monster>(_pOther.lock())->Get_Groggy())
		{
			m_pNero.lock()->SetNoHit(true);
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::EM5000_BUSTER_START);
			m_bHitOnce_Em5000 = false;
		}
		break;
	default:
		break;
	}

}

void Buster_Arm::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void Buster_Arm::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = m_bIsRender;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"NeroWingArmSK",
		[this](const DrawInfo& _Info)
			{
				RenderAlphaBlendEffect(_Info);
			}
		},
	};
	//_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	//{
	//	{"gbuffer_dsSK",
	//	[this](const DrawInfo& _Info)
	//		{
	//			RenderGBufferSK(_Info);
	//		}
	//	},
	//};
	//_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
	//	=
	//{
	//	{"ShadowSK" ,
	//	[this](const DrawInfo& _Info)
	//	{
	//		RenderShadowSK(_Info);
	//	}
	//} };
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
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Buster_Arm\\BusterArm_Right.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());

}

void Buster_Arm::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	//if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
	//	return;

	m_pMesh->BindVTF(_Info.Fx);

	auto WeakSubset = m_pMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		const Matrix World = _RenderUpdateInfo.World;
		_Info.Fx->SetMatrix("World", &World);
		_Info.Fx->SetTexture("NRMR0Map", m_NRMRTex->GetTexture());
		_Info.Fx->SetTexture("ATOS0Map", m_ATOSTex->GetTexture());
		_Info.Fx->SetTexture("GradationMap", m_GradationTex->GetTexture());
		_Info.Fx->SetFloat("_BrightScale", 0.015f);
		_Info.Fx->SetFloat("_SliceAmount", 0.f);
		_Info.Fx->SetFloat("_AccumulationTexV", m_fAccTime * 0.6f);

		SharedSubset->Render(_Info.Fx);
	}
}

void Buster_Arm::RenderGBufferSK(const DrawInfo& _Info)
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
		//if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		//{
		//	continue;
		//}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Buster_Arm::RenderShadowSK(const DrawInfo& _Info)
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
		//if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		//{
		//	continue;
		//}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Buster_Arm::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Buster_Arm::RenderDebugSK(const DrawInfo& _Info)
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
		//if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		//{
		//	continue;
		//}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Buster_Arm::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

std::string Buster_Arm::GetName()
{
	return "Buster_Arm";
}

float Buster_Arm::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float Buster_Arm::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

void Buster_Arm::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		//const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		//if (m_pMesh)
		//{
		//	const uint32  Numsubset = m_pMesh->GetNumSubset();
		//	_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

		//	for (uint32 i = 0; i < Numsubset; ++i)
		//	{
		//		const auto& _Subset = m_pMesh->GetSubset(i);
		//		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		//		_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
		//	}
		//}
	}
};


void Buster_Arm::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		// 에디터 .... 
	}
}
