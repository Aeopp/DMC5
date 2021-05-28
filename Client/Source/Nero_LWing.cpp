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

	//m_DissolveInfo.SetDissolve(false);
	
	return S_OK;
}

HRESULT Nero_LWing::Awake()
{
	GameObject::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	
	if (!m_pNero.expired())
		m_pParentBoneMat = m_pNero.lock()->Get_BoneMatrixPtr("L_Shoulder");

	m_pChestRoot = m_pNero.lock()->Get_BoneMatrixPtr("Chest");

	LPCLOTHBONE pClothBone = nullptr;
	Node* pNode = nullptr;

	pClothBone = new CLOTHBONE;
	pNode = m_pMesh->GetNode("L_WingCloth01_end");
	pClothBone->sName = pNode->Name;
	pClothBone->pParent = nullptr;
	pClothBone->matToRoot = pNode->ToRoot;
	pClothBone->pNode = pNode;
	m_vecClothBone.push_back(pClothBone);

	pClothBone = new CLOTHBONE;
	pNode = m_pMesh->GetNode("L_WingCloth00_00");
	pClothBone->sName = pNode->Name;
	pClothBone->pParent = nullptr;
	pClothBone->matToRoot = pNode->ToRoot;
	pClothBone->pNode = pNode;
	m_vecClothBone.push_back(pClothBone);

	pClothBone = new CLOTHBONE;
	pNode = m_pMesh->GetNode("L_WingCloth00_01");
	pClothBone->sName = pNode->Name;
	pClothBone->pParent = m_vecClothBone.back();
	pClothBone->matToRoot = pNode->ToRoot;
	pClothBone->pNode = pNode;
	m_vecClothBone.push_back(pClothBone);

	pClothBone = new CLOTHBONE;
	pNode = m_pMesh->GetNode("L_WingCloth00_02");
	pClothBone->sName = pNode->Name;
	pClothBone->pParent = m_vecClothBone.back();
	pClothBone->matToRoot = pNode->ToRoot;
	pClothBone->pNode = pNode;
	m_vecClothBone.push_back(pClothBone);

	pClothBone = new CLOTHBONE;
	pNode = m_pMesh->GetNode("L_WingCloth00_03");
	pClothBone->sName = pNode->Name;
	pClothBone->pParent = m_vecClothBone.back();
	pClothBone->matToRoot = pNode->ToRoot;
	pClothBone->pNode = pNode;
	m_vecClothBone.push_back(pClothBone);

	pClothBone = new CLOTHBONE;
	pNode = m_pMesh->GetNode("L_WingCloth00_end");
	pClothBone->sName = pNode->Name;
	pClothBone->pParent = m_vecClothBone.back();
	pClothBone->matToRoot = pNode->ToRoot;
	pClothBone->pNode = pNode;
	m_vecClothBone.push_back(pClothBone);

	vector<PxVec4> vecVertex;

	for (UINT i = 0; i < m_vecClothBone.size(); ++i)
	{
		PxVec3 vPos(0.f, 0.f, 0.f);

		memcpy_s(&vPos, sizeof(PxVec3), m_vecClothBone[i]->matToRoot.m[3], sizeof(PxVec3));

		float fInvMass = m_vecClothBone[i]->pParent == nullptr ? 0.f : 1.f;

		vecVertex.push_back(PxVec4(vPos, fInvMass));
	}

	vector<PxU32> vecIndex;
	
	for (UINT i = 0; i < 4; ++i)
	{
		vecIndex.push_back(0);
		vecIndex.push_back(i + 2);
		vecIndex.push_back(i + 1);
	}

	PxClothMeshDesc meshDesc;

	meshDesc.setToDefault();

	meshDesc.points.data = vecVertex.data();
	meshDesc.points.count = vecVertex.size();
	meshDesc.points.stride = sizeof(PxVec4);

	meshDesc.invMasses.data = &vecVertex.data()->w;
	meshDesc.invMasses.count = vecVertex.size();
	meshDesc.invMasses.stride = sizeof(PxVec4);

	meshDesc.triangles.data = vecIndex.data();
	meshDesc.triangles.count = vecIndex.size() / 3;
	meshDesc.triangles.stride = sizeof(PxU32) * 3;

	PxClothMeshQuadifier quadifier(meshDesc);

	PxClothFabric* pClothFabric = PxClothFabricCreate(*Physics::GetPxPhysics(), quadifier.getDescriptor(), PxVec3(0, -1, 0));

	PxClothParticle* pParticles = (PxClothParticle*)meshDesc.points.data;

	PxTransform transform(PxVec3(0.f, 0.f, 0.f), PxQuat(0.f, 0.f, 0.f, 1.f));

	m_pCloth = Physics::GetPxPhysics()->createCloth(transform, *pClothFabric, pParticles, PxClothFlags());

	//m_pCloth->setSolverFrequency(240);

	//m_pCloth->setStiffnessFrequency(10.0f);

	//// damp global particle velocity to 90% every 0.1 seconds
	////m_pCloth->setDampingCoefficient(PxVec3(0.5f)); // damp local particle velocity
	////m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	////m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

	//// reduce impact of frame acceleration
	//// x, z: cloth swings out less when walking in a circle
	//// y: cloth responds less to jump acceleration
	////m_pCloth->setLinearInertiaScale(PxVec3(0.8f, 0.6f, 0.8f));

	//// leave impact of frame torque at default
	////m_pCloth->setAngularInertiaScale(PxVec3(1.0f));

	////// reduce centrifugal force of rotating frame
	////m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));


	////m_pCloth->setClothFlag(PxClothFlag::eSWEPT_CONTACT, true);


	////PxClothStretchConfig stretchConfig;
	////stretchConfig.stiffness = 1.0f;

	//m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.5f));
	//m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.5f));
	//m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.75f));
	//m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
	//m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));

	Physics::AddActor(GetSceneID(), *m_pCloth);
	return S_OK;
}

