#include "stdafx.h"

#include "..\Header\CbsTrail.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "Cbs_Short.h"
#include "ParticleSystem.h"

CbsTrail::CbsTrail()
{
	VtxBuffers.fill(nullptr);
	IdxBuffers.fill(nullptr);
}

void CbsTrail::Free()
{
	GameObject::Free();

	for (auto& _VtxBuf : VtxBuffers)
	{
		if (_VtxBuf)
		{
			_VtxBuf->Release();
		}
	}
	for (auto& _IdxBuf : IdxBuffers)
	{
		if (_IdxBuf)
		{
			_IdxBuf->Release();
		}
	}

	if (VtxDecl)
	{
		VtxDecl->Release();
	}
};

std::string CbsTrail::GetName()
{
	return "CbsTrail";
};

CbsTrail* CbsTrail::Create()
{
	return new CbsTrail{};
};

void CbsTrail::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	_RenderUpdateInfo.World = FMath::Identity();
};

void CbsTrail::RenderInit()
{
	m_nTag = Eff_CbsTrail;
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

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"CbsTrail",[this](const DrawInfo& _Info)
			{
				this->RenderTrail(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	const int32 TriCnt = 24;

	_Desc.VtxSize = sizeof(Vertex::TrailVertex);
	_Desc.VtxCnt = TriCnt+2;
	// 반드시 짝수로 매칭 . 
	_Desc.TriCnt = TriCnt;
	_Desc.DrawTriCnt = TriCnt;
	_Desc.IdxSize = sizeof(Vertex::Index32);
	_Desc.IdxFmt = D3DFMT_INDEX32;

	_Desc.UpdateCycle = 0.00f;
	_Desc.NewVtxCnt = 0;
	UV0Multiply = 1.f;

	Device = g_pDevice;
	VtxDecl = Vertex::TrailVertex::GetVertexDecl(Device);

	for (uint32 i = 0; i < BoneCnt; ++i)
	{
		Device->CreateVertexBuffer
		(_Desc.VtxSize * _Desc.VtxCnt, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			NULL,
			D3DPOOL_DEFAULT,
			&VtxBuffers[i], nullptr);

		Device->CreateIndexBuffer
		(_Desc.IdxSize * _Desc.TriCnt
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, _Desc.IdxFmt,
			D3DPOOL_DEFAULT, &IdxBuffers[i], nullptr);
	}


	TrailMap = Resources::Load<Texture>(
		"..\\..\\Usable\\tex_03_common_000_0002_alpg.tga");
	IceMap = Resources::Load<Texture >(
		  "..\\..\\Resource\\Texture\\Effect\\mesh_03_debris_ice00_00_albm.tga");
	IceTrailMap =Resources::Load<Texture >(
		"..\\..\\Usable\\tex_03_common_000_0001_alpg.tga");
	EmissiveMskMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	NoiseMap = Resources::Load<Texture>(
		"..\\..\\Usable\\noisesample_msk.tga");

};

void CbsTrail::PlayStart(const Mode _Mode,
	const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	if (auto _GameObject = FindGameObjectWithTag(Tag_Cbs_Short).lock();
		_GameObject)
	{
		if (auto _CbsShort = std::dynamic_pointer_cast<Cbs_Short>(_GameObject);
			_CbsShort)
		{
			const auto _CbsWorld = _CbsShort->_RenderUpdateInfo.World;
			for (int32 i = 0; i < BoneCnt; ++i)
			{
				Vertex::TrailVertex* VtxPtr{};
				auto& VtxBuffer = VtxBuffers[i];
				VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);

				auto Low = _CbsShort->Get_BoneMatrixPtr(BoneNames[i]);
				LatelyLocations[i].first = FMath::Mul(Offset[CurMode].first, *Low * _CbsWorld);

				auto High = Low;
				LatelyLocations[i].second = FMath::Mul(Offset[CurMode].second, *High * _CbsWorld);

				for (int32 j = 0; j  < _Desc.VtxCnt; j+=2)
				{
					VtxPtr[j + 1].Location = LatelyLocations[i].second;
					VtxPtr[j].Location = LatelyLocations[i].first;
				};
				VtxBuffer->Unlock();
			}
		}
	};

	for (auto& _IdxBuffer : IdxBuffers)
	{
		void* IdxPtr{ nullptr };
		_IdxBuffer->Lock(0, 0, (void**)&IdxPtr, NULL);
		ZeroMemory(IdxPtr, _Desc.IdxSize * _Desc.TriCnt);
		_IdxBuffer->Unlock();
	}

	CurMode = static_cast<uint32>(_Mode);
	_RenderProperty.bRender = true;
	T = 0.0f;
	_Desc.NewVtxCnt = 0;
	_Desc.CurVtxUpdateCycle = 0.0f;
	bVertexPoolFull = 0u;
};

void CbsTrail::PlayEnd()
{
	_RenderProperty.bRender = false;
	bVertexPoolFull = 0u;
};

void CbsTrail::RenderTrail(const DrawInfo& _Info)
{
	if (T <= 0.25f)return;

	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	if (CurMode == Mode::IceAge)
	{
		_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
		_Info.Fx->SetTexture("TrailMap", IceTrailMap->GetTexture());
		_Info.Fx->SetTexture("AlbmMap", IceMap->GetTexture());

		_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());
		_Info.Fx->SetFloatArray("ScrollSpeed", ScrollSpeed, 3);
		_Info.Fx->SetFloatArray("Scale", Scale, 3);

		_Info.Fx->SetFloatArray("NoiseDistortion0", NoiseDistortion0,2u);
		_Info.Fx->SetFloatArray("NoiseDistortion1", NoiseDistortion1,2u);
		_Info.Fx->SetFloatArray("NoiseDistortion2", NoiseDistortion2,2u);

		_Info.Fx->SetFloat("EmissiveIntencity", EmissiveIntencity);
	}
	else if (CurMode == Mode::Non)
	{
		_Info.Fx->SetFloat("DistortionIntencity", NonDistortionIntencity);
		_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());
		_Info.Fx->SetTexture("AlbmMap", nullptr);
		_Info.Fx->SetFloat("EmissiveIntencity", 0.0f);
	}

	_Info.Fx->SetTexture("EmissiveMskMap", EmissiveMskMap->GetTexture());
	

	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);

	for (int32 i = 0; i < BoneCnt; ++i)
	{
		Device->SetStreamSource(0, VtxBuffers[i], 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffers[i]);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);
	}
}

