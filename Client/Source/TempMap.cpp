#include "stdafx.h"
#include "..\Header\TempMap.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"

void TempMap::Free()
{
	GameObject::Free();
}

std::string TempMap::GetName()
{
	return "TempMap";
};

TempMap* TempMap::Create()
{
	return new TempMap{};
};

void TempMap::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (_StaticMesh)
		{
			const uint32  Numsubset = _StaticMesh->GetNumSubset();  
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset ;  ++i)
			{
				const auto& _Subset = _StaticMesh->GetSubset(i); 
				const auto& _CurBS =_Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i]= _CurBS.Transform(_RenderUpdateInfo.World, Scale.x); 
			}
		}
	}
}

void TempMap::RenderInit()
{
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;
	//_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	//{
	//	{"gbuffer_ds",
	//	[this](const DrawInfo& _Info)
	//		{
	//			RenderGBuffer(_Info);
	//		}
	//	},
	//};
	//_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
	//	=
	//{
	//	{"Shadow" ,
	//	[this](const DrawInfo& _Info)
	//	{
	//		RenderShadow(_Info);
	//	}
	//} };

	//_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
	//	=
	//{
	//	{"Debug" ,
	//	[this](const DrawInfo& _Info)
	//	{
	//		RenderDebug(_Info);
	//	}
	//} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
		}
	} };

	RenderInterface::Initialize(_InitRenderProp);

	// 
	// 스태틱 메쉬 로딩

	
};

void TempMap::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset =_StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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
void TempMap::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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

void TempMap::LoadMap(const int _iStage)
{
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	switch (_iStage)
	{
	case 1:
		_StaticMesh = Resources::Load<ENGINE::StaticMesh>(
			L"..\\..\\Resource\\Map\\Collmap\\Stage1.fbx", _InitInfo);
		break;
	case 2:
		_StaticMesh = Resources::Load<ENGINE::StaticMesh>(
			L"..\\..\\Resource\\Map\\Collmap\\Stage2.fbx", _InitInfo);
		break;
	case 3:
		_StaticMesh = Resources::Load<ENGINE::StaticMesh>(
			L"..\\..\\Resource\\Map\\Collmap\\Stage3.fbx", _InitInfo);
		break;
	case 4:
		_StaticMesh = Resources::Load<ENGINE::StaticMesh>(
			L"..\\..\\Resource\\Map\\Collmap\\BossStage.fbx", _InitInfo);
		break;
	case 5:
		_StaticMesh = Resources::Load<ENGINE::StaticMesh>(
			L"..\\..\\Resource\\Map\\Collmap\\Stage4.fbx", _InitInfo);
		break;
	default:
		break;
	}
	PushEditEntity(_StaticMesh.get());
	
}


void TempMap::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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


HRESULT TempMap::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ GScale,GScale,GScale });
	//InitTransform.lock()->SetPosition(Vector3{ -3672.85 ,57.237 ,22512.534}*GScale) ;

	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT TempMap::Awake()
{
	m_pCollider = AddComponent<MeshCollider>();
	m_pCollider.lock()->ReadyMeshCollider(_StaticMesh->GetVerticesPointer(), _StaticMesh->GetNumVertices(), _StaticMesh->GetIndicesPointer(), _StaticMesh->GetNumIndices());
	m_pCollider.lock()->SetRigid(false);
	m_pCollider.lock()->SetGravity(false);
	m_pCollider.lock()->SetRayCastTarget(true);
	PushEditEntity(m_pCollider.lock().get());

	//auto pCollider = AddComponent<CapsuleCollider>();
	//pCollider.lock()->ReadyCollider();
	//PushEditEntity(pCollider.lock().get());
	return S_OK;
}

HRESULT TempMap::Start()
{
	return S_OK;
}

UINT TempMap::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	/*Vector3 vDir = m_pTransform.lock()->GetLook();

	D3DXVec3Normalize(&vDir, &vDir);*/
	/*if (Input::GetKey(DIK_UP))
		m_pTransform.lock()->Translate(vDir * _fDeltaTime * 10.f);
	if (Input::GetKey(DIK_DOWN))
		m_pTransform.lock()->Translate(-vDir * _fDeltaTime * 10.f);
	if (Input::GetKey(DIK_LEFT))
		m_pTransform.lock()->Rotate({ 0.f, D3DXToRadian(180 * -_fDeltaTime * 50.f), 0.f });
	if (Input::GetKey(DIK_RIGHT))
		m_pTransform.lock()->Rotate({ 0.f, D3DXToRadian(180 * _fDeltaTime * 50.f), 0.f });*/
		

	
	return 0;
}

UINT TempMap::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void TempMap::Editor()
{
	GameObject::Editor();
}


void TempMap::OnEnable()
{
	GameObject::OnEnable();
}

void TempMap::OnDisable()
{
	GameObject::OnDisable();
}
