#include "stdafx.h"

#include "..\Header\JudgementSwordTrail.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "ParticleSystem.h"
#include "JudgementSword.h"

JudgementSwordTrail::JudgementSwordTrail()
{
	VtxBuffers.fill(nullptr);
	IdxBuffers.fill(nullptr);
}

void JudgementSwordTrail::Free()
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

std::string JudgementSwordTrail::GetName()
{
	return "JudgementSwordTrail";
};

JudgementSwordTrail* JudgementSwordTrail::Create()
{
	return new JudgementSwordTrail{};
};

void JudgementSwordTrail::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 .
	_RenderUpdateInfo.World = FMath::Identity();
};

void JudgementSwordTrail::RenderInit()
{
	m_nTag = Eff_JudgementSwordTrail;
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
		{"JudgementSwordTrail",[this](const DrawInfo& _Info)
			{
				this->RenderTrail(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	const int32 TriCnt = 4;

	_Desc.VtxSize = sizeof(Vertex::TrailVertex);
	_Desc.VtxCnt = TriCnt + 2;
	// 반드시 짝수로 매칭 . 
	_Desc.TriCnt = TriCnt;
	_Desc.DrawTriCnt = TriCnt;
	_Desc.IdxSize = sizeof(Vertex::Index32);
	_Desc.IdxFmt = D3DFMT_INDEX32;

	_Desc.UpdateCycle = 0.016f;
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
		"..\\..\\Resource\\Texture\\Effect\\mesh_03_cs_trailmap_53_002_msk1.tga");

	NoiseMap = Resources::Load<Texture>(
		"..\\..\\Usable\\noisesample_msk.tga");

	_DynamicLight.Color =
	{
		LightColorLow,
		LightColorHigh
	};

	_DynamicLight.Flux =
	{
		FluxLow,
		FluxHigh
	};

	_DynamicLight.PointRadius =
	{
		RadiusLow,
		RadiusHigh
	};
};

void JudgementSwordTrail::PlayStart(const Mode _Mode)
{
	if (auto _GameObject = FindGameObjectWithType<JudgementSword>().lock();
		_GameObject)
	{
		if (auto _JudgementSword = std::dynamic_pointer_cast<JudgementSword>(_GameObject);
			_JudgementSword)
		{
			const auto _World = _JudgementSword->GetComponent<Transform>().lock()->GetWorldMatrix();

			for (int32 BoneIdx = 0; BoneIdx < BoneCnt; ++BoneIdx)
			{
				Vertex::TrailVertex* VtxPtr{};
				auto& VtxBuffer = VtxBuffers[BoneIdx];
				VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);

				const Vector3 LowOffset = CurModeLowOffset();
				auto Low = _JudgementSword->Get_BoneMatrixPtr(BoneLowNames[BoneIdx]);
				LatelyOffsets[BoneIdx].first = FMath::Mul(LowOffset, *Low * _World);

				const Vector3  HighOffset = CurModeHighOffset();
				auto High = _JudgementSword->Get_BoneMatrixPtr(BoneHighNames[BoneIdx]);
				LatelyOffsets[BoneIdx].second = FMath::Mul(HighOffset, *High * _World);

				PrevBoneWorld = BoneWorld;
				BoneWorld = *Low * _World;

				for (int32 j = 0; j  < _Desc.VtxCnt; j+=2)
				{
					VtxPtr[j].Location = LatelyOffsets[BoneIdx].first;
					VtxPtr[j + 1].Location = LatelyOffsets[BoneIdx].second;
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

	_DynamicLight.PlayStart(Vector3{0.f,0.f,0.f},
						(std::numeric_limits<float>::max)());
};

void JudgementSwordTrail::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
	_DynamicLight.PlayEnd();
};

void JudgementSwordTrail::ParticleUpdate(const float DeltaTime)
{
	CurJudgementDayParticleDelta -= DeltaTime;
	if (CurJudgementDayParticleDelta < 0.0f)
	{
		CurJudgementDayParticleDelta += JudgementDayParticleDelta;
		PlayParticle();
	};
};

void JudgementSwordTrail::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		// ..... Look 
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"JudgementDay", 111ul, true);
			_Particle.empty() == false)
		{
			/*const Vector3 Prev{ PrevBoneWorld._41,PrevBoneWorld._42 ,PrevBoneWorld._43 };
			const Vector3 Cur{ BoneWorld._41,BoneWorld._42 ,BoneWorld._43 };

			static const Vector3 WorldUp{ 0.f,1.f,0.f };

			const Vector3 Look = FMath::Normalize(Cur - Prev);
			const Vector3 Right = FMath::Normalize(FMath::Cross(WorldUp, Look));
			const Vector3 Up = FMath::Normalize(FMath::Cross(Look,Right));
			const Vector3 CurLocation = FMath::Lerp(Prev, Cur, 0.5f);

			Matrix CurWorld{ FMath::Identity() };

			CurWorld._11 = Right.x;
			CurWorld._12 = Right.y;
			CurWorld._13 = Right.z;

			CurWorld._21 = Up.x;
			CurWorld._22 = Up.y;
			CurWorld._23 = Up.z;

			CurWorld._31 = Look.x;
			CurWorld._32 = Look.y;
			CurWorld._33 = Look.z;

			CurWorld._41 = CurLocation.x;
			CurWorld._42 = CurLocation.y;
			CurWorld._43 = CurLocation.z;

			CurWorld._44 = 1.f;*/

			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(BoneWorld);
			}
		}
	};
};

