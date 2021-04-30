#include "stdafx.h"
#include "..\Header\Trail.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"

Trail::Trail()
{

}

void Trail::Free()
{
	GameObject::Free();

	if (VtxBuffer)
	{
		VtxBuffer->Release();
	}
	if (IdxBuffer)
	{
		IdxBuffer->Release();
	}
	if (VtxDecl)
	{
		VtxDecl->Release();
	}
};

std::string Trail::GetName()
{
	return "Trail";
};

Trail* Trail::Create()
{
	return new Trail{};
};

void Trail::RenderReady()
{

};

void Trail::RenderInit()
{
	m_nTag = Eff_Trail;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,[this](const DrawInfo& _Info)
			{
				DrawCollider(_Info);
			}
		}
	};

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"Trail",[this](const DrawInfo& _Info)
			{
				this->RenderTrail(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	_Desc.VtxSize = sizeof(Vertex::TrailVertex);
	_Desc.VtxCnt = 12;
	// 반드시 짝수로 매칭 . 
	_Desc.TriCnt = 10;
	_Desc.IdxSize = sizeof(Vertex::Index32);
	_Desc.IdxFmt = D3DFMT_INDEX32;
	_Desc.UpdateCycle = 0.0f;
	_Desc.DrawTriCnt = 10;



	_Desc.NewVtxCnt = 0;
	

	Device = g_pDevice;

	VtxDecl = Vertex::TrailVertex::GetVertexDecl(Device);
	Device->CreateVertexBuffer
	(_Desc.VtxSize * _Desc.VtxCnt, 0, 
		NULL, 
		D3DPOOL_MANAGED, 
		&VtxBuffer, nullptr);

	Device->CreateIndexBuffer
	(_Desc.IdxSize* _Desc.TriCnt
		, 0, _Desc.IdxFmt, 
		D3DPOOL_MANAGED, &IdxBuffer, nullptr);
	TrailMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\mesh_03_cs_trailmap_53_002_msk1.tga");

};

void Trail::PlayStart(const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	T = 0.0f;
	_Desc.NewVtxCnt = 0;
	_RenderProperty.bRender = true;
};

void Trail::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void Trail::RenderTrail(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());
	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity); 
	Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
	Device->SetVertexDeclaration(VtxDecl);
	Device->SetIndices(IdxBuffer);
	_Info.Fx->CommitChanges();
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);

	// const Matrix World = _RenderUpdateInfo.World;
	/*_Info.Fx->SetMatrix("matWorld", &World);

	_Color.w = (FMath::Clamp((EndT - T) / EndT + MinAlpha, 0.0f, 1.f));
	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("Intencity", WaveIntencity);
	_Info.Fx->SetFloat("Progress", T);
	_Info.Fx->SetFloat("UV_VOffset", UV_VOffset);
	_Info.Fx->SetBool("bWaveDistortion", bWaveDistortion);

	if (bWaveDistortion)
	{
		_Info.Fx->SetTexture("WaveMaskMap", _WaveMask->GetTexture());
	}

	const uint32 Numsubset = _WaveCircle->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _WaveCircle->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};*/
};

void Trail::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
	Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
	Device->SetVertexDeclaration(VtxDecl);
	Device->SetIndices(IdxBuffer);
	_Info.Fx->CommitChanges();
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);
};


