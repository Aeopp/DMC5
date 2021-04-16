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
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ���� ) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = true;
	// �Ѱ��� �н������� ������ ȣ�� ���� . 
	/*_InitRenderProp.RenderOrders =
	{
		RenderProperty::Order::ForwardAlphaBlend,
		RenderProperty::Order::Debug
	};*/
	RenderInterface::Initialize(_InitRenderProp);
	// 

	// 

	// ����ƽ �޽� �ε�
	_StaticMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Blood_4\\Blood4.fbx");
	PushEditEntity(_StaticMesh.get());

	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.3f,0.3f,0.3f });
	PushEditEntity(InitTransform.lock().get());

	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
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
