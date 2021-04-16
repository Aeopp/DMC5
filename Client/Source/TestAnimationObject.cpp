#include "stdafx.h"
#include "..\Header\TestAnimationObject.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <filesystem>


void TestAnimationObject::Free()
{
}

std::string TestAnimationObject::GetName()
{
	return "TestObject";
};

TestAnimationObject* TestAnimationObject::Create()
{
	//에베베
	return new TestAnimationObject{};
};

void TestAnimationObject::RenderInit()
{
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 속성 전체 초기화 
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_dsSK",
		[this](const DrawInfo& _Info)
			{
				RenderGBufferSK(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"ShadowSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadowSK(_Info);
		}
	} };
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
	_SkeletonMesh = Resources::Load<ENGINE::SkeletonMesh>
		("C:\\WorkingDirectory\\TestResource\\Boss\\Em5300\\Em5300.X", _InitInfo);
	_SkeletonMesh->LoadAnimationFromDirectory
	(L"..\\..\\Resource\\Mesh\\Dynamic\\_AnimationFBX_C4D");

	//  애니메이션을 원하는 만큼 로딩하고 애니메이션 데이터를 제이슨 테이블에 있는 데이터로 덮어 씌운다. 파일이 존재하지 않는다면 덮어씌우지 않는다.
	//  애니메이션 데이터 로딩이 끝난뒤에 호출해줘야 한다. 
	_SkeletonMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante.Animation");
	// ToRoot 매트릭스를 클론마다 저장한다 . (1. 디버그 본 렌더링 필요할시 2. 본 위치가 CPU 에서도 필요할시 )

	_SkeletonMesh->EnableToRootMatricies();

	// 디폴트 이름 말고 원하는 이름으로 루트모션 켜기 . 
	// (필요없는 루트모션 정보는 이름을 "" 으로 입력)
	// _SkeletonMesh->EnableRootMotion("스케일루트모션루트이름", "로테이션...", "이동...");

	PushEditEntity(_SkeletonMesh.get());
	// Prev VTF 켜기 . (모션블러등 이전 스키닝 정보가 필요할 경우
	_SkeletonMesh->EnablePrevVTF();
	// Prev VTF 끄기 . (런타임에 끌수 있으며 자주 켜고 끄는 것은 좋지않음)
	// _SkeletonMesh->DisablePrevVTF();

	/* 애니메이션에 따라서 콜백이 필요한 경우 설정.
	return true 인 경우 콜백 하고나서 삭제
	return false 인 경우 콜백 하고 나서 다음 루프때도 다시 콜백.
	*/
	ENGINE::AnimNotify _Notify{};
	_Notify.Event[0.5] = [this]() {  Log("0.5 Sec Call");  return true; };
	_Notify.Event[0.9] = [this]() {  Log("0.9 Sec Call");  return false; };
}

void TestAnimationObject::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
		if (_SkeletonMesh)
		{
			const uint32  Numsubset = _SkeletonMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = _SkeletonMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
}

void TestAnimationObject::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = _SkeletonMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		_SkeletonMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = _SkeletonMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		}; 
	};
}
void TestAnimationObject::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = _SkeletonMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		_SkeletonMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = _SkeletonMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void TestAnimationObject::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	_SkeletonMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
};

void TestAnimationObject::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = _SkeletonMesh->GetNumSubset();
	
	if (Numsubset > 0)
	{
		_SkeletonMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = _SkeletonMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
};

HRESULT TestAnimationObject::Ready()
{
	RenderInit();

	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.0005,0.0005,0.0005 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());

	return S_OK;
};

HRESULT TestAnimationObject::Awake()
{
	return S_OK;
}

HRESULT TestAnimationObject::Start()
{
	return S_OK;
}

UINT TestAnimationObject::Update(const float _fDeltaTime)
{
	auto [DeltaScale,DeltaQuat,DeltaPos ] = _SkeletonMesh->Update(_fDeltaTime);
	 Vector3 Axis = { 1,0,0 };

	 const float Length = FMath::Length(DeltaPos);

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(SpTransform->GetPosition() + DeltaPos);
	}

	return 0;
}

UINT TestAnimationObject::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void TestAnimationObject::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{
		// 에디터 .... 
	}
}


void TestAnimationObject::OnEnable()
{

}

void TestAnimationObject::OnDisable()
{

}