HRESULT Nero_LWing::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT Nero_LWing::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	PxClothParticleData* pParticleData = m_pCloth->lockParticleData();

	PxClothParticle* pParticles = pParticleData->particles;

	for (UINT i = 0; i < m_vecClothBone.size(); ++i)
	{
		if (nullptr == m_vecClothBone[i]->pParent)
			continue;

		D3DXMATRIX matParent = m_vecClothBone[i]->pParent->matToRoot;
		D3DXMatrixInverse(&matParent, nullptr, &matParent);

		memcpy_s(m_vecClothBone[i]->matToRoot.m[3], sizeof(PxVec3), &pParticles[i].pos, sizeof(PxVec3));

		D3DXMATRIX newTransform = m_vecClothBone[i]->matToRoot * matParent;

		m_vecClothBone[i]->pNode->ClothTransform = newTransform;
	}

	pParticleData->unlock();

	D3DXVECTOR3 vScale, vPos;
	D3DXQUATERNION tQuat;
	D3DXMatrixDecompose(&vScale, &tQuat, &vPos, m_pChestRoot);

	PxVec3 targetPos;
	memcpy_s(&targetPos, sizeof(PxVec3), &vPos, sizeof(PxVec3));
	PxQuat targetQuat;
	memcpy_s(&targetQuat, sizeof(PxQuat), &tQuat, sizeof(PxQuat));

	m_pCloth->setTargetPose(PxTransform(targetPos, targetQuat));

	PxVec3 vWindDir(1.f, 0.2f, 0.5f);
	PxReal fWindStrength = 100.f;
	PxVec3 vWindRange(0.f, 10.f, 10.f);

	static PxReal fTime = 0.f;

	fTime += _fDeltaTime * (rand() % 10);

	float st = 1.f + (float)sin(fTime);
	float windStrength = (((rand() / 10) * (float)sin(fTime)) + 1.f) * fWindStrength;
	float windRangeStrength = (rand() % 20) * 0.1f;

	PxVec3 vOffset((rand() % 10) * 0.1f * (rand() % 2 == 0 ? 1 : -1), (rand() % 10) * 0.1f * (rand() % 2 == 0 ? 1 : -1), (rand() % 10) * 0.1f * (rand() % 2 == 0 ? 1 : -1));

	float ct = 1.f + (float)cos(fTime + 0.1);
	vOffset *= ct;

	PxVec3 vWindAcceleration = fWindStrength * vWindDir + windRangeStrength * vWindRange.multiply(vOffset);

	m_pCloth->setExternalAcceleration(vWindAcceleration);

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
	m_DissolveInfo.DissolveUpdate(_fDeltaTime, _RenderUpdateInfo.World);
	//m_pTransform.lock()->UpdateTransform();

	if (m_IsTimeOut && !m_DissolveInfo.IsDissolve())
	{
		SetActive(false);
	}
	return 0;
}

void Nero_LWing::OnEnable()
{
	GameObject::OnEnable();
	m_bIsRender = true;
	_RenderProperty.bRender = m_bIsRender;
	m_DissolveInfo.DissolveStart(true, false,1.f);
	m_IsTimeOut = false;
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

	// 렌더 속성 전체 초기화 
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = m_bIsRender;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"NeroWingSK2",
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

	// 스켈레톤 메쉬 로딩 ... 
	Mesh::InitializeInfo _InitInfo{};
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;

	m_DissolveInfo.Initialize(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing\\Wing_Left.fbx",
		Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f / 255.f , 0.f , 1.f / 255.f }, 0.85f);

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing\\Wing_Left.fbx" , _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());

	m_AuraMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_cloth_aura00_00.fbx");
	m_AuraMesh->EnableToRootMatricies();
	PushEditEntity(m_AuraMesh.get());
}

void Nero_LWing::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	//if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
	//	return;

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
		m_DissolveInfo.DissolveVariableBind(_Info.Fx);
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

void Nero_LWing::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		//const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
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
		m_DissolveInfo.DissolveEditor();
	}
}

std::string Nero_LWing::GetName()
{
	return "Nero_LWing";
}

void Nero_LWing::SetDissolve()
{
	m_DissolveInfo.DissolveStart(false, false, 1.f);
	m_IsTimeOut = true;
}
