#include "stdafx.h"
#include "NewWingSword.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

NewWingSword::NewWingSword()
	:m_pParentBoneMat(nullptr)
{
}

void NewWingSword::Free()
{
	GameObject::Free();
}

NewWingSword* NewWingSword::Create()
{
	return new NewWingSword;
}

HRESULT NewWingSword::Ready()
{
	GameObject::Ready();
	RenderInit();

	m_NRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\wp01_006_NRMR.tga");
	m_ATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\wp01_006_ATOS.tga");
	m_GradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\grad.png");

	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);

	return S_OK;
}

HRESULT NewWingSword::Awake()
{
	GameObject::Awake();

	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	if (!m_pNero.expired())
		m_pParentBoneMat = m_pNero.lock()->Get_BoneMatrixPtr("root");

	return S_OK;
}

HRESULT NewWingSword::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT NewWingSword::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh[m_iMeshIndex]->Update(_fDeltaTime);

	// 디졸브 하고있다면 대신 SliceAmount 등등 계산 해줌 .
	m_DissolveInfo.DissolveUpdate(_fDeltaTime,_RenderUpdateInfo.World);
	
	if (m_pMesh[m_iMeshIndex]->bLoop == false &&
		0.7f <= m_pMesh[m_iMeshIndex]->PlayingTime())
	{
		m_DissolveInfo.DissolveStart();
	}

	if (m_pMesh[m_iMeshIndex]->IsAnimationEnd())
	{
		SetActive(false);
	}

	return 0;
}

UINT NewWingSword::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	Matrix	ParentWorldMatrix, FinalWorld;

	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();
	if (nullptr != m_pParentBoneMat)
	{

		FinalWorld = *m_pParentBoneMat * ParentWorldMatrix;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}

	//
	m_fAccTime += _fDeltaTime;

	return 0;
}

void NewWingSword::OnEnable()
{
	GameObject::OnEnable();
	_RenderProperty.bRender = true;
	m_DissolveInfo.DissolveEnd();
}

void NewWingSword::OnDisable()
{
	GameObject::OnDisable();
	_RenderProperty.bRender = false;
}

std::string NewWingSword::GetName()
{
	return "NewWingSword";
}

void NewWingSword::ChangeAnimation(UINT _eAniList, bool _bLoop, bool _Overlap)
{
	m_iMeshIndex = _eAniList;
	m_pMesh[m_iMeshIndex]->PlayAnimation("CINEMA_4D___", _bLoop, {}, 1.f, 1.f, _Overlap);
}

void NewWingSword::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
	}
}

void NewWingSword::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{
		m_DissolveInfo.DissolveEditor();
	}
}

void NewWingSword::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh[m_iMeshIndex]->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh[m_iMeshIndex]->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void NewWingSword::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh[m_iMeshIndex]->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh[m_iMeshIndex]->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void NewWingSword::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh[m_iMeshIndex]->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void NewWingSword::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh[m_iMeshIndex]->GetNumSubset();

	if (Numsubset > 0)
	{
		m_pMesh[m_iMeshIndex]->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void NewWingSword::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh[m_iMeshIndex]->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh[m_iMeshIndex]->BindVTF(_Info.Fx);
		// 쉐이더 변수를 디졸브 정보가 대신 올려줌 . 
		m_DissolveInfo.DissolveVariableBind(_Info.Fx);
	}
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			_Info.Fx->SetTexture("NRMR0Map", m_NRMRTex->GetTexture());
			_Info.Fx->SetTexture("ATOS0Map", m_ATOSTex->GetTexture());
			_Info.Fx->SetTexture("GradationMap", m_GradationTex->GetTexture());
			_Info.Fx->SetFloat("_BrightScale", 0.015f);
			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_AccumulationTexV", m_fAccTime * 0.6f);

			SpSubset->Render(_Info.Fx);
		}
	}
}

void NewWingSword::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
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

	Vector3 DissolveColor = {1.f,0.f,0.f};
	m_DissolveInfo.Initialize(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Ar1.fbx", DissolveColor);

	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh[WingSword_Ar1] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Ar1.fbx", _InitInfo);
	m_pMesh[WingSword_Ar1]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Ar1].get());
	m_pMesh[WingSword_Ar2] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Ar2.fbx", _InitInfo);
	m_pMesh[WingSword_Ar2]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Ar2].get());
	m_pMesh[WingSword_Ar3] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Ar3.fbx", _InitInfo);
	m_pMesh[WingSword_Ar3]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Ar3].get());
	m_pMesh[WingSword_Ar4] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Ar4.fbx", _InitInfo);
	m_pMesh[WingSword_Ar4]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Ar4].get());

	m_pMesh[WingSword_ComboA1] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\ComboA1.fbx", _InitInfo);
	m_pMesh[WingSword_ComboA1]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_ComboA1].get());
	m_pMesh[WingSword_ComboA2] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\ComboA2.fbx", _InitInfo);
	m_pMesh[WingSword_ComboA2]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_ComboA2].get());
	m_pMesh[WingSword_ComboA3] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\ComboA3.fbx", _InitInfo);
	m_pMesh[WingSword_ComboA3]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_ComboA3].get());

	m_pMesh[WingSword_ComboB1] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\ComboB1.fbx", _InitInfo);
	m_pMesh[WingSword_ComboB1]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_ComboB1].get());

	m_pMesh[WingSword_Stinger_End] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Stinger_End.fbx", _InitInfo);
	m_pMesh[WingSword_Stinger_End]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Stinger_End].get());
	m_pMesh[WingSword_Stinger_Loop ] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Stinger_Loop.fbx", _InitInfo);
	m_pMesh[WingSword_Stinger_Loop]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Stinger_Loop].get());
	m_pMesh[WingSword_Stinger_Start] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Stinger_Start.fbx", _InitInfo);
	m_pMesh[WingSword_Stinger_Start]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Stinger_Start].get());

	m_pMesh[WingSword_HT] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\HT.fbx", _InitInfo);
	m_pMesh[WingSword_HT]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_HT].get());

	m_pMesh[WingSword_ComboB2_Loop] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\ComboB2_Loop.fbx", _InitInfo);
	m_pMesh[WingSword_ComboB2_Loop]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_ComboB2_Loop].get());

	m_pMesh[WingSword_ComboB2_End] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\ComboB2_End.fbx", _InitInfo);
	m_pMesh[WingSword_ComboB2_End]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_ComboB2_End].get());

	m_pMesh[WingSword_Helm] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\Helm.fbx", _InitInfo);
	m_pMesh[WingSword_Helm]->EnableToRootMatricies();
	PushEditEntity(m_pMesh[WingSword_Helm].get());
}
