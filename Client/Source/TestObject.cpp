#include "stdafx.h"
#include "..\Header\TestObject.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>

#include "PhysicsSystem.h"

void TestObject::Free()
{
	GameObject::Free();
}

std::string TestObject::GetName()
{
	return "TestObject";
};

TestObject* TestObject::Create()
{
	return new TestObject{};
};

void TestObject::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (m_pSkeletonMesh)
		{
			const uint32  Numsubset = m_pSkeletonMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset ;  ++i)
			{
				const auto& _Subset = m_pSkeletonMesh->GetSubset(i);
				const auto& _CurBS =_Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i]= _CurBS.Transform(_RenderUpdateInfo.World, Scale.x); 
			}
		}
	}
}

void TestObject::RenderInit()
{
	m_nTag = Player;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_ds",
		[this](const DrawInfo& _Info)
			{
				RenderGBuffer(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"Shadow" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadow(_Info);
		}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
		}
	} };

	/// <summary>
	/// DrawCollider
	/// </summary>
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };
	RenderInterface::Initialize(_InitRenderProp);

	// 
	// 스태틱 메쉬 로딩

	m_pSkeletonMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Player.fbx");
	PushEditEntity(m_pSkeletonMesh.get());
};

void TestObject::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pSkeletonMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pSkeletonMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue; 
			}
			

			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}
void TestObject::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pSkeletonMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pSkeletonMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};
}


void TestObject::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pSkeletonMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pSkeletonMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == 
				_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT TestObject::Ready()
{
	RenderInit();
	//
	Node* pNode = m_pSkeletonMesh->GetNode("CoatCloth_00_04");

	LPCLOTHBONE pCurr = nullptr;
	LPCLOTHBONE pNext = nullptr;

	int nIndex = 0;
	float fInvMass = 0;
	while (true)
	{
		pNext = new CLOTHBONE;

		if (nullptr != pCurr)
			pNext->pParent = pCurr;

		pNext->sName		= pNode->Name;
		pNext->matLocal		= pNode->Transform;
		pNext->matToRoot	= pNode->ToRoot;
		pNext->pNode		= pNode;
		
		pNext->vPos.x = pNext->matToRoot.m[3][0];
		pNext->vPos.y = pNext->matToRoot.m[3][1];
		pNext->vPos.z = pNext->matToRoot.m[3][2];


		m_vecClothParticle.push_back(PxClothParticle(PxVec3(pNext->vPos.x, pNext->vPos.y, pNext->vPos.z), fInvMass));
		m_vecClothBone.push_back(pNext);

		pCurr = pNext;

		if (0 == pNode->Childrens.size())
		{
			nIndex++;
			string sNextRootNode = "CoatCloth_0";
			char szBuf[16] = {};

			_itoa_s(nIndex, szBuf, 10);
			sNextRootNode += szBuf;

			if (7 == nIndex)
				sNextRootNode += "_04";
			else
				sNextRootNode += "_01";

			pNode = m_pSkeletonMesh->GetNode(sNextRootNode);

			if (nullptr == pNode)
				break;

			pCurr = nullptr;
			pNext = nullptr;
			fInvMass = 0.f;
			continue;
		}

		fInvMass = 1.f;
		//fInvMass += 0.25f;
		pNode = pNode->Childrens[0];
	}

	PxClothMeshDesc meshDesc;

	meshDesc.setToDefault();

	meshDesc.points.count = m_vecClothParticle.size();
	meshDesc.points.stride = sizeof(PxClothParticle);
	meshDesc.points.data = m_vecClothParticle.data();

	//meshDesc quad data 설정
	for (UINT nCoulmn = 0; nCoulmn < 7; nCoulmn++)
	{
		for (UINT nRow = 0; nRow < 4; nRow++)
		{
			m_vecIndices.push_back(nCoulmn * 5 + nRow);
			m_vecIndices.push_back(nCoulmn * 5 + nRow + 5);
			m_vecIndices.push_back(nCoulmn * 5 + nRow + 5 + 1);
			m_vecIndices.push_back(nCoulmn * 5 + nRow + 1);
		}
	}

	meshDesc.quads.count = 28;
	meshDesc.quads.stride = sizeof(PxU32) * 4;	
	meshDesc.quads.data = m_vecIndices.data();
	
	PxClothFabric* pFabric = PxClothFabricCreate(*Physics::GetPxPhysics(), meshDesc, PxVec3(0.f, -4.5f, 0.f));

	//
	if (false == meshDesc.isValid())
	{
		return E_FAIL;
	}

	PxTransform pose = PxTransform(PxIdentity);
	pCloth = Physics::GetPxPhysics()->createCloth(pose, *pFabric, m_vecClothParticle.data(), PxClothFlags());


	Physics::AddActor(GetSceneID(), *pCloth);

	return S_OK;
};

HRESULT TestObject::Awake()
{
	return S_OK;
}

HRESULT TestObject::Start()
{
	return S_OK;
}

UINT TestObject::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (Input::GetKey(DIK_I))
	{
		PxTransform pose = pCloth->getGlobalPose();

		pose.p += PxVec3(1.f, 0.f, 0.f);

		pCloth->setTargetPose(pose);
	}
	return 0;
}

UINT TestObject::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void TestObject::Editor()
{
	GameObject::Editor();
}


void TestObject::OnEnable()
{
	GameObject::OnEnable();
}

void TestObject::OnDisable()
{
	GameObject::OnDisable();
}
