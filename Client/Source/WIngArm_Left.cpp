#include "stdafx.h"
#include "WIngArm_Left.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

WIngArm_Left::WIngArm_Left()
	:m_bIsRender(false)
	,m_bLoop(false)
	,m_pParentBoneMat(nullptr)
{
}

void WIngArm_Left::Free()
{
	GameObject::Free();
}

WIngArm_Left* WIngArm_Left::Create()
{
	return new WIngArm_Left;
}

HRESULT WIngArm_Left::Ready()
{
	GameObject::Ready();
	RenderInit();

	m_NRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing_Arm\\pl0010_01_NRMR.tga");
	m_ATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing_Arm\\pl0010_01_Wing_ATOS.tga");
	//m_GradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing_Arm\\pl0010_04_Gradation_MSK1.tga");
	m_GradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\grad.png");

	m_pTransform.lock()->SetScale({ 0.025f,0.025f,0.025f });

	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);

	return S_OK;
}

HRESULT WIngArm_Left::Awake()
{
	GameObject::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	if (!m_pNero.expired())
		m_pParentBoneMat = m_pNero.lock()->Get_BoneMatrixPtr("L_Shoulder");

	return S_OK;
}

HRESULT WIngArm_Left::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT WIngArm_Left::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	float fCurAnimationTime = m_pMesh->PlayingTime();

	if (0.52f <= fCurAnimationTime && !m_bLoop)
	{
		SetActive(false);
		if (m_pNero.lock()->Get_IsMajinMode())
			m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_LWing, true);
	}

	//
	m_fAccTime += _fDeltaTime;

	return 0;
} 

UINT WIngArm_Left::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	Matrix								ParentWorldMatrix, FinalWorld;
	Matrix								Scale, Trans;
	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();
	Vector3 PlayerLook = m_pNero.lock()->GetComponent<Transform>().lock()->GetLook();


	if (nullptr != m_pParentBoneMat)
	{
		D3DXMatrixScaling(&Scale, 0.001f, 0.001f, 0.001f);
		D3DXMatrixTranslation(&Trans, m_pParentBoneMat->_41, m_pParentBoneMat->_42, m_pParentBoneMat->_43);

		FinalWorld = Trans * ParentWorldMatrix;
		FinalWorld._41 += PlayerLook.x * 0.03f;
		FinalWorld._42 += 0.005f;
		FinalWorld._43 += PlayerLook.z * 0.02f;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}


	return 0;
}

void WIngArm_Left::OnEnable()
{
	GameObject::OnEnable();
	m_bIsRender = true;
	_RenderProperty.bRender = m_bIsRender;

	m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_LWing, true);
}

void WIngArm_Left::OnDisable()
{
	GameObject::OnDisable();
	m_bIsRender = false;
	_RenderProperty.bRender = m_bIsRender;
}

void WIngArm_Left::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);

	m_bLoop = bLoop;
}

std::string WIngArm_Left::GetName()
{
	return "WingArm_Left";
}

float WIngArm_Left::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float WIngArm_Left::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

void WIngArm_Left::RenderInit()
{
	SetRenderEnable(true);

	// ???? ???? ???? ?????? 
	ENGINE::RenderProperty _InitRenderProp;
	// ?????? ???????? ?????? ?????? ???? ???? ????. 
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
	RenderInterface::Initialize(_InitRenderProp);

	// ???????? ???? ???? ... 
	Mesh::InitializeInfo _InitInfo{};
	// ?????? ???? ?????? CPU ?????? ???? ???? ? 
	_InitInfo.bLocalVertexLocationsStorage = false;

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing_Arm\\Wing_Arm_Left.fbx");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void WIngArm_Left::RenderAlphaBlendEffect(const DrawInfo& _Info)
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

void WIngArm_Left::RenderGBufferSK(const DrawInfo& _Info)
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

void WIngArm_Left::RenderShadowSK(const DrawInfo& _Info)
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

void WIngArm_Left::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void WIngArm_Left::RenderDebugSK(const DrawInfo& _Info)
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

void WIngArm_Left::RenderReady()
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

void WIngArm_Left::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{
		// ?????? .... 
		//char BoneName[MAX_PATH] = "";
		//ImGui::InputText("BoneName", BoneName, MAX_PATH);
		//if (ImGui::Button("ChangeBone"))
		//{
		//	m_pParentBoneMat = m_pNero.lock()->Get_BoneMatrixPtr(BoneName);
		//}
	}
}