void JudgementSwordTrail::RenderTrail(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
	_Info.Fx->SetFloatArray("ScrollSpeed", ScrollSpeed,3u);
	_Info.Fx->SetFloatArray("Scale", Scale, 3u);
	_Info.Fx->SetFloatArray("NoiseDistortion0", NoiseDistortion0, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion1", NoiseDistortion1, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion2", NoiseDistortion2, 2u);

	_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());
	_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());
	
	for (int32 i = 0; i < BoneCnt; ++i)
	{
		Device->SetStreamSource(0, VtxBuffers[i], 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffers[i]);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.NewVtxCnt);
	}
}

Vector3 JudgementSwordTrail::GetTrailLocation()
{
	Vector3 Result{0,0,0};

	for (auto&  [Low,High] : LatelyOffsets)
	{
		Result += Low;
		Result += High;
	}

	Result /= static_cast<float> (LatelyOffsets.size() * 2.f);

	return Result;
}

void JudgementSwordTrail::BufferUpdate(const float DeltaTime)
{
	_Desc.CurVtxUpdateCycle -= DeltaTime;

	if (_Desc.CurVtxUpdateCycle < 0.0f)
	{
		_Desc.CurVtxUpdateCycle += _Desc.UpdateCycle;

		IndexBufUpdate();
		VertexBufUpdate();
	}
};

void JudgementSwordTrail::VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr)
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

void JudgementSwordTrail::VertexBufUpdate()
{
	for (int32 BoneIdx = 0; BoneIdx < BoneCnt; ++BoneIdx)
	{
		auto& VtxBuffer = VtxBuffers[BoneIdx];

		Vertex::TrailVertex* VtxPtr{};
		VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);
		// 최대 버텍스 카운트를 초과한 경우 .

		if (_Desc.NewVtxCnt > _Desc.VtxCnt)
		{
			static constexpr uint32 RemoveCount = 2;
			_Desc.NewVtxCnt -= RemoveCount;

			for (uint32 VtxIdx = 0; VtxIdx < _Desc.NewVtxCnt; VtxIdx += 2)
			{
				VtxPtr[VtxIdx + 1].Location = VtxPtr[RemoveCount + VtxIdx + 1].Location;
				VtxPtr[VtxIdx].Location = VtxPtr[RemoveCount + VtxIdx].Location;
			}
		}

		
		if (auto _GameObject = FindGameObjectWithType<JudgementSword>().lock();
			_GameObject)
		{
			if (auto _JudgementSword = std::dynamic_pointer_cast<JudgementSword>(_GameObject);
				_JudgementSword)
			{
				const auto _World = _JudgementSword->GetComponent<Transform>().lock()->GetWorldMatrix();
				
				const Vector3 LowOffset = CurModeLowOffset();
				auto Low = _JudgementSword->Get_BoneMatrixPtr(BoneLowNames[BoneIdx]);
				LatelyOffsets[BoneIdx].first =   FMath::Mul(LowOffset, *Low * _World);
				
				const Vector3  HighOffset= CurModeHighOffset();
				auto High = _JudgementSword->Get_BoneMatrixPtr(BoneHighNames[BoneIdx]);
				LatelyOffsets[BoneIdx].second = FMath::Mul(HighOffset, *High * _World);

				PrevBoneWorld = BoneWorld;
				BoneWorld = *Low * _World;

				VtxPtr[_Desc.NewVtxCnt].Location = LatelyOffsets[BoneIdx].first;
				VtxPtr[_Desc.NewVtxCnt + 1].Location = LatelyOffsets[BoneIdx].second;
			}
		};

		_Desc.NewVtxCnt += 2;
		VtxUVCalc(VtxPtr);
		VtxSplineInterpolation(VtxPtr);
		
		for (int32 i = 0; i < BoneCnt; ++i)
		{
			auto& _CurVtxLog = _TrailVtxWorldLocations[i];
			_CurVtxLog.resize(_Desc.NewVtxCnt);
			for (size_t j = 0; j < _CurVtxLog.size(); ++j)
			{
				_CurVtxLog[j] = VtxPtr[j];
			}
		}

		VtxBuffer->Unlock();
	};

};

