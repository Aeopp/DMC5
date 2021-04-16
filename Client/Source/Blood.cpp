#include "stdafx.h"
#include "..\Header\Blood.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Blood::Free()
{

}

std::string Blood::GetName()
{
	return "Blood";
};

Blood* Blood::Create()
{
	return new Blood{};
}


void Blood::RenderForwardAlphaBlendImplementation(
	const DrawInfo& _ImplInfo)
{
	const uint64 NumSubset = _StaticMesh->GetNumSubset();
	for (uint64 SubsetIdx = 0u; SubsetIdx < NumSubset; ++SubsetIdx)
	{
		auto WeakSubset = _StaticMesh->GetSubset(m_iIndex);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			const auto& VtxBufDesc = SharedSubset->GetVertexBufferDesc();
			SharedSubset->BindProperty(TextureType::DIFFUSE, 0u, "ALBM0Map", _ImplInfo.Fx);
			SharedSubset->BindProperty(TextureType::NORMALS, 0u, "NRMR0Map", _ImplInfo.Fx);
			SharedSubset->Render(_ImplInfo.Fx);
		}
	}
};

void Blood::RenderDebugImplementation(const DrawInfo& _ImplInfo)
{
	const uint64 NumSubset = _StaticMesh->GetNumSubset();
	for (uint64 SubsetIdx = 0u; SubsetIdx < NumSubset; ++SubsetIdx)
	{
		auto WeakSubset = _StaticMesh->GetSubset(SubsetIdx);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			SharedSubset->Render(_ImplInfo.Fx);
		}
	}
};

void Blood::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderProperty.bRender = true;
		ENGINE::RenderInterface::UpdateInfo _UpdateInfo{};
		_UpdateInfo.World = _SpTransform->GetWorldMatrix();
	}
}

HRESULT Blood::Ready()
{
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;
	// 넘겨준 패스에서는 렌더링 호출 보장 . 
	/*_InitRenderProp.RenderOrders =
	{
		RenderProperty::Order::ForwardAlphaBlend,
		RenderProperty::Order::Debug
	};*/
	RenderInterface::Initialize(_InitRenderProp);
	// 

	// 

	// 스태틱 메쉬 로딩
	_StaticMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Blood_4\\Blood4.fbx");
	PushEditEntity(_StaticMesh.get());

	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.3f,0.3f,0.3f });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// PushEditEntity(_ShaderInfo.ForwardAlphaBlendShader.get());

	return S_OK;
};

HRESULT Blood::Awake()
{

	return S_OK;
}

HRESULT Blood::Start()
{
	return S_OK;
}

UINT Blood::Update(const float _fDeltaTime)
{
	if (Input::GetKeyDown(DIK_Y))
	{
		if (m_bTest == true)
			m_bTest = false;
		else if (m_bTest == false)
			m_bTest = true;
	}

	if (m_bTest == true)
	{
		m_fTextTime += _fDeltaTime * 40;
	}
	else
		m_fTextTime = 0.f;

	if (m_fTextTime >= 1.f)
	{
		m_iIndex++;
		m_fTextTime = 0.f;
	}

	if (m_iIndex >= 17)
		m_iIndex = 0;
	return 0;
}

UINT Blood::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Blood::Editor()
{
	GameObject::Editor();
}


void Blood::OnEnable()
{

}

void Blood::OnDisable()
{

}