HRESULT Trail::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT Trail::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT Trail::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Trail::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	_Desc.CurUpdateCycle -= _fDeltaTime;
	 if (_Desc.CurUpdateCycle < 0.0f)
	// if(true ) 
	{
		 _Desc.CurUpdateCycle += _Desc.UpdateCycle;
		/*
		1 3 5 7 9 11.......
		0 2 4 6 8 10 .....

		1번 삼각형
		1 →  3
		↑ ↙
		0

		2번 삼각형
		   3
		 ↗ ↓
		0 ← 2
		*/

		 Vertex::Index32* IdxPtr{ nullptr };
		IdxBuffer->Lock(0, 0, (void**)&IdxPtr, 0);
		_IdxLog.resize(_Desc.TriCnt);
		for (uint32 i = 0; i < _Desc.TriCnt; i += 2)
		{
			IdxPtr[i]._0 = i;
			IdxPtr[i]._1 = i + 1;
			IdxPtr[i]._2 = i + 3;

			_IdxLog[i] = IdxPtr[i];

			IdxPtr[i + 1]._0 = i;
			IdxPtr[i + 1]._1 = i + 3;
			IdxPtr[i + 1]._2 = i + 2;

			_IdxLog[i + 1] = IdxPtr[i + 1];
		}

		IdxBuffer->Unlock();



		Vertex::TrailVertex* VtxPtr{};
		VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);
		// 최대 버텍스 카운트를 초과한 경우 .

		if (_Desc.NewVtxCnt > _Desc.VtxCnt)
		{
			static constexpr uint32 RemoveCount = 2;
			_Desc.NewVtxCnt -= RemoveCount;
			_VtxLog.resize(_Desc.NewVtxCnt);

			for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
			{
				VtxPtr[i].Location = VtxPtr[RemoveCount + i].Location;
				VtxPtr[i + 1].Location = VtxPtr[RemoveCount + i + 1].Location;

				_VtxLog[i].Location = VtxPtr[i].Location;
				_VtxLog[i + 1].Location = VtxPtr[i + 1].Location;
			}
		}

		if (auto _GameObject = FindGameObjectWithTag(TAG_RedQueen).lock();
			_GameObject)
		{
			if (auto RQ = std::dynamic_pointer_cast<RedQueen>(_GameObject);
				RQ)
			{
				const auto SwordWorld = RQ->GetComponent<Transform>().lock()->GetWorldMatrix();

				auto Low = RQ->Get_BoneMatrixPtr("_000") ;
				const Vector3 LowPos = FMath::Mul(LowOffset, *Low * SwordWorld);

				auto High = RQ->Get_BoneMatrixPtr("_001");
				const Vector3 HighPos = FMath::Mul(HighOffset, *High * SwordWorld);

				VtxPtr[_Desc.NewVtxCnt + 1].Location = HighPos;
				VtxPtr[_Desc.NewVtxCnt].Location = LowPos;
				_RenderUpdateInfo.World = FMath::Identity();
			}
		};





		// 여기서 위치 대입 (월드까지 적용해서 대입하거나 쉐이더에서 월드 곱하거나)
		// VtxPtr[_Desc.NewVtxCnt].Location   = ? 
		// VtxPtr[_Desc.NewVtxCnt+1].Location = ?;

		for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
		{
			VtxPtr[i + 1].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2),0.f };
			VtxPtr[i].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2) ,1.f };

			VtxPtr[i + 1].UV0 = {  ( (float)i / ((float)_Desc.NewVtxCnt - 2) ) * UV0Multiply,0.f };
			VtxPtr[i].UV0 = { ( (float)i / ((float)_Desc.NewVtxCnt - 2 ) )* UV0Multiply ,1.f };
		}

		_Desc.NewVtxCnt += 2;

		// 곡선 보간 .....
		for (int32 i = 0; i < _Desc.NewVtxCnt; ++i)
		{
			Vector3 VtxPt{}; 
			D3DXVec3CatmullRom(
				&VtxPt,
				&VtxPtr[i - 1].Location,
				&VtxPtr[i].Location,
				&VtxPtr[i + 1].Location,
				&VtxPtr[i + 2].Location,
				CurveT);
		}

		VtxBuffer->Unlock();
	}

	return 0;
}

UINT Trail::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void Trail::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				PlayStart();
			}

			if (_VtxLog.empty() == false)
			{
				for (int32 i = 0; i < _Desc.VtxCnt; ++i)
				{
					ImGui::Text("Vtx %d : %9.6f %9.6f %9.6f", i,
						_VtxLog[i].Location.x, _VtxLog[i].Location.y, _VtxLog[i].Location.z);
				}
			}
			
			if (_IdxLog.empty() == false)
			{
				for (int32 i = 0; i < _Desc.TriCnt; ++i)
				{
					ImGui::Text("Idx %d : %d %d %d", i,
						_IdxLog[i]._0, _IdxLog[i]._1, _IdxLog[i]._2);
				}
			}

			ImGui::SliderFloat3("LowOffset", LowOffset, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat3("HighOffset", HighOffset, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat("UpdateCycle", &_Desc.UpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderInt("DrawTriCnt", &_Desc.DrawTriCnt, 0, _Desc.TriCnt);
			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::ColorEdit4("Color", _Color);
			ImGui::SliderFloat("UV0Multiply", &UV0Multiply,0.f,10.f,"%1.6f");
			ImGui::SliderFloat("CurveT", &CurveT, 0.f, 1.f);

	/*		ImGui::Text("T : %2.6f", T);
			ImGui::BulletText("r %1.6f g %1.6f b %1.6f a %1.6f", _Color.x, _Color.y, _Color.z, _Color.w);

			ImGui::Checkbox("bWaveDistortion", &bWaveDistortion);
			ImGui::SliderFloat("WaveScale", &WaveScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("WaveSpeed", &WaveSpeed, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("WaveIntencity", &WaveIntencity, -1.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("MinAlpha", &MinAlpha, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("UV_VOffset", &UV_VOffset, -1.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("EndT", &EndT, -1.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);*/
		}
		ImGui::EndChild();
	}
}


void Trail::OnEnable()
{
	GameObject::OnEnable();
}

void Trail::OnDisable()
{
	GameObject::OnDisable();
};