void JudgementSwordTrail::IndexBufUpdate()
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
	for (uint32 IdxIdx  = 0; IdxIdx < BoneCnt; ++IdxIdx)
	{
		Vertex::Index32* IdxPtr{ nullptr };
		auto& IdxBuffer = IdxBuffers[IdxIdx];
		IdxBuffer->Lock(0, 0, (void**)&IdxPtr, 0);

		for (uint32 j = 0; j < _Desc.TriCnt; j+= 2)
		{
			IdxPtr[j]._0 = j;
			IdxPtr[j]._1 = j + 1;
			IdxPtr[j]._2 = j + 3;

			IdxPtr[j + 1]._0 = j;
			IdxPtr[j + 1]._1 = j + 3;
			IdxPtr[j + 1]._2 = j + 2;
		}

		if (bEdit)
		{
			auto& _CurIdxLog = _IdxLog[IdxIdx];
			_CurIdxLog.resize(_Desc.TriCnt);

			for (size_t j = 0; j < _CurIdxLog.size(); ++j)
			{
				_CurIdxLog[j] = IdxPtr[j];
			}
		}

		IdxBuffer->Unlock();
	}
};

void JudgementSwordTrail::VtxUVCalc(Vertex::TrailVertex* const VtxPtr)
{
	for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		VtxPtr[i + 1].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)) ,0.f };
		VtxPtr[i].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)),1.f };

		VtxPtr[i + 1].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2),0.f };
		VtxPtr[i].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2) ,1.f };
	}
};

void JudgementSwordTrail::RenderDebug(const DrawInfo& _Info)
{
	if (T <= 0.25f)
		return;

	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	for (int32 i = 0; i < BoneCnt; ++i)
	{
		auto& VtxBuffer = VtxBuffers[i];
		auto& IdxBuffer = IdxBuffers[i];

		Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffer);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);
	}
};


HRESULT JudgementSwordTrail::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();


	return S_OK;
};

HRESULT JudgementSwordTrail::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT JudgementSwordTrail::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT JudgementSwordTrail::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	BufferUpdate(_fDeltaTime);
	ParticleUpdate(_fDeltaTime);
	
	const Vector3 Location 
		{ BoneWorld._41,BoneWorld._42 ,BoneWorld._43};
	_DynamicLight.Update(_fDeltaTime, Location);

	return 0;
}

UINT JudgementSwordTrail::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void JudgementSwordTrail::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			static int32 _Mode;
			ImGui::InputInt("Mode", &_Mode);
			if (ImGui::SmallButton("Play"))
			{
				PlayStart(static_cast<JudgementSwordTrail::Mode>(_Mode));
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}


			for (auto& _Vtx : _TrailVtxWorldLocations)
			{
				for (auto& _CurVtx : _Vtx)
				{
					ImGui::Text("Location x %9.6f y %9.6f z %9.6f",
						_CurVtx.Location.x, _CurVtx.Location.y, _CurVtx.Location.z);
					ImGui::Text(" UV0 %9.6f ,%9.6f", _CurVtx.UV0.x, _CurVtx.UV0.y);
					ImGui::Text(" UV1 %9.6f ,%9.6f", _CurVtx.UV1.x, _CurVtx.UV1.y);
				}
			}

			ImGui::SliderFloat3("LowOffset", LowOffset, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat3("HighOffset",HighOffset, -300.f, 300.f, "%9.6f");

			ImGui::SliderFloat3("JudgementLowOffset", JudgementLowOffset, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat3("JudgementHighOffset", JudgementHighOffset, -300.f, 300.f, "%9.6f");

			ImGui::SliderFloat("UpdateCycle", &_Desc.UpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderInt("DrawTriCnt", &_Desc.DrawTriCnt, 0, _Desc.TriCnt);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat3("Noise Scale", Scale, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat3("Noise ScrollSpeed", ScrollSpeed, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion0", NoiseDistortion0, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion0", NoiseDistortion0, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion1", NoiseDistortion1, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion1", NoiseDistortion1, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion2", NoiseDistortion2, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion2", NoiseDistortion2, FLT_MIN, 100.f, "%9.6f");


			ImGui::InputFloat("ColoIntencity", &ColorIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::ColorEdit4("Color", _Color);
			ImGui::SliderFloat("UV0Multiply", &UV0Multiply, 0.f, 10.f, "%1.6f");
			ImGui::SliderFloat("CurveT", &CurveT, 0.f, 1.f);

			ImGui::ColorEdit4("LightColorLow", LightColorLow);
			ImGui::ColorEdit4("LightColorHigh", LightColorHigh);

			ImGui::SliderFloat("FluxLow", &FluxLow, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat("FluxHigh", &FluxHigh, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat("RadiusLow", &RadiusLow, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat("RadiusHigh", &RadiusHigh, FLT_MIN, 100.f, "%9.6f");

			

			_DynamicLight.Color =
			{
				LightColorLow,
				LightColorHigh
			};

			_DynamicLight.Flux =
			{
				FluxLow,
				FluxHigh
			};

			_DynamicLight.PointRadius =
			{
				RadiusLow,
				RadiusHigh
			};

		}
		ImGui::EndChild();
	}
};

void JudgementSwordTrail::OnEnable()
{
	GameObject::OnEnable();
}

void JudgementSwordTrail::OnDisable()
{
	GameObject::OnDisable();
};