void CbsTrail::BufferUpdate(const float DeltaTime)
{
	_Desc.CurVtxUpdateCycle -= DeltaTime;

	if (_Desc.CurVtxUpdateCycle < 0.0f)
	{
		_Desc.CurVtxUpdateCycle += _Desc.UpdateCycle;

		IndexBufUpdate();
		VertexBufUpdate();
	}
}
void CbsTrail::ParticleUpdate(const float DeltaTime)
{
	CurParticleCycle-= DeltaTime;

	if (CurParticleCycle)
	{
		CurParticleCycle += ParticleCycle;

		if (auto SpTransform = m_pTransform.lock();
			SpTransform)
		{
			auto _PlayableParticle = 
				ParticleSystem::GetInstance()->PlayParticle("IceCbsMid",300ul,true);
			// 얼음 뿌리셈
			
			for (int32 i = 0; i < _PlayableParticle.size();
				++i)
			{
				auto& _PlayInstance = _PlayableParticle[i];

				const uint32 BoneIdx = FMath::Random(0u, BoneCnt - 1); 
				
				const Vector3 WorldLocation = 
					FMath::Lerp(LatelyLocations[BoneIdx].first,
					LatelyLocations[BoneIdx].second,
					FMath::Random(0.5f,1.f) );

				const Vector3 Scale = SpTransform->GetScale();
				const Vector3 Rotation = Vector3{ 0.f,0.f,0.f };

				_PlayInstance->PlayDescBind(
					FMath::WorldMatrix(Scale, Rotation, WorldLocation));
			}		
		}
	}
}
void CbsTrail::VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr)
{
	// 곡선 보간 .....
	for (int32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		{
			Vector3 VtxPt{};

			const int32 p0 = std::clamp<int32>(i - 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p1 = std::clamp<int32>(i, 0, _Desc.NewVtxCnt - 1);
			const int32 p2 = std::clamp<int32>(i + 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p3 = std::clamp<int32>(i + 4, 0, _Desc.NewVtxCnt - 1);

			D3DXVec3CatmullRom(
				&VtxPt,
				&VtxPtr[p0].Location,
				&VtxPtr[p1].Location,
				&VtxPtr[p2].Location,
				&VtxPtr[p3].Location,
				CurveT);

			VtxPtr[i].Location = VtxPt;
		}

		{
			Vector3 VtxPt{};

			const int32 p0 = std::clamp<int32>((i + 1) - 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p1 = std::clamp<int32>((i + 1), 0, _Desc.NewVtxCnt - 1);
			const int32 p2 = std::clamp<int32>((i + 1) + 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p3 = std::clamp<int32>((i + 1) + 4, 0, _Desc.NewVtxCnt - 1);

			D3DXVec3CatmullRom(
				&VtxPt,
				&VtxPtr[p0].Location,
				&VtxPtr[p1].Location,
				&VtxPtr[p2].Location,
				&VtxPtr[p3].Location,
				CurveT);

			VtxPtr[i + 1].Location = VtxPt;
		}
	}
};

void CbsTrail::VertexBufUpdate()
{
	for (int32 i = 0; i < BoneCnt; ++i)
	{
		auto& VtxBuffer = VtxBuffers[i];

		Vertex::TrailVertex* VtxPtr{};
		VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);
		// 최대 버텍스 카운트를 초과한 경우 .

		if (_Desc.NewVtxCnt > _Desc.VtxCnt)
		{


			static constexpr uint32 RemoveCount = 2;
			_Desc.NewVtxCnt -= RemoveCount;

			for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
			{
				VtxPtr[i].Location = VtxPtr[RemoveCount + i].Location;
				VtxPtr[i + 1].Location = VtxPtr[RemoveCount + i + 1].Location;
			}
		}

		if (auto _GameObject = FindGameObjectWithTag(Tag_Cbs_Short).lock();
			_GameObject)
		{
			if (auto _CbsShort = std::dynamic_pointer_cast<Cbs_Short>(_GameObject);
				_CbsShort)
			{
				const auto CbsShortWorld = _CbsShort->_RenderUpdateInfo.World;
				// _CbsShort->GetComponent<Transform>().lock()->GetWorldMatrix();

				auto Low = _CbsShort->Get_BoneMatrixPtr(BoneNames[i]);
				
				LatelyLocations[i].first = FMath::Mul(Offset[CurMode].first, *Low * CbsShortWorld);

				auto High = Low;
				LatelyLocations[i].second = FMath::Mul(Offset[CurMode].second, *High * CbsShortWorld);

				VtxPtr[_Desc.NewVtxCnt + 1].Location = LatelyLocations[i].second;
				VtxPtr[_Desc.NewVtxCnt].Location = LatelyLocations[i].first;
			}
		};

		_Desc.NewVtxCnt += 2;
		VtxUVCalc(VtxPtr);
		VtxSplineInterpolation(VtxPtr);

		if (bEdit)
		{
			for (int32 i = 0; i < BoneCnt; ++i)
			{
				auto& _CurVtxLog = _VtxLog[i];
				_CurVtxLog.resize(_Desc.NewVtxCnt);
				for (size_t j = 0; j < _CurVtxLog.size(); ++j)
				{
					_CurVtxLog[j] = VtxPtr[j];
				}
			}
		}

		VtxBuffer->Unlock();
	};

};

void CbsTrail::IndexBufUpdate()
{
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
	for (uint32 i = 0; i < BoneCnt; ++i)
	{
		Vertex::Index32* IdxPtr{ nullptr };
		auto& IdxBuffer = IdxBuffers[i];
		IdxBuffer->Lock(0, 0, (void**)&IdxPtr, 0);

		for (uint32 i = 0; i < _Desc.TriCnt; i += 2)
		{
			IdxPtr[i]._0 = i;
			IdxPtr[i]._1 = i + 1;
			IdxPtr[i]._2 = i + 3;

			IdxPtr[i + 1]._0 = i;
			IdxPtr[i + 1]._1 = i + 3;
			IdxPtr[i + 1]._2 = i + 2;
		}

		if (bEdit)
		{
			auto& _CurIdxLog = _IdxLog[i];
			_CurIdxLog.resize(_Desc.TriCnt);
			for (size_t j = 0; j < _CurIdxLog.size(); ++j)
			{
				_CurIdxLog[j] = IdxPtr[j];
			}
		}

		IdxBuffer->Unlock();
	}
};

void CbsTrail::VtxUVCalc(Vertex::TrailVertex* const VtxPtr)
{
	for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		VtxPtr[i + 1].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)) ,0.f };
		VtxPtr[i].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)),1.f };

		VtxPtr[i + 1].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2),0.f };
		VtxPtr[i].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2) ,1.f };
	}
};

