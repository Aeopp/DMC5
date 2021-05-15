#include "stdafx.h"
#include "..\Header\CollObject.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"

void CollObject::Free()
{
	m_pStaticMesh.reset();
	GameObject::Free();
}

std::string CollObject::GetName()
{
	return "CollObject";
};

CollObject* CollObject::Create()
{
	return new CollObject{};
};

void CollObject::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (m_pStaticMesh)
		{
			const uint32  Numsubset = m_pStaticMesh->GetNumSubset();  
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset ;  ++i)
			{
				const auto& _Subset = m_pStaticMesh->GetSubset(i); 
				const auto& _CurBS =_Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i]= _CurBS.Transform(_RenderUpdateInfo.World, Scale.x); 
			}
		}
	}
}

void CollObject::RenderInit()
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
	_InitRenderProp.bShadowCache = true;

	
	
};

void CollObject::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset =m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
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
void CollObject::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
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

void CollObject::SetUp(
	const TSTRING _sMesh, 
	const D3DXVECTOR3& _vScale, 
	const D3DXVECTOR3& _vRotation, 
	const D3DXVECTOR3 _vPosition)
{
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;


	m_pStaticMesh = Resources::Load<ENGINE::StaticMesh>(_sMesh, _InitInfo);

	filesystem::path sFileName = _sMesh;
	sFileName = sFileName.filename();

	D3DXVECTOR3 vScale = _vScale * GScale;
	D3DXVECTOR3 vPos = _vPosition * GScale;
	m_pTransform.lock()->SetScale(vScale);
	m_pTransform.lock()->SetRotation(_vRotation);
	m_pTransform.lock()->SetPosition(vPos);

	PushEditEntity(m_pStaticMesh.get());


	m_pCollider = AddComponent<MeshCollider>();
	m_pCollider.lock()->ReadyMeshCollider(m_pStaticMesh->GetVerticesPointer(), m_pStaticMesh->GetNumVertices(), m_pStaticMesh->GetIndicesPointer(), m_pStaticMesh->GetNumIndices());
	m_pCollider.lock()->SetRigid(false);
	m_pCollider.lock()->SetGravity(false);
	PushEditEntity(m_pCollider.lock().get());

	RenderInit();
}


void CollObject::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
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


HRESULT CollObject::Ready()
{
	GameObject::Ready();


	return S_OK;
};

HRESULT CollObject::Awake()
{
	GameObject::Awake();
		
	return S_OK;
}

HRESULT CollObject::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT CollObject::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	return 0;
}

UINT CollObject::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void CollObject::Editor()
{
	GameObject::Editor();
}


void CollObject::OnEnable()
{
	GameObject::OnEnable();
}

void CollObject::OnDisable()
{
	GameObject::OnDisable();
}
