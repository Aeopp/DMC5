#include "stdafx.h"
#include "Buster_Arm_Left.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

Buster_Arm_Left::Buster_Arm_Left()
{
	m_nTag = TAG_BusterArm_Left;
}

void Buster_Arm_Left::Free()
{
	Unit::Free();
}

Buster_Arm_Left* Buster_Arm_Left::Create()
{
	return new Buster_Arm_Left;
}

HRESULT Buster_Arm_Left::Ready()
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

HRESULT Buster_Arm_Left::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	return S_OK;
}

HRESULT Buster_Arm_Left::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Buster_Arm_Left::Update(const float _fDeltaTime)
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

	if("em0000_Buster_End" == m_pMesh->AnimName && 0.2f <= m_pMesh->PlayingTime())
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

UINT Buster_Arm_Left::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Buster_Arm_Left::OnEnable()
{
	Unit::OnEnable();

	Matrix NeroWorld = m_pNero.lock()->Get_NeroWorldMatrix();
	std::optional<Matrix> R_HandLocal = m_pNero.lock()->Get_BoneMatrix_ByName("root");
	Matrix R_HandWorld = *R_HandLocal * NeroWorld;

	memcpy(NeroWorld.m[3], R_HandWorld.m[3], sizeof(Vector3));

	Vector3 PlayerLook = m_pNero.lock()->GetComponent<Transform>().lock()->GetLook();
	NeroWorld._41 += PlayerLook.x * -0.02f;
	NeroWorld._43 += PlayerLook.z * -0.02f;

	m_pTransform.lock()->SetWorldMatrix(NeroWorld);

	_RenderProperty.bRender = true;
}

void Buster_Arm_Left::OnDisable()
{
	Unit::OnDisable();
	_RenderProperty.bRender = false;
}

void Buster_Arm_Left::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Buster_Arm_Left::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Buster_Arm_Left::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void Buster_Arm_Left::RenderReady()
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
}

void Buster_Arm_Left::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		// ������ .... 
	}
}

void Buster_Arm_Left::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = true;
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
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Buster_Arm\\BusterArm_Left.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void Buster_Arm_Left::RenderAlphaBlendEffect(const DrawInfo& _Info)
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

void Buster_Arm_Left::RenderGBufferSK(const DrawInfo& _Info)
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

void Buster_Arm_Left::RenderShadowSK(const DrawInfo& _Info)
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

void Buster_Arm_Left::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Buster_Arm_Left::RenderDebugSK(const DrawInfo& _Info)
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

void Buster_Arm_Left::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

std::string Buster_Arm_Left::GetName()
{
	return "Buster_Arm_Left";
}

float Buster_Arm_Left::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float Buster_Arm_Left::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}
