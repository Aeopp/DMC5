#include "stdafx.h"
#include "Nero_LWing.h"
#include "Renderer.h"
#include "Subset.h"
#include "Nero.h"
Nero_LWing::Nero_LWing()
	:m_bIsRender(false)
	,m_pParentBoneMat(nullptr)
{
}

void Nero_LWing::Free()
{
	GameObject::Free();
}

Nero_LWing* Nero_LWing::Create()
{
	return new Nero_LWing;
}

HRESULT Nero_LWing::Ready()
{
	RenderInit();

	m_NRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing\\pl0010_06_WingArm_NRMR.tga");
	m_ATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing\\pl0010_06_WingArm_ATOS.tga");
	m_GradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\grad.png");
	m_ClothAuraALBTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_03_cloth_aura00_00_00_ALBA.tga");

	D3DXMatrixScaling(&m_AuraOffsetMatrix, 0.4f, 0.8f, 0.55f);
	Matrix Temp;
	D3DXMatrixRotationX(&Temp, D3DXToRadian(10.f));
	m_AuraOffsetMatrix *= Temp;
	D3DXMatrixRotationY(&Temp, D3DXToRadian(-130.f));
	m_AuraOffsetMatrix *= Temp;
	D3DXMatrixRotationZ(&Temp, D3DXToRadian(-25.f));
	m_AuraOffsetMatrix *= Temp;

	m_pTransform.lock()->SetScale({ 0.001f,0.001f,0.001f });
	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);

	return S_OK;
}

HRESULT Nero_LWing::Awake()
{
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	
	if (!m_pNero.expired())
		m_pParentBoneMat = m_pNero.lock()->Get_BoneMatrixPtr("L_Shoulder");

	return S_OK;
}

HRESULT Nero_LWing::Start()
{
	return S_OK;
}

UINT Nero_LWing::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	m_pMesh->GetRootNode()->NodeUpdate(FMath::Identity(), 0.f, "", {});
	m_pMesh->UpdateToRootMatricies();
	m_pMesh->VTFUpdate();

	//
	m_fAccTime += _fDeltaTime;
	m_WingCloth01Matrix = (*m_pMesh->GetNodeToRoot("L_WingCloth01_end") * m_pTransform.lock()->GetRenderMatrix());
	m_AuraWorldMatrix = m_AuraOffsetMatrix * m_WingCloth01Matrix;

	m_AuraMesh->GetRootNode()->NodeUpdate(FMath::Identity(), 0.f, "", {});
	m_AuraMesh->UpdateToRootMatricies();
	m_AuraMesh->VTFUpdate();
	//

	return 0;
}

UINT Nero_LWing::LateUpdate(const float _fDeltaTime)
{
	Matrix	ParentWorldMatrix, FinalWorld;

	if (auto pNero = m_pNero.lock();
		pNero)
	{
		ParentWorldMatrix = pNero->Get_NeroWorldMatrix();

		if (nullptr != m_pParentBoneMat)
		{
			FinalWorld = *m_pParentBoneMat * ParentWorldMatrix;
			m_pTransform.lock()->SetWorldMatrix(FinalWorld);
		}
	}

	//m_pTransform.lock()->UpdateTransform();

	return 0;
}

void Nero_LWing::OnEnable()
{
	GameObject::OnEnable();
	m_bIsRender = true;
	_RenderProperty.bRender = m_bIsRender;
}

void Nero_LWing::OnDisable()
{
	GameObject::OnDisable();
	m_bIsRender = false;
	_RenderProperty.bRender = m_bIsRender;
}

void Nero_LWing::RenderInit()
{
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = m_bIsRender;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"NeroWingSK",
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

	// ���̷��� �޽� �ε� ... 
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing\\Wing_Left.fbx" , _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());

	m_AuraMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_cloth_aura00_00.fbx");
	m_AuraMesh->EnableToRootMatricies();
	PushEditEntity(m_AuraMesh.get());
}

void Nero_LWing::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	if (0 == _Info.PassIndex)
	{
		auto WeakSubset = m_AuraMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_Info.Fx->SetMatrix("World", &m_AuraWorldMatrix);
			_Info.Fx->SetTexture("GradationMap", m_ClothAuraALBTex->GetTexture());
			_Info.Fx->SetFloat("_BrightScale", 0.015f);
			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_AccumulationTexV", m_fAccTime * 0.2f);

			SharedSubset->Render(_Info.Fx);
		}
	}
	else if (1 == _Info.PassIndex)
	{
		m_pMesh->BindVTF(_Info.Fx);

		auto WeakSubset = m_pMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			const Matrix World = _RenderUpdateInfo.World;
			_Info.Fx->SetMatrix("World", &World);

			//if (!Renderer::GetInstance()->GetDirLights().empty())
			//{
			//	// ����
			//	auto dirLight = Renderer::GetInstance()->GetDirLights().begin()->get()->GetDirection();
			//	_Info.Fx->SetFloatArray("LightDirection", dirLight, 3u);
			//}

			_Info.Fx->SetTexture("NRMR0Map", m_NRMRTex->GetTexture());
			_Info.Fx->SetTexture("ATOS0Map", m_ATOSTex->GetTexture());
			_Info.Fx->SetTexture("GradationMap", m_GradationTex->GetTexture());
			_Info.Fx->SetFloat("_BrightScale", 0.015f);
			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_AccumulationTexV", m_fAccTime * 0.6f);

			SharedSubset->Render(_Info.Fx);
		}
	}
}

void Nero_LWing::RenderGBufferSK(const DrawInfo& _Info)
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

void Nero_LWing::RenderShadowSK(const DrawInfo& _Info)
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

void Nero_LWing::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Nero_LWing::RenderDebugSK(const DrawInfo& _Info)
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

void Nero_LWing::RenderReady()
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


void Nero_LWing::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{
		static char BoneName[MAX_PATH] = "";
		ImGui::InputText("BoneName", BoneName, MAX_PATH);
		if (ImGui::Button("ChangeBone"))
		{
			m_pParentBoneMat = m_pNero.lock()->Get_BoneMatrixPtr(BoneName);
		}

		//static Vector3 Rot{ 0.f, 0.f, 0.f };
		//ImGui::SliderFloat3("Rot##LWing", Rot, -180.f, 180.f);
		//m_AuraRot = Rot;
	}
}

std::string Nero_LWing::GetName()
{
	return "Nero_LWing";
}