void CbsTrail::RenderDebug(const DrawInfo& _Info)
{
	if (T <= 0.25f)return;

	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	for (int32 i = 0; i < BoneCnt; ++i)
	{
		auto& VtxBuffer = VtxBuffers[i];
		auto& IdxBuffer = IdxBuffers[i];

		Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffer);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.NewVtxCnt);
	}
};


HRESULT CbsTrail::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT CbsTrail::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT CbsTrail::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT CbsTrail::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;
	T += _fDeltaTime;
	BufferUpdate(_fDeltaTime);
	ParticleUpdate(_fDeltaTime);

	return 0;
}

UINT CbsTrail::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void CbsTrail::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::Begin(ChildName.c_str());
		{
			static int32 _Mode;
			ImGui::InputInt("Mode", &_Mode);
			if (ImGui::SmallButton("Play"))
			{
				PlayStart(static_cast<CbsTrail::Mode>(_Mode));
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}
		

			for (auto& _Vtx : _VtxLog)
			{
				for (auto& _CurVtx : _Vtx)
				{
					ImGui::Text("Location x %9.6f y %9.6f z %9.6f", 
							_CurVtx.Location.x, _CurVtx.Location.y ,_CurVtx.Location.z);
					ImGui::Text(" UV0 %9.6f ,%9.6f", _CurVtx.UV0.x, _CurVtx.UV0.y);
					ImGui::Text(" UV1 %9.6f ,%9.6f", _CurVtx.UV1.x, _CurVtx.UV1.y);
				}
			}
			
			
			ImGui::SliderFloat3("LowOffset",Offset[CurMode].first, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat3("HighOffset", Offset[CurMode].second, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat("UpdateCycle", &_Desc.UpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderInt("DrawTriCnt", &_Desc.DrawTriCnt, 0, _Desc.TriCnt);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat("NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat3("Noise Scale", Scale, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat3("Noise ScrollSpeed", ScrollSpeed, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion0", NoiseDistortion0, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion0", NoiseDistortion0, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion1", NoiseDistortion1, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion1", NoiseDistortion1, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion2", NoiseDistortion2, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion2", NoiseDistortion2, FLT_MIN, 100.f, "%9.6f");


			ImGui::InputFloat("ColoIntencity", &ColorIntencity, FLT_MIN,1.f,"%9.6f");
			ImGui::InputFloat("EmissiveIntencity", &EmissiveIntencity, FLT_MIN, 1.f, "%9.6f");
			
			ImGui::ColorEdit4("Color", _Color);
			ImGui::SliderFloat("UV0Multiply", &UV0Multiply,0.f,10.f,"%1.6f");
			ImGui::SliderFloat("CurveT", &CurveT, 0.f, 1.f);
		}
		ImGui::End();
	}
}


void CbsTrail::OnEnable()
{
	GameObject::OnEnable();
}

void CbsTrail::OnDisable()
{
	GameObject::OnDisable();
};



