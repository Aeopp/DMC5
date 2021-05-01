#include "stdafx.h"
#include "MapObject.h"
#include "Subset.h"

MapObject::MapObject()
{
}

void MapObject::Free()
{
	m_pStaticMesh.reset();
	GameObject::Free();
}

MapObject* MapObject::Create()
{
	MapObject* pInstance = new MapObject;
	return pInstance;
}

HRESULT MapObject::Ready()
{
	GameObject::Ready();

	return S_OK;
}

HRESULT MapObject::Awake()
{
	GameObject::Awake();
	return S_OK;
}

HRESULT MapObject::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT MapObject::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	return 0;
}

UINT MapObject::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void MapObject::OnEnable()
{
}

void MapObject::OnDisable()
{
}

#pragma region RENDER
void MapObject::RenderReady()
{
	if (auto _SpTransform = m_pTransform.lock(); _SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (m_pStaticMesh)
		{
			const uint32  Numsubset = m_pStaticMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = m_pStaticMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
}

void MapObject::RenderInit()
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

	if (bEmissive == false)
	{
		_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
		{
			{"gbuffer_ds",
			[this](const DrawInfo& _Info)
				{
					RenderGBuffer(_Info);
				}
			},
		};
	}
	else
	{
		_InitRenderProp.RenderOrders[RenderProperty::Order::Emissive] =
		{
			{"Emissive",
			[this](const DrawInfo& _Info)
				{
					RenderEmissive(_Info);
				}
			},
		};
	}

	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"Shadow" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadow(_Info);
		}
	} };

	//_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
	//	=
	//{
	//	{"Debug" ,
	//	[this](const DrawInfo& _Info)
	//	{
	//		RenderDebug(_Info);
	//	}
	//} };
	RenderInterface::Initialize(_InitRenderProp);
}

void MapObject::RenderGBuffer(const DrawInfo& _Info)
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


			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void MapObject::RenderShadow(const DrawInfo& _Info)
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

void MapObject::RenderEmissive(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	_Info.Fx->SetFloat("EmissivePower", EmissivePower);
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

			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void MapObject::RenderDebug(const DrawInfo& _Info)
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
}
#pragma endregion

void MapObject::SetUp(
	const TSTRING _sMesh, 
	const D3DXVECTOR3& _vScale,
	const D3DXVECTOR3& _vRotation, 
	const D3DXVECTOR3 _vPosition)
{
	m_pStaticMesh = Resources::Load<ENGINE::StaticMesh>(_sMesh);

	{
		filesystem::path sFileName = _sMesh;
		sFileName = sFileName.filename();

		if (sFileName == "electornicbillboard.fbx")
		{
			bEmissive = false;
		};
	}


	D3DXVECTOR3 vScale = _vScale * GScale;
	D3DXVECTOR3 vPos = _vPosition * GScale;
	m_pTransform.lock()->SetScale(vScale);
	m_pTransform.lock()->SetRotation(_vRotation);
	m_pTransform.lock()->SetPosition(vPos);


	RenderInit();
}
