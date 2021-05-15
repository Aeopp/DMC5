#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "Renderer.h"
#include "FileHelper.h"
#include "GraphicSystem.h"
#include <ostream>
#include <fstream>
#include <istream>
#include "FMath.hpp"
#include "Color.h"
#include "DxHelper.h"
#include "Resources.h"
#include "TimeSystem.h"
#include "Vertexs.h"
#include <filesystem>
#include <d3d9.h>
#include <d3dx9.h>
#include "StaticMesh.h"
#include "Subset.h"
#include "ParticleSystem.h"
#include <string_view>



USING(ENGINE)
IMPLEMENT_SINGLETON(Renderer)

Renderer::Renderer() {}

void Renderer::Free()
{
	TestShaderRelease();
};


HRESULT Renderer::ReadyRenderSystem(LPDIRECT3DDEVICE9 const _pDevice)
{
	Device = _pDevice;
	SafeAddRef(Device);
	ReadyRenderTargets();
	ReadyShader("..\\..\\Resource\\Shader");
	ReadyLights();
	ReadyFrustum();
	ReadyQuad();
	ReadySky();
	ReadyTextures();
	ReadyPtLightPool();

	TestShaderInit();

	return S_OK;
};

void Renderer::ReadyShader(const std::filesystem::path& TargetPath)
{
	std::filesystem::directory_iterator itr(TargetPath);

	for (const std::filesystem::directory_entry& entry :
		std::filesystem::recursive_directory_iterator(TargetPath))
	{
		const auto& CurPath = entry.path();
		if (CurPath.has_extension())
		{
			const std::string& ShaderKey = CurPath.stem().string();
			Shaders[ShaderKey] = Resources::Load<Shader>(CurPath);
		}
	}
};


void Renderer::ReadySky()
{
	Mesh::InitializeInfo InitInfo{};
	InitInfo.bLocalVertexLocationsStorage = false;

	SkysphereMesh = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Sphere.fbx", InitInfo);

	SkyTexMission02Sun = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Sky\\mission02\\First.dds");
	SkyTexMission02Sunset = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Sky\\mission02\\Second.dds");
	SkyNoiseMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Sky\\mission02\\smoke_01_iam.tga");
}

void Renderer::ReadyLights()
{

}

void Renderer::ReadyRenderTargets()
{
	static const Vector2 RenderTargetDebugRenderSize{ 80.f,80.f };

	const  float InitX =
		g_nWndCX / -2.f + (RenderTargetDebugRenderSize.x);
	const  float InitY =
		g_nWndCY / +2.f - (RenderTargetDebugRenderSize.y);

	const float YOffset = -RenderTargetDebugRenderSize.y * 2.f;
	const float XOffset = RenderTargetDebugRenderSize.x * 2.f;
	const float Interval = 5.f;

	{
		auto& SceneTarget = RenderTargets["SceneTarget"] =
			std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo;
		InitInfo.Width = g_nWndCX;
		InitInfo.Height = g_nWndCY;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A16B16G16R16F;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		SceneTarget->Initialize(InitInfo);
		SceneTarget->DebugBufferInitialize(
			{ InitX,InitY },
			RenderTargetDebugRenderSize);
	}

	{
		auto& ALBM = RenderTargets["ALBM"] = std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo;
		InitInfo.Width = g_nWndCX;
		InitInfo.Height = g_nWndCY;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A8R8G8B8;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		ALBM->Initialize(InitInfo);
		ALBM->DebugBufferInitialize(
			{ InitX,InitY + (YOffset * 1.f) + Interval },
			RenderTargetDebugRenderSize);
	}

	{
		auto& NRMR = RenderTargets["NRMR"] = std::make_shared<RenderTarget>();


		RenderTarget::Info InitInfo;
		InitInfo.Width = g_nWndCX;
		InitInfo.Height = g_nWndCY;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A8R8G8B8;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		NRMR->Initialize(InitInfo);
		NRMR->DebugBufferInitialize(
			{ InitX,InitY + (YOffset * 2.f) + Interval },
			RenderTargetDebugRenderSize);
	}

	{
		auto& Depth = RenderTargets["Depth"] = std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo;
		InitInfo.Width = g_nWndCX;
		InitInfo.Height = g_nWndCY;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_R32F;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		Depth->Initialize(InitInfo);
		Depth->DebugBufferInitialize(
			{ InitX,InitY + (YOffset * 3.f) + Interval },
			RenderTargetDebugRenderSize   );
	}

	{
		auto& Distortion = RenderTargets["Distortion"] = std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo{};
		InitInfo.Width = g_nWndCX; 
		InitInfo.Height = g_nWndCY;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A8R8G8B8;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;

		Distortion->Initialize(InitInfo);
		Distortion->DebugBufferInitialize(
			{ InitX,InitY + (YOffset * FLT_MAX) + Interval },
			RenderTargetDebugRenderSize);
	}

	{
		auto& Emissive = RenderTargets["Emissive"] = std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo{};
		InitInfo.Width = g_nWndCX;
		InitInfo.Height = g_nWndCY;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A16B16G16R16F;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		Emissive->Initialize(InitInfo);
		Emissive->DebugBufferInitialize(
			{ InitX,InitY + (YOffset * FLT_MAX) + Interval },
			RenderTargetDebugRenderSize);
	}

	{
		auto& avgluminance = RenderTargets["avgluminance"] =
			std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo{};
		InitInfo.Width = 64;
		InitInfo.Height = 64;
		InitInfo.Levels = 0;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_R16F;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		avgluminance->Initialize(InitInfo);
		avgluminance->DebugBufferInitialize(
			{ InitX + (XOffset * 1.f) + Interval ,InitY + Interval },
			RenderTargetDebugRenderSize);

		for (int32 i = 1; i < 4; ++i)
		{
			avgluminance->InsertSurface(i * 2);
		}
	}

	{
		auto& avglumsystemmem = RenderTargets["avglumsystemmem"] =
			std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo{};
		InitInfo.Width = 1;
		InitInfo.Height = 1;
		InitInfo.Levels = 0;
		InitInfo.Usages = 0;
		InitInfo.Format = D3DFMT_R16F;
		InitInfo._D3DPool = D3DPOOL_SYSTEMMEM;
		avglumsystemmem->Initialize(InitInfo);
		avglumsystemmem->DebugBufferInitialize(
			{ InitX + (XOffset * 1.f) + Interval,InitY + (YOffset * 1.f) + Interval },
			RenderTargetDebugRenderSize);
	}

	{
		auto& bloomresult = RenderTargets["bloomresult"] =
			std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo{};
		InitInfo.Width = g_nWndCX / 2;
		InitInfo.Height = g_nWndCY / 2;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A16B16G16R16F;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		bloomresult->Initialize(InitInfo);
		bloomresult->DebugBufferInitialize(
			{ InitX + (XOffset * 1.f) + Interval , InitY + (YOffset * 2.f) + Interval },
			RenderTargetDebugRenderSize);
	};

	{
		auto& starresult = RenderTargets["starresult"] =
			std::make_shared<RenderTarget >();
		RenderTarget::Info InitInfo{};
		InitInfo.Width = g_nWndCX / 4;
		InitInfo.Height = g_nWndCY / 4;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A16B16G16R16F;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		starresult->Initialize(InitInfo);
		starresult->DebugBufferInitialize(
			{ InitX + (XOffset * 1.f) + Interval , InitY + (YOffset * 3.f) + Interval },
			RenderTargetDebugRenderSize);
	}

	for (int i = 0; i < 5; ++i)
	{
		{
			const std::string key = "dsampletargets" + std::to_string(i);

			auto& dsampletargets = RenderTargets[key] =
				std::make_shared<RenderTarget>();
			RenderTarget::Info InitInfo{};
			InitInfo.Width = g_nWndCX / (2 << i);
			InitInfo.Height = g_nWndCY / (2 << i);
			InitInfo.Levels = 1;
			InitInfo.Usages = D3DUSAGE_RENDERTARGET;
			InitInfo.Format = D3DFMT_A16B16G16R16F;
			InitInfo._D3DPool = D3DPOOL_DEFAULT;
			dsampletargets->Initialize(InitInfo);

			dsampletargets->DebugBufferInitialize
			(
				{ InitX + (XOffset * 2.f) + Interval,
				InitY + (YOffset * i) + (i > 0 ? Interval : 0) },
				RenderTargetDebugRenderSize);
		}

		{
			const std::string key = "blurtargets" + std::to_string(i);

			auto& blurtargets = RenderTargets[key] =
				std::make_shared<RenderTarget>();
			RenderTarget::Info InitInfo{};
			InitInfo.Width = g_nWndCX / (2 << i);
			InitInfo.Height = g_nWndCY / (2 << i);
			InitInfo.Levels = 1;
			InitInfo.Usages = D3DUSAGE_RENDERTARGET;
			InitInfo.Format = D3DFMT_A16B16G16R16F;
			InitInfo._D3DPool = D3DPOOL_DEFAULT;
			blurtargets->Initialize(InitInfo);

			blurtargets->DebugBufferInitialize
			(
				{ InitX + (XOffset * 3.f) + Interval,
				  InitY + (YOffset * i) + (i > 0 ? Interval : 0) },
				RenderTargetDebugRenderSize);
		}
	}

	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				const std::string key = "startargets" +
					std::to_string(i) + std::to_string(j);

				auto& startargets = RenderTargets[key] =
					std::make_shared<RenderTarget>();

				RenderTarget::Info InitInfo{};
				InitInfo.Width = g_nWndCX / 4;
				InitInfo.Height = g_nWndCY / 4;
				InitInfo.Levels = 1;
				InitInfo.Usages = D3DUSAGE_RENDERTARGET;
				InitInfo.Format = D3DFMT_A16B16G16R16F;
				InitInfo._D3DPool = D3DPOOL_DEFAULT;
				startargets->Initialize(InitInfo);
				startargets->DebugBufferInitialize(
					{ InitX + (XOffset * (4 + j)) + Interval,
					InitY + (YOffset * i) + (i > 0 ? Interval : 0) },
					RenderTargetDebugRenderSize);
			}
		}
	};

	for (int i = 0; i < 2; ++i)
	{
		{
			const std::string key = "lensflaretargets" + std::to_string(i);

			auto& lensflaretargets = RenderTargets[key] = std::make_shared<RenderTarget>();

			RenderTarget::Info InitInfo{};
			InitInfo.Width = g_nWndCX / 2;
			InitInfo.Height = g_nWndCY / 2;
			InitInfo.Levels = 1;
			InitInfo.Usages = D3DUSAGE_RENDERTARGET;
			InitInfo.Format = D3DFMT_A16B16G16R16F;
			InitInfo._D3DPool = D3DPOOL_DEFAULT;
			lensflaretargets->Initialize(InitInfo);
			lensflaretargets->DebugBufferInitialize
			(
				{ InitX + (XOffset * 6) + Interval,
				  InitY + (YOffset * i) + (i > 0 ? Interval : 0) },
				RenderTargetDebugRenderSize
			);
		}

		{
			const std::string key = "afterimagetargets" + std::to_string(i);

			auto& afterimagetargets = RenderTargets[key] =
				std::make_shared<RenderTarget>();

			RenderTarget::Info InitInfo{};
			InitInfo.Width = g_nWndCX / 2;
			InitInfo.Height = g_nWndCY / 2;
			InitInfo.Levels = 1;
			InitInfo.Usages = D3DUSAGE_RENDERTARGET;
			InitInfo.Format = D3DFMT_A16B16G16R16F;
			InitInfo._D3DPool = D3DPOOL_DEFAULT;
			afterimagetargets->Initialize(InitInfo);
			afterimagetargets->DebugBufferInitialize
			(
				{ InitX + (XOffset * 6) + Interval,
					  InitY + (YOffset * (i + 2)) + (i > 0 ? Interval : 0) },
				RenderTargetDebugRenderSize
			);
		}
	}


};

void Renderer::ReadyRenderInfo()
{
	Matrix CameraView, CameraProjection, Ortho;

	Device->GetTransform(D3DTS_VIEW, &CameraView);
	Device->GetTransform(D3DTS_PROJECTION, &CameraProjection);

	D3DXMatrixOrthoLH(&Ortho, g_nWndCX, g_nWndCY, 0.0f, 1.f);

	_RenderInfo.View = (CameraView);
	_RenderInfo.Projection = (CameraProjection);
	_RenderInfo.ViewInverse = FMath::Inverse(_RenderInfo.View);
	_RenderInfo.Billboard = _RenderInfo.ViewInverse;
	_RenderInfo.Billboard._41 = 0.f;
	_RenderInfo.Billboard._42 = 0.f;
	_RenderInfo.Billboard._43 = 0.f;

	_RenderInfo.ProjectionInverse = FMath::Inverse(_RenderInfo.Projection);
	_RenderInfo.ViewProjection =
		CameraView * CameraProjection;
	_RenderInfo.ViewProjectionInverse = FMath::Inverse(_RenderInfo.ViewProjection);
	_RenderInfo.Eye =
	{ _RenderInfo.ViewInverse._41  , _RenderInfo.ViewInverse._42,_RenderInfo.ViewInverse._43,1.f };
	_RenderInfo.Ortho = Ortho;

	CameraFrustum->Make(_RenderInfo.ViewInverse, _RenderInfo.Projection);
	Device->GetViewport(&_RenderInfo.Viewport);
}

void Renderer::ReadyFrustum()
{
	CameraFrustum = std::make_shared<Frustum>();
	CameraFrustum->Initialize(Device);
	CurShadowFrustum = std::make_shared<Frustum>();
	CurShadowFrustum->Initialize(Device);
}

void Renderer::ReadyQuad()
{
	_Quad = std::make_shared<Quad>();
	_Quad->Initialize(Device);
}

void Renderer::ReadyTextures()
{
	DistortionTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\water_new_height.png");
};

void Renderer::ReadyPtLightPool()
{
	for (uint32 i = 0; i < 33; ++i)
	{
		auto _Insert = std::make_shared<FLight>(
			FLight(
				FLight::Type::Point, { 1.5f,0.5f, 0.0f ,1 },
				{ 1,1,1,1 }));

		DynamicPointLights.push_back(_Insert);
		_Insert->SetProjectionParameters(0, 0, 0.1f, 10.0f);
		_Insert->bEnable = false;
	}
}

void Renderer::Push(const std::weak_ptr<GameObject>& _RenderEntity)&
{
	if (auto _SharedObject = _RenderEntity.lock();
		_SharedObject)
	{
		if (_SharedObject->IsRenderEnable())
		{
			if (
				std::shared_ptr<RenderInterface> _SharedRenderEntity
				= std::dynamic_pointer_cast<RenderInterface>(_SharedObject);
				_SharedRenderEntity)
			{
				if (false == _SharedRenderEntity->GetRenderProp().bRender)return;

				const auto& _EntityRenderProp = _SharedRenderEntity->GetRenderProp();

				for (const auto& [_EntityOrder, ShaderKeyCallMap] : _EntityRenderProp.RenderOrders)
				{
					for (const auto& [ShaderKey, Call] : ShaderKeyCallMap)
					{
						RenderEntitys[_EntityOrder][ShaderKey].push_back(
							RenderEntityType{ _SharedRenderEntity.get(), Call });
						RenderEntitySet.insert(_SharedRenderEntity.get());
					}
				}
			}
		}
	}
};

// 렌더 레디에서 절두체 준비 ..
HRESULT Renderer::Render()&
{
	if (g_bOptRender)
	{
		return OptRender();
	}

	RenderReady();
	RenderBegin();

	if (bDistortion)
	{
		ClearDistortion();
	}

	//  쉐도우 패스 
	if (bShadowMapBake)
	{
		ShadowCacheBake();
	}
	else
	{
		RenderShadowMaps();
	}
	
	EnableDepthBias();
	// 기하 패스
	RenderGBuffer();
	// 디퍼드 렌더링 .
	DeferredShading();

	if (bEnvironmentRender)
	{
		RenderEnvironment();
	}
	else
	{
		RenderSkySphere();
	}

	// RenderEmissive();
	AlphaBlendEffectRender();
	if (bDistortion)
	{
		BlendDistortion();
	}
	UIRender();
	// RenderInsulatorMetal();
	{
		// 테스트 끝나면 주석 풀기
		RenderMeasureLuminance();
		const float DeltaTime = TimeSystem::GetInstance()->DeltaTime();
		AdaptLuminance(DeltaTime);
		BrightPass();
		DownSample();
		Stars();
		Bloom();
		LensFlare();
		// 백버퍼로 백업 . 
		Device->SetRenderTarget(0, BackBuffer);
		Device->SetDepthStencilSurface(BackBufferZBuffer);
		Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
		// 테스트 
		ToneMap();
		//  여기 까지 ..... 
	}

	// Tonemapping();
	//AlphaBlendEffectRender();
	UIRenderAfterPostProcessing();

	ResetState();
	RenderTargetDebugRender();
	RenderDebug();
	LightFrustumRender();
	RendererCollider();
	RenderDebugBone();
	ImguiRender();
	GraphicSystem::GetInstance()->End();
	DisableDepthBias();
	RenderEnd();
	Device->Present(NULL, NULL, NULL, NULL);
	BackBuffer->Release();
	BackBufferZBuffer->Release();

	return S_OK;
}
HRESULT Renderer::OptRender()&
{
	RenderReady();
	RenderBegin();
	EnableDepthBias();
	// 기하 패스
	Device->SetRenderTarget(0, BackBuffer);
	Device->SetDepthStencilSurface(BackBufferZBuffer);
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);

	RenderGBuffer();

	if (bEnvironmentRender)
	{
		RenderEnvironment();
	}
	else
	{
		RenderSkySphere();
	}
	// 백버퍼로 백업 . 

	// Tonemapping();
	AlphaBlendEffectRender();
	UIRender();
	UIRenderAfterPostProcessing();

	ResetState();
	RenderTargetDebugRender();
	RenderDebug();
	LightFrustumRender();
	RendererCollider();
	RenderDebugBone();
	ImguiRender();
	GraphicSystem::GetInstance()->End();
	DisableDepthBias();
	RenderEnd();
	Device->Present(NULL, NULL, NULL, NULL);
	BackBuffer->Release();

	return S_OK;
};

void Renderer::Editor()&
{
	ImGui::Begin("Render Editor");
	{
		if (ImGui::TreeNode("RenderTarget"))
		{
			for (auto& _RT : RenderTargets)
			{
				ImGui::Text(_RT.first.c_str());
				ImGui::Image(reinterpret_cast<void**>
					(_RT.second->GetTexture()), { 128,128 });
				ImGui::Separator();
			}

			ImGui::TreePop();
		}
		if (ImGui::Button("LightSave"))
		{
			LightSave(FileHelper::OpenDialogBox());
		}

		if (ImGui::Button("LightLoad"))
		{
			LightLoad(FileHelper::OpenDialogBox());
		}

		if (ImGui::Button("Shadow Cache Bake"))
		{
			bShadowMapBake = true;
		}

		ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, 0.f, 1.f,
			"%3.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("SoftParticleDepthScale", &SoftParticleDepthScale, 0.0f, 1.f);
		ImGui::InputFloat("In SoftParticleDepthScale", &SoftParticleDepthScale);

		ImGui::SliderFloat("StarScale", &StarScale, -10.f, 10.f);
		ImGui::SliderFloat("StarFactor", &StarFactor, -10.f, 10.f);
		;


		;

		ImGui::Checkbox("SRGBAlbm", &bSRGBAlbm);
		ImGui::Checkbox("SRGBNRMR", &bSRGBNRMR);
		ImGui::Checkbox("Distortion", &bDistortion);
		ImGui::Checkbox("AfterImage", &drawafterimage);
		ImGui::Checkbox("PtLightScrRtTest", &bPtLightScrRtTest);
		ImGui::Checkbox("EnvironmentRender", &bEnvironmentRender);
		ImGui::Checkbox("LightRender", &bLightRender);

		ImGui::SliderFloat("ao", &ao, 0.0f, 1.f);
		ImGui::InputFloat("In ao", &ao, 0.0f, 1.f);
		ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.f);
		ImGui::Checkbox("SkyDistortion", &SkyDistortion);
		if (SkyDistortion)
		{
			ImGui::BeginChild("SkyDistortion Edit");
			ImGui::SliderFloat("SkyNoisewrap", &SkyNoisewrap, 0.0f, 20.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);;
			ImGui::SliderFloat("SkyTimecorr", &SkyTimecorr, 0.0f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::ColorEdit4("DistortionColor", DistortionColor);
			ImGui::SliderFloat("SkyDistortionIntencity ", &SkyDistortionIntencity, 0.0f, 100.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::EndChild();
		}

		ImGui::SliderFloat("SkyIntencity", &SkyIntencity, 0.0f, 2.f);

		ImGui::SliderFloat("FogStart", &FogStart, 0.0f, 1000.f);
		ImGui::InputFloat("In FogStart", &FogStart);

		ImGui::SliderFloat("FogDistance", &FogDistance, 0.0f, 1000.f);
		ImGui::InputFloat("In FogDistance", &FogDistance);

		ImGui::ColorEdit3("FogColor", FogColor);
		ImGui::SliderFloat("FogDensity", &FogDensity, 0.0f,10.f);
		ImGui::InputFloat("In FogDensity", &FogDensity);

		ImGui::SliderFloat("SkySphereScale", &SkysphereScale, 0.f, 10.f);
		ImGui::SliderFloat3("SkySphereRot", SkysphereRot, -360.f, 360.f);
		ImGui::SliderFloat3("SkySphereLoc", SkysphereLoc, -10.f, 10.f);
		ImGui::SliderFloat("SkySphereRotSpeed", &SkyRotationSpeed, 0.0f, 1.f, "%1.6f", 0.0001f);

		/*static bool  DepthBiasButton = true;
		static float ZeroDotOne = 0.000001f;*/

		{
			ImGui::SliderFloat("SlpoeScaleDepthBias", &SlpoeScaleDepthBias, 0.f, 1.0f);
			ImGui::InputFloat("In SlpoeScaleDepthBias", &SlpoeScaleDepthBias);

			ImGui::SliderFloat("DepthBias", &DepthBias, 0.f, 10.f);
			ImGui::InputFloat("In DepthBias", &DepthBias);
		}

		if (ImGui::CollapsingHeader("AdaptLuminance"))
		{
			for (int32 i = 0; i < adaptedluminance_var.size(); ++i)
			{
				std::string label = "adpvar " + std::to_string(i);
				ImGui::SliderFloat(label.c_str(), &adaptedluminance_var[i], -1000.f, 1000.f);
			};
			if (ImGui::Button("AdaptLuminance Default"))
			{
				adaptedluminance_var = def_adaptedluminance_var;
			}
		}

		// ImGui::SliderFloat("ShadowMin", &ShadowMin, 0.0f, 1.0f);

		if (ImGui::CollapsingHeader("Add Light"))
		{
			static int32 ShadowMapSize = 0.0f;
			if (ImGui::InputInt("ShadowMapSize", &ShadowMapSize))
			{

			}

			if (ImGui::Button("Directional"))
			{
				auto _Insert = std::make_shared<FLight>(
					FLight(FLight::Type::Directional,
						{ 0,0,0,0 }, (const D3DXCOLOR&)Color::sRGBToLinear(250, 250, 250))
					);
				DirLights.push_back(_Insert);
				if (ShadowMapSize > 0)
				{
					_Insert->CreateShadowMap(Device, ShadowMapSize);
				}
				_Insert->SetProjectionParameters(7.1f, 7.1f, -20.f, +20.f);
				_Insert->InitRender();
			}

			if (ImGui::Button("Point"))
			{
				auto _Insert = std::make_shared<FLight>(
					FLight(
						FLight::Type::Point, { 1.5f,0.5f, 0.0f ,1 },
						{ 1,1,1,1 }));

				PointLights.push_back(_Insert);
				if (ShadowMapSize > 0)
				{
					_Insert->CreateShadowMap(Device, ShadowMapSize);
				}
				_Insert->SetProjectionParameters(0, 0, 0.1f, 10.0f);
				_Insert->InitRender();
			};
		};
		if (ImGui::CollapsingHeader("Lights"))
		{
			uint32 Idx = 0u;
			for (auto& _Light : PointLights)
			{
				_Light->Edit(Idx);
				++Idx;
			}
			for (auto& _Light : DirLights)
			{
				_Light->Edit(Idx);
				++Idx;
			}
		}
	}
	ImGui::End();
};

void Renderer::SkyDistortionStart()
{
	SkyDistortion = true;
};

void Renderer::SkyDistortionEnd()
{
	SkyDistortion = false;
};

void Renderer::LateSceneInit()
{
	Renderer::GetInstance()->SkyDistortionEnd();
	Renderer::GetInstance()->RequestShadowMapBake();
};

void Renderer::SceneChangeRender()
{
	CurDirLight = nullptr;
};

void Renderer::RequestShadowMapBake()
{
	bShadowMapBake = true;
};

std::weak_ptr<FLight> Renderer::RefRemainingDynamicLight()
{
	std::weak_ptr<FLight> ReturnVal{};
	for (auto& _Target : DynamicPointLights)
	{
		if (_Target)
		{
			if (_Target->bEnable == false)
			{
				ReturnVal = _Target;
				continue;
			}
		}
	}

	return  ReturnVal;
};

void Renderer::RenderReady()&
{
	RenderReadyEntitys();
	ReadyRenderInfo();
};

void Renderer::RenderBegin()&
{
	if (g_bOptRender == false)
	{
		if (CurDirLight)
		{
			if (CurDirLight->ShadowMapSize > 0)
			{
				CurDirLight->bCurrentShadowRender = true;

				if (FAILED(g_pDevice->StretchRect
				(CurDirLight->CacheDepthStencil, nullptr,
					CurDirLight->DepthStencil, nullptr, D3DTEXF_POINT)))
				{
					PRINT_LOG(TEXT("Warning"),
						TEXT("Depth stencil copy falure."));
				}
			}
			else
			{
				CurDirLight->bCurrentShadowRender = false;
			}
		}


		for (auto& PointLight : PointLights)
		{
			if ((PointLight->GetShadowMapSize() > 0) == false)
			{
				PointLight->bCurrentShadowRender = false;
				continue;
			}

			Sphere PtlightSphere{};
			PtlightSphere.Center = (D3DXVECTOR3&)PointLight->GetPosition();
			PtlightSphere.Radius = PointLight->GetFarPlane();
			if (false == CameraFrustum->IsIn(PtlightSphere))
			{
				PointLight->bCurrentShadowRender = false;
				continue;
			}

			PtlightSphere.Radius = PointLight->GetPointRadius();
			if (false == CameraFrustum->IsIn(PtlightSphere))
			{
				PointLight->bCurrentShadowRender = false;
				continue;
			}

			for (int i = 0; i < 6; ++i)
			{
				if (FAILED(g_pDevice->StretchRect
				(PointLight->CubeCacheDepthStencil[i], nullptr,
					PointLight->CubeDepthStencil[i], nullptr, D3DTEXF_POINT)))
				{
					PRINT_LOG(TEXT("Warning"), TEXT(
						"Depth stencil copy failure."));
				}
			}
			PointLight->bCurrentShadowRender = true;
		}
	}
	

	GraphicSystem::GetInstance()->Begin();
	Device->GetRenderTarget(0, &BackBuffer);
	Device->GetDepthStencilSurface(&BackBufferZBuffer);
	// Device->Clear(0, nullptr,
	// /*Device->Clear(0, nullptr,
	// 	D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
	// 	0xff00ff00, 1.0f, 0);*/
};

//   등록코드수정 
void Renderer::RenderReadyEntitys()&
{
	for (auto& _Entity : RenderEntitySet)
	{
		_Entity->RenderReady();
	}
}

void Renderer::ResetState()&
{
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	Device->SetViewport(&_RenderInfo.Viewport);
};

void Renderer::ClearDistortion()&
{
	Device->SetRenderTarget(0u, RenderTargets["Distortion"]->GetSurface(0));
	// Device->Clear(0u, nullptr, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
	 Device->Clear(0u, nullptr, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	Device->SetRenderTarget(0u, nullptr);
};

void Renderer::RenderEnd()&
{
	_PrevRenderInfo = _RenderInfo;
	RenderEntityClear();

	for (auto iter = DirLights.begin(); iter != DirLights.end(); )
	{
		if (iter->get()->bRemove)
		{
			if (iter->get() == CurDirLight)
			{
				CurDirLight = nullptr;
			}

			iter = DirLights.erase(iter);
		}
		else
			++iter;
	}
	for (auto iter = PointLights.begin(); iter != PointLights.end(); )
	{
		if (iter->get()->bRemove)
			iter = PointLights.erase(iter);
		else
			++iter;
	}
}

void Renderer::RenderEntityClear()&
{
	RenderEntitys.clear();
	RenderEntitySet.clear();
};

void Renderer::RenderShadowMaps()
{
	if (bShadowMapBake)return;

	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	auto shadowmap = Shaders["Shadow"]->GetEffect();

	if(
		CurDirLight && 
		(CurDirLight->GetShadowMapSize() > 0) 
		&& CurDirLight->bCurrentShadowRender)
	{
		CurDirLight->RenderShadowMap(
			Device, [&](FLight* light) {
			D3DXMATRIX  viewproj;
			D3DXVECTOR4 clipplanes(
				light->GetNearPlane(),
				light->GetFarPlane(), 0, 0);

			light->CalculateViewProjection(viewproj);

			shadowmap->SetTechnique("variance");
			shadowmap->SetVector("clipPlanes", &clipplanes);
			shadowmap->SetBool("isPerspective", FALSE);

			//Device->Clear(0, NULL, 
			//	D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

			// 렌더 시작 ... 
			DrawInfo _DrawInfo{};
			_DrawInfo._Device = Device;
			CurShadowFrustum->Make(light->viewinv, light->proj);
			_DrawInfo._Frustum = CurShadowFrustum.get();
			_DrawInfo.BySituation.reset();
			for (auto& [ShaderKey, EntityArr] : RenderEntitys[RenderProperty::Order::Shadow])
			{
				auto Fx = Shaders[ShaderKey]->GetEffect();
				Fx->SetMatrix("matViewProj", &viewproj);
				_DrawInfo.Fx = Fx;
				UINT Passes = 0u;
				Fx->Begin(&Passes, NULL);
				for (int32 i = 0; i < Passes; ++i)
				{
					_DrawInfo.PassIndex = i;
					Fx->BeginPass(i);
					for (auto& [_Entity, _Call] : EntityArr)
					{
						if (_Entity->_RenderProperty.bShadowCache == false)
						{
							_Call(_DrawInfo);
						}
					}
					Fx->EndPass();
				}
				Fx->End();
			}
			// 렌더 엔드 ... 

			});
	};

	shadowmap->SetBool("isPerspective", TRUE);
	for (auto& PointLight : PointLights)
	{
		if (PointLight->bCurrentShadowRender == false) 
			continue;

		PointLight->RenderShadowMap(Device, [&](FLight* light) {
			D3DXMATRIX viewproj;
			D3DXVECTOR4 clipplanes(
				light->GetNearPlane(), light->GetFarPlane(), 0, 0);

			light->CalculateViewProjection(viewproj);

			shadowmap->SetTechnique("variance");
			shadowmap->SetVector("lightPos", &light->GetPosition());
			shadowmap->SetVector("clipPlanes", &clipplanes);

			//Device->Clear(0,NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

			CurShadowFrustum->Make(light->viewinv, light->proj);
			// 렌더 시작 ... 
			DrawInfo _DrawInfo{};
			_DrawInfo._Device = Device;
			_DrawInfo._Frustum = CurShadowFrustum.get();
			// 여기까지 했음 . 내일 화이팅
			for (auto& [ShaderKey, EntityArr] :
				RenderEntitys[RenderProperty::Order::Shadow])
			{
				auto Fx = Shaders[ShaderKey]->GetEffect();
				_DrawInfo.Fx = Fx;
				UINT Passes = 0u;
				Fx->SetMatrix("matViewProj", &viewproj);
				Fx->Begin(&Passes, NULL);
				for (int32 i = 0; i < Passes; ++i)
				{
					_DrawInfo.PassIndex = i;
					Fx->BeginPass(i);
					for (auto& [_Entity, _Call] : EntityArr)
					{
						if (_Entity->_RenderProperty.bShadowCache == false)
						{
							_Call(_DrawInfo);
						}
					}
					Fx->EndPass();
				}
				Fx->End();
			}
		});
	};

	Device->SetDepthStencilSurface(BackBufferZBuffer);
	Device->SetViewport(&_RenderInfo.Viewport);
}
void Renderer::ShadowCacheBake()
{
	if (!bShadowMapBake)return;

	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	auto shadowmap = Shaders["Shadow"]->GetEffect();

	for (auto& _DirLight : DirLights)
	{
		if ((_DirLight->ShadowMapSize >= 0) == false)continue;

		_DirLight->CacheShadowMapBake(
			Device, [&](FLight* light) {
				D3DXMATRIX  viewproj;
				D3DXVECTOR4 clipplanes(
					light->GetNearPlane(),
					light->GetFarPlane(), 0, 0);

				light->CalculateViewProjection(viewproj);

				shadowmap->SetTechnique("variance");
				shadowmap->SetVector("clipPlanes", &clipplanes);
				shadowmap->SetBool("isPerspective", FALSE);

				Device->Clear(0, NULL,
					D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

				// 렌더 시작 ... 
				DrawInfo _DrawInfo{};
				_DrawInfo._Device = Device;
				CurShadowFrustum->Make(light->viewinv, light->proj);
				_DrawInfo._Frustum = CurShadowFrustum.get();
				_DrawInfo.BySituation.reset();
				for (auto& [ShaderKey, EntityArr] : RenderEntitys[RenderProperty::Order::Shadow])
				{
					auto Fx = Shaders[ShaderKey]->GetEffect();
					Fx->SetMatrix("matViewProj", &viewproj);
					_DrawInfo.Fx = Fx;
					UINT Passes = 0u;
					Fx->Begin(&Passes, NULL);
					for (int32 i = 0; i < Passes; ++i)
					{
						_DrawInfo.PassIndex = i;
						Fx->BeginPass(i);
						for (auto& [_Entity, _Call] : EntityArr)
						{
							if (_Entity->_RenderProperty.bShadowCache)
							{
								_Call(_DrawInfo);
							}
						}
						Fx->EndPass();
					}
					Fx->End();
				}

			});
	}

	shadowmap->SetBool("isPerspective", TRUE);
	for (auto& PointLight : PointLights)
	{
		if ((PointLight->ShadowMapSize >= 0) == false) continue;

		PointLight->CacheShadowMapBake(Device, [&](FLight* light) {
			D3DXMATRIX viewproj;
			D3DXVECTOR4 clipplanes(
				light->GetNearPlane(), light->GetFarPlane(), 0, 0);

			light->CalculateViewProjection(viewproj);

			shadowmap->SetTechnique("variance");
			shadowmap->SetVector("lightPos", &light->GetPosition());
			shadowmap->SetVector("clipPlanes", &clipplanes);

			Device->Clear(0,
				NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

			CurShadowFrustum->Make(light->viewinv, light->proj);
			// 렌더 시작 ... 
			DrawInfo _DrawInfo{};
			_DrawInfo._Device = Device;
			_DrawInfo._Frustum = CurShadowFrustum.get();
			// 여기까지 했음 . 내일 화이팅
			for (auto& [ShaderKey, EntityArr] :
				RenderEntitys[RenderProperty::Order::Shadow])
			{
				auto Fx = Shaders[ShaderKey]->GetEffect();
				_DrawInfo.Fx = Fx;
				UINT Passes = 0u;
				Fx->SetMatrix("matViewProj", &viewproj);
				Fx->Begin(&Passes, NULL);
				for (int32 i = 0; i < Passes; ++i)
				{
					_DrawInfo.PassIndex = i;
					Fx->BeginPass(i);
					for (auto& [_Entity, _Call] : EntityArr)
					{
						if (_Entity->_RenderProperty.bShadowCache)
						{
							_Call(_DrawInfo);
						}
					}
					Fx->EndPass();
				}
				Fx->End();
			}
			});
	};

	Device->SetDepthStencilSurface(BackBufferZBuffer);
	Device->SetViewport(&_RenderInfo.Viewport);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	bShadowMapBake = false;
};

void Renderer::RenderGBuffer()
{
	auto* const device = Device;
	// 감마보정은 쉐이딩시 수행 기하 정보를 그릴때는 필요 없음 . 
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);

	if (g_bOptRender)
	{
		device->SetRenderTarget(0, BackBuffer);
	}
	else
	{
		device->SetRenderTarget(0, RenderTargets["ALBM"]->GetSurface());
		device->SetRenderTarget(1, RenderTargets["NRMR"]->GetSurface());
		device->SetRenderTarget(2, RenderTargets["Depth"]->GetSurface());
		// 왜곡도 클리어 . 
	}

	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);
	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 알베도 노말 깊이 렌더타겟 한번에 초기화 . 
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xffffffff, 1.0f, 0);

	auto& GBufferGroup = RenderEntitys[RenderProperty::Order::GBuffer];
	DrawInfo _DrawInfo{};
	_DrawInfo.BySituation.reset();
	_DrawInfo._Device = Device;
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto& [ShaderKey, Entitys] : GBufferGroup)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		Fx->SetMatrix("matViewProj", &_RenderInfo.ViewProjection);
		_DrawInfo.Fx = Fx;

		std::sort(std::begin(Entitys), std::end(Entitys), [EyePos = _RenderInfo.Eye](
			const RenderEntityType& Lhs,
			const RenderEntityType& Rhs
			) 
			{
			const Vector3 LhsLocation
				=
			{
				Lhs.first->_RenderUpdateInfo.World._41,
				Lhs.first->_RenderUpdateInfo.World._42,
				Lhs.first->_RenderUpdateInfo.World._43
			};

			const Vector3 RhsLocation
				=
			{ Rhs.first->_RenderUpdateInfo.World._41,
				Rhs.first->_RenderUpdateInfo.World._42,
				Rhs.first->_RenderUpdateInfo.World._43
			};

			return FMath::LengthSq((Vector3&)(EyePos)-LhsLocation)
				<
				FMath::LengthSq((Vector3&)(EyePos)-RhsLocation);
			});

		for (auto& [Entity, Call] : Entitys)
		{
			UINT Passes{ 0u };
			Fx->Begin(&Passes, NULL);
			for (int32 i = 0; i < Passes; ++i)
			{
				_DrawInfo.PassIndex = i;
				Fx->BeginPass(i);
				{
					Call(_DrawInfo);

					if (bLightRender)
					{
						DWORD zwrite, zenable;
						Device->GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);
						Device->GetRenderState(D3DRS_ZENABLE, &zenable);

						Device->SetRenderState(D3DRS_ZENABLE, FALSE);
						Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
						for (auto& _Light : DirLights)
						{
							_Light->Render(_DrawInfo);
						}
						for (auto& _Light : PointLights)
						{
							_Light->Render(_DrawInfo);
						}

						Device->SetRenderState(D3DRS_ZENABLE, zenable);
						Device->SetRenderState(D3DRS_ZWRITEENABLE, zwrite);
					}

				}
				Fx->EndPass();
			}
			Fx->End();
		}
	}

	device->SetRenderTarget(1, NULL);
	device->SetRenderTarget(2, NULL);
}

void Renderer::DeferredShading()
{
	D3DXVECTOR4			pixelsize(1, 1, 1, 1);

	// STEP 2: deferred shading
	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	pixelsize.x = 1.0f / (float)_RenderInfo.Viewport.Width;
	pixelsize.y = -1.0f / (float)_RenderInfo.Viewport.Height;

	auto device = Device;

	auto scenesurface = RenderTargets["SceneTarget"]->GetSurface();
	auto albedo = RenderTargets["ALBM"]->GetTexture();
	auto normals = RenderTargets["NRMR"]->GetTexture();
	auto depth = RenderTargets["Depth"]->GetTexture();

	auto deferred = Shaders["DeferredShading"]->GetEffect();

	device->SetRenderTarget(0, scenesurface);
	// Scene 타겟은 Z-Buffer 클리어 필요 없음 . 
	device->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);

	// albm 맵에서 감마보정 수행 .
	if (bSRGBAlbm)
	{
		device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, TRUE);
	}
	else
	{
		device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, FALSE);
	}

	if (bSRGBNRMR)
	{
		device->SetSamplerState(1, D3DSAMP_SRGBTEXTURE, TRUE);
	}
	else
	{
		device->SetSamplerState(1, D3DSAMP_SRGBTEXTURE, FALSE);
	}

	for (int i = 0; i < 5; ++i) {
		device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		// 그림자를 맵 샘플링을 위한 처리 그림자맵이 그려지지 않은 곳은 그림자 없음 .
		if (i > 2)
		{
			device->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0x00000000);
			device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		}
		else
		{
			// 노말 and 깊이 
			device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
	};

	device->SetTexture(0, albedo);
	device->SetTexture(1, normals);
	device->SetTexture(2, depth);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	deferred->SetTechnique("deferred");
	deferred->SetMatrix("matViewProjInv", &_RenderInfo.ViewProjectionInverse);
	deferred->SetVector("pixelSize", &pixelsize);
	deferred->SetVector("eyePos", &_RenderInfo.Eye);


	deferred->Begin(NULL, 0);
	deferred->BeginPass(0);

	{
		D3DXMATRIX lightviewproj;
		D3DXVECTOR4 clipplanes(0, 0, 0, 0);

		if(CurDirLight)
		{
			auto& DirLight = CurDirLight;
			DirLight->CalculateViewProjection(lightviewproj);
			{
				// directional light
				deferred->SetMatrix("lightViewProj", &lightviewproj);
				deferred->SetVector("lightColor", (D3DXVECTOR4*)&DirLight->GetColor());
				deferred->SetVector("lightPos", &DirLight->GetPosition());
				deferred->SetBool("IsPoint", false);
				deferred->SetFloat("lightFlux", DirLight->lightFlux);
				deferred->SetFloat("lightIlluminance", DirLight->lightIlluminance);
				deferred->SetFloat("lightRadius", DirLight->GetPointRadius());
				deferred->SetFloat("specularPower", DirLight->specularPower);
				deferred->SetFloat("shadowmin", DirLight->shadowmin);
				deferred->SetFloat("ShadowDepthBias", DirLight->shadowdepthbias);
				deferred->SetFloat("ShadowDepthMapHeight", DirLight->GetShadowMapSize());
				deferred->SetFloat("ShadowDepthMapWidth", DirLight->GetShadowMapSize());
				deferred->SetFloat("ao", ao);
				Vector3 LightDirection = DirLight->GetDirection();
				deferred->SetFloatArray("LightDirection", LightDirection, 3);
				deferred->SetFloatArray("Lradiance", DirLight->Lradiance, 3);
				deferred->SetFloat("sinAngularRadius", DirLight->sinAngularRadius);
				deferred->SetFloat("cosAngularRadius", DirLight->cosAngularRadius);

				clipplanes.x = DirLight->GetNearPlane();
				clipplanes.y = DirLight->GetFarPlane();
				deferred->SetVector("clipPlanes", &clipplanes);
				device->SetTexture(3, DirLight->GetShadowMap());
				deferred->CommitChanges();
				_Quad->Render(Device);
			}
		}

		// 여기서부터 ..
		// point lights
		// 현재 버그 ... 
		if (bPtLightScrRtTest)
		{
			device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		}
		
		std::vector<FLight*> _PointLights{};
		for (auto& Target : PointLights)
		{
			_PointLights.push_back(Target.get());
		}
		for (auto& Target : DynamicPointLights)
		{
			if (Target->bEnable)
			{
				_PointLights.push_back(Target.get());
			}
		}


		for (auto& PointLight : _PointLights)
		{
			Sphere PtLtSp{};

			PtLtSp.Center = (D3DXVECTOR3&)PointLight->GetPosition();
			PtLtSp.Radius = PointLight->GetPointRadius();

			/*if (FMath::Length((Vector3&)_RenderInfo.Eye - PtLtSp.Center) <= PtLtSp.Radius)
			{

			}
			else
			{

			}*/

			if (false == CameraFrustum->IsIn(PtLtSp))continue;

			clipplanes.x = PointLight->GetNearPlane();
			clipplanes.y = PointLight->GetFarPlane();
			RECT scissorrect;
			PointLight->CalculateScissorRect(
				scissorrect, _RenderInfo.View, _RenderInfo.Projection,
				PointLight->GetPointRadius(), g_nWndCX, g_nWndCY);

			Device->SetScissorRect(&scissorrect);

			deferred->SetFloat("lightFlux", PointLight->lightFlux);
			deferred->SetFloat("lightIlluminance", PointLight->lightIlluminance);
			deferred->SetFloat("specularPower", PointLight->specularPower);
			deferred->SetFloat("sinAngularRadius", PointLight->sinAngularRadius);
			deferred->SetFloat("cosAngularRadius", PointLight->cosAngularRadius);
			deferred->SetFloat("shadowmin", PointLight->shadowmin);
			deferred->SetFloat("ShadowDepthBias", PointLight->shadowdepthbias);
			deferred->SetFloat("ShadowDepthMapHeight", PointLight->GetShadowMapSize());
			deferred->SetFloat("ShadowDepthMapWidth", PointLight->GetShadowMapSize());
			deferred->SetFloatArray("Lradiance", PointLight->Lradiance, 3);
			deferred->SetBool("IsPoint", true);
			deferred->SetVector("clipPlanes", &clipplanes);
			deferred->SetVector("lightColor", (D3DXVECTOR4*)&PointLight->GetColor());
			deferred->SetVector("lightPos", &PointLight->GetPosition());
			deferred->SetFloat("lightRadius", PointLight->GetPointRadius());
			device->SetTexture(4, PointLight->GetCubeShadowMap());
			deferred->CommitChanges();
			_Quad->Render(Device);
		}

		if (bPtLightScrRtTest)
		{
			device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		}
	}

	deferred->EndPass();
	deferred->End();

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	// 0 , 1 ( 알베도 , 노말 감마보정 꺼주기 )
	device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, FALSE);
	device->SetSamplerState(1, D3DSAMP_SRGBTEXTURE, FALSE);

	if (g_bRenderPtLightScissorTest)
	{
		auto screenquad = Shaders["ScreenQuad"]->GetEffect();
		{
			device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			screenquad->SetTechnique("rect");
			screenquad->Begin(NULL, 0);
			screenquad->BeginPass(0);
			for (auto& PtLt : PointLights)
			{
				// 시저렉트 외곽 렌더 ... 
				float rectvertices[24];
				uint16_t rectindices[5] = { 0, 1, 2, 3, 0 };
				memcpy(rectvertices, DXScreenQuadVerticesFFP, 24 * sizeof(float));

				rectvertices[0] += PtLt->LastScissorRect.left;
				rectvertices[1] += PtLt->LastScissorRect.top;
				rectvertices[6] += PtLt->LastScissorRect.left;
				rectvertices[7] += PtLt->LastScissorRect.bottom;
				rectvertices[12] += PtLt->LastScissorRect.right;
				rectvertices[13] += PtLt->LastScissorRect.bottom;
				rectvertices[18] += PtLt->LastScissorRect.right;
				rectvertices[19] += PtLt->LastScissorRect.top;
				screenquad->CommitChanges();
				device->DrawIndexedPrimitiveUP(
					D3DPT_LINESTRIP, 0, 4, 4, rectindices, D3DFMT_INDEX16, rectvertices, 6 * sizeof(float));
			}
			screenquad->EndPass();
			screenquad->End();

			device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);
		}
	}

}



HRESULT Renderer::RenderDebug()&
{
	if (g_bDebugRender == false)return S_OK;

	DrawInfo _DrawInfo;
	_DrawInfo._Device = Device;
	_DrawInfo.BySituation = {};
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto& [ShaderKey, _EntityArr] : RenderEntitys[RenderProperty::Order::Debug])
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		auto DebugColor = Vector4(0.7f, 0.0f, 0.3f, 0.5f);
		Fx->SetVector("DebugColor", &DebugColor);
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		_DrawInfo.Fx = Fx;
		UINT Passes{ 0u };
		Fx->Begin(&Passes, NULL);
		for (uint32 i = 0; i < Passes; ++i)
		{
			_DrawInfo.PassIndex = i;
			Fx->BeginPass(i);
			for (auto& [_Entity, Call] : _EntityArr)
			{
				Call(_DrawInfo);
			}
			Fx->EndPass();
		}
		Fx->End();
	}
	return S_OK;
};


HRESULT Renderer::RenderDebugBone()&
{
	if (g_bDebugBoneToRoot == false)return S_OK;

	auto& _Order = RenderEntitys[RenderProperty::Order::DebugBone];
	DrawInfo _DrawInfo{};
	_DrawInfo._Device = Device;
	_DrawInfo.BySituation.reset();
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto& [ShaderKey, _EntityArr] : _Order)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		_DrawInfo.Fx = Fx;
		Vector4 DebugColor{ 0.3f,0.7f,0.1f,0.5f };
		const Matrix ScaleOffset = FMath::Scale({ 0.01f,0.01f,0.01f });
		const Matrix ViewProjection = _RenderInfo.ViewProjection;
		Fx->SetVector("DebugColor", &DebugColor);
		Fx->SetMatrix("ScaleOffset", &ScaleOffset);
		Fx->SetMatrix("ViewProjection", &ViewProjection);
		UINT Passes = 0u;
		Fx->Begin(&Passes, NULL);
		for (int32 i = 0; i < Passes; ++i)
		{
			Fx->BeginPass(i);
			_DrawInfo.PassIndex = i;
			for (auto& [Entity, Call] : _EntityArr)
			{
				Call(_DrawInfo);
			}
			Fx->EndPass();
		}
		Fx->End();
	}

	return S_OK;
}


HRESULT Renderer::ImguiRender()&
{
	ImGui::EndFrame();
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
};


HRESULT Renderer::RenderTargetDebugRender()&
{
	if (g_bRenderTargetVisible)
	{
		auto RTDebugIter = Shaders.find("RTDebug");
		if (RTDebugIter != std::end(Shaders))
		{
			auto RTDebug = RTDebugIter->second;
			auto* const RTFx = RTDebug->GetEffect();
			for (auto& [RTName, RT] : RenderTargets)
			{
				RT->DebugRender(RTFx);
			};
		}
	}

	return S_OK;
}

HRESULT Renderer::RenderSky()&
{
	// 기존 코드 
	auto screenquad = Shaders["ScreenQuad"]->GetEffect();
	screenquad->SetTechnique("screenquad");
	screenquad->Begin(NULL, 0);
	screenquad->BeginPass(0);
	Device->SetTexture(0, sky->GetTexture());
	_Quad->Render(Device);
	screenquad->EndPass();
	screenquad->End();

	return S_OK;
};

HRESULT Renderer::RenderSkySphere()&
{
	if (CurSkysphereTex)
	{
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
		Device->SetRenderState(D3DRS_ZENABLE, FALSE);

		//Device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, FALSE);
		//Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		//Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
		//Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
		//Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);
		//Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		//Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		Device->SetRenderTarget(1u, RenderTargets["Distortion"]->GetSurface(0));

		Matrix SkyView = _RenderInfo.View;
		SkyView._41 = SkyView._42 = SkyView._43 = 0.0f;
		const Matrix ViewProj = SkyView * _RenderInfo.Projection;

		auto Fx = Shaders["skysphere"]->GetEffect();
		Fx->SetMatrix("matViewProj", &ViewProj);
		const float Dt = TimeSystem::GetInstance()->DeltaTime();
		if (!SkyDistortion)
		{
			SkysphereRot.y += Dt * SkyRotationSpeed;
		}
		
		const Matrix World = FMath::WorldMatrix(SkysphereScale, FMath::ToRadian( SkysphereRot ) , SkysphereLoc);
		Fx->SetMatrix("matSkyRotation", &World);
		Fx->SetFloat("Time", TimeSystem::GetInstance()->AccTime());
		Fx->SetFloat("intencity", SkyIntencity);
		Fx->SetBool("Distortion", SkyDistortion);
		if (SkyDistortion)
		{
			Fx->SetFloat("noisewrap", SkyNoisewrap);
			Fx->SetFloat("timecorr", SkyTimecorr);
			Fx->SetVector("DistortionColor", &DistortionColor);
			Fx->SetTexture("NoiseMap", SkyNoiseMap->GetTexture());
			Fx->SetFloat("DistortionIntencity", SkyDistortionIntencity);
		}
		Fx->SetTexture("SkyMap", CurSkysphereTex->GetTexture());
		Fx->Begin(NULL, 0);
		Fx->BeginPass(0);
		// Device->SetTexture(0u, CurSkysphereTex->GetTexture());
		const int32 Numsubset = SkysphereMesh->GetNumSubset();
		SkysphereMesh->GetSubset(0).lock()->Render(Fx);
		Fx->EndPass();
		Fx->End();
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		Device->SetRenderTarget(1u, nullptr);
	}

	return S_OK;
};


HRESULT Renderer::RenderEnvironment()&
{
	// Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, FALSE);

	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	Matrix Skyview = _RenderInfo.View;
	Skyview._41 = Skyview._42 = Skyview._43 = 0.0f;

	const Matrix viewproj = Skyview * _RenderInfo.Projection;

	auto Fx = Shaders["sky"]->GetEffect();
	Fx->SetMatrix("matViewProj", &viewproj);

	static float EnvironmentScale = 2.f;

	const Matrix rotation = FMath::Rotation(Vector3{ 0,0,0 });
	const Matrix world = FMath::Scale(EnvironmentScale);

	Fx->SetMatrix("matSkyRotation", &rotation);
	Fx->SetMatrix("matWorld", &world);

	Fx->Begin(NULL, 0);
	Fx->BeginPass(0);
	Device->SetTexture(0u, environment);
	Fx->CommitChanges();
	skymesh->DrawSubset(0);
	Fx->EndPass();
	Fx->End();
	return S_OK;
};

HRESULT Renderer::Tonemapping()&
{
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);

	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	auto tonemap = Shaders["ToneMap"]->GetEffect();

	auto scenetarget = RenderTargets["SceneTarget"]->GetTexture();
	D3DXVECTOR4			pixelsize(1, 1, 1, 1);
	pixelsize.x = 1.0f / (float)_RenderInfo.Viewport.Width;
	pixelsize.y = -1.0f / (float)_RenderInfo.Viewport.Height;
	tonemap->SetTechnique("tonemap");
	tonemap->SetVector("pixelSize", &pixelsize);
	tonemap->SetFloat("exposure", exposure);
	tonemap->Begin(NULL, 0);
	tonemap->BeginPass(0);
	Device->SetTexture(0, scenetarget);
	_Quad->Render(Device);
	tonemap->EndPass();
	tonemap->End();

	return S_OK;
};

HRESULT Renderer::AlphaBlendEffectRender()&
{
	DWORD ZEnable, ZWrite, AlphaEnable;

	Device->GetRenderState(D3DRS_ZENABLE, &ZEnable);
	Device->GetRenderState(D3DRS_ZWRITEENABLE, &ZWrite);
	Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &AlphaEnable);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	Device->SetRenderTarget(1u, RenderTargets["Distortion"]->GetSurface(0));
	auto& _Group = RenderEntitys[RenderProperty::Order::AlphaBlendEffect];
	
	using AsType = std::pair<const std::string*,
		ENGINE::Renderer::RenderEntityType*>;

	std::vector<AsType> AlphaSortArr;

	for (auto& [ShaderKey, Entitys] : _Group)
	{
		for (auto& _Entity : Entitys)
		{
			AlphaSortArr.push_back(AsType{ &ShaderKey,&_Entity } );
		}
	};

	std::sort(std::begin(AlphaSortArr), std::end(AlphaSortArr),
		[EyePos = _RenderInfo.Eye](const AsType& _Lhs,
								   const AsType& _Rhs)
		{
			const Vector3 LhsLocation
				=
				{		
					_Lhs.second->first->_RenderUpdateInfo.World._41,
					_Lhs.second->first->_RenderUpdateInfo.World._42,
					_Lhs.second->first->_RenderUpdateInfo.World._43
				};

			const Vector3 RhsLocation
				=
				{	_Rhs.second->first->_RenderUpdateInfo.World._41,
					_Rhs.second->first->_RenderUpdateInfo.World._42,
					_Rhs.second->first->_RenderUpdateInfo.World._43
				};

			return FMath::LengthSq((Vector3&)(EyePos)-LhsLocation) 
					>
					FMath::LengthSq((Vector3&)(EyePos)-RhsLocation);
		});

	DrawInfo _DrawInfo{};
	EffectInfo _EffInfo{};
	_EffInfo.SoftParticleDepthBiasScale = SoftParticleDepthScale;
	_DrawInfo.BySituation = _EffInfo;
	_DrawInfo._Device  = Device;
	_DrawInfo._Frustum = CameraFrustum.get();

	for (auto& [_RefStr,_RefEntity] : AlphaSortArr)
	{
		auto Fx = Shaders[*_RefStr]->GetEffect();
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		Fx->SetMatrix("InverseProjection", &_RenderInfo.ProjectionInverse);
		Fx->SetTexture("DepthMap", RenderTargets["Depth"]->GetTexture());
		Fx->SetFloat("SoftParticleDepthScale", SoftParticleDepthScale);
		Fx->SetFloat("exposure_corr",1.f/(exposure *0.002f));
		
		Vector3  LightDirection = { 0,-1,0 };

		if (CurDirLight)
		{
			LightDirection = CurDirLight->GetDirection();
		}

		LightDirection = FMath::Normalize(LightDirection);

		Fx->SetFloatArray("LightDirection", LightDirection, 3u);
		Fx->SetFloatArray("EyePos", _RenderInfo.Eye ,3u);

		_DrawInfo.Fx = Fx;

		UINT Passes{ 0u };
		Fx->Begin(&Passes, NULL); 
		{
			for (int32 i = 0; i < Passes; ++i)
			{
				_DrawInfo.PassIndex = i;
				Fx->BeginPass(i);
				_RefEntity->second(_DrawInfo);
				Fx->EndPass();
			}
		}
		Fx->End();
	}

	auto ParSys =ParticleSystem::GetInstance();
	if (ParSys)
	{
		ParSys->Render(this);
	}


	/*for (auto& [ShaderKey, Entitys] : _Group)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		Fx->SetMatrix("InverseProjection", &_RenderInfo.ProjectionInverse);
		Fx->SetTexture("DepthMap", RenderTargets["Depth"]->GetTexture());
		Fx->SetFloat("SoftParticleDepthScale",SoftParticleDepthScale);
		_DrawInfo.Fx = Fx;
		for (auto& [Entity, Call] : Entitys)
		{
			UINT Passes{ 0u };
			Fx->Begin(&Passes, NULL);
			for (int32 i = 0; i < Passes; ++i)
			{
				_DrawInfo.PassIndex = i;
				Fx->BeginPass(i);
				{
					Call(_DrawInfo);
				}
				Fx->EndPass();
			}
			Fx->End();
		}
	}*/

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, AlphaEnable);
	Device->SetRenderState(D3DRS_ZENABLE, ZEnable);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, ZWrite);

	Device->SetRenderTarget(1u, nullptr);
	return S_OK;
};

HRESULT Renderer::UIRender()&
{
	DWORD ZEnable, ZWrite, AlphaEnable;

	Device->GetRenderState(D3DRS_ZENABLE, &ZEnable);
	Device->GetRenderState(D3DRS_ZWRITEENABLE, &ZWrite);
	Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &AlphaEnable);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	auto& _Group = RenderEntitys[RenderProperty::Order::UI];
	DrawInfo _DrawInfo{};
	_DrawInfo.BySituation.reset();
	_DrawInfo._Device = Device;
	_DrawInfo.IsAfterPostProcessing = false;
	for (auto& [ShaderKey, Entitys] : _Group)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		Fx->SetMatrix("Ortho", &_RenderInfo.Ortho);
		Fx->SetFloat("exposure_corr",1.f/(exposure *0.002f));
		_DrawInfo.Fx = Fx;
		for (auto& [Entity, Call] : Entitys)
		{
			UINT Passes{ 0u };
			Fx->Begin(&Passes, NULL);
			Call(_DrawInfo);
			Fx->End();
		}
	}
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, AlphaEnable);
	Device->SetRenderState(D3DRS_ZENABLE, ZEnable);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, ZWrite);
	return S_OK;
}

HRESULT Renderer::UIRenderAfterPostProcessing()&
{
	DWORD ZEnable, ZWrite, AlphaEnable;

	Device->GetRenderState(D3DRS_ZENABLE, &ZEnable);
	Device->GetRenderState(D3DRS_ZWRITEENABLE, &ZWrite);
	Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &AlphaEnable);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	auto& _Group = RenderEntitys[RenderProperty::Order::UI];

	// 후처리 이후에 그리는 UI만 Transform.z로 정렬해서 그림
	using ZsType = std::pair<const std::string*,
							ENGINE::Renderer::RenderEntityType*>;

	std::list<ZsType> ZSortlist;

	for (auto& [ShaderKey, Entitys] : _Group)
	{
		for (auto& _Entity : Entitys)
		{
			ZSortlist.push_back(ZsType{ &ShaderKey, &_Entity });
		}
	};

	ZSortlist.sort([](const ZsType& _Lhs, const ZsType& _Rhs)
		{
			return _Lhs.second->first->_RenderUpdateInfo.World._43
			< _Rhs.second->first->_RenderUpdateInfo.World._43;
		});

	DrawInfo _DrawInfo{};
	_DrawInfo.BySituation.reset();
	_DrawInfo._Device = Device;
	_DrawInfo.IsAfterPostProcessing = true;

	for (auto& [RefShaderKey, RefEntity] : ZSortlist)
	{
		auto Fx = Shaders[*RefShaderKey]->GetEffect();
		Fx->SetMatrix("Ortho", &_RenderInfo.Ortho);
		Fx->SetFloat("exposure_corr", 1.f / (exposure * 0.002f));
		_DrawInfo.Fx = Fx;

		UINT Passes{ 0u };
		Fx->Begin(&Passes, NULL);
		RefEntity->second(_DrawInfo);
		Fx->End();
	}

	ZSortlist.clear();

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, AlphaEnable);
	Device->SetRenderState(D3DRS_ZENABLE, ZEnable);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, ZWrite);
	return S_OK;
}

HRESULT Renderer::RendererCollider()&
{
	if (g_bRenderCollider == false)return S_OK;

	auto& _Order = RenderEntitys[RenderProperty::Order::Collider];
	DrawInfo _DrawInfo{};
	_DrawInfo._Device = Device;
	_DrawInfo.BySituation.reset();
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto& [ShaderKey, _EntityArr] : _Order)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		_DrawInfo.Fx = Fx;

		const Matrix ScaleOffset = FMath::Scale({ 0.01f,0.01f,0.01f });
		Fx->SetVector("DebugColor", &ColliderRenderDefaultColor);
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		UINT Passes = 0u;
		for (auto& [Entity, Call] : _EntityArr)
		{
			Fx->Begin(&Passes, NULL);
			Fx->BeginPass(0);
			Call(_DrawInfo);
			Fx->EndPass();
			Fx->End();
		}
	}

	return S_OK;
};

HRESULT Renderer::LightFrustumRender()&
{
	if (bLightRender)
	{
		auto Fx = Shaders["Frustum"]->GetEffect();
		Fx->Begin(nullptr, NULL);
		Fx->BeginPass(0);
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		static const Vector4 DebugColor{ 1.f,1.f,1.f,0.025f };
		Fx->SetVector("DebugColor", &DebugColor);
		for (auto& _Light : DirLights)
		{
			CurShadowFrustum->Render(Device, Fx, _Light->GetWorld());
		}
		Fx->EndPass();
		Fx->End();
	}

	if (bLightRender)
	{
		auto Fx = Shaders["Frustum"]->GetEffect();
		Fx->Begin(nullptr, NULL);
		Fx->BeginPass(0);
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		static const Vector4 DebugColor{ 1.f,1.f,1.f,0.025f };
		Fx->SetVector("DebugColor", &DebugColor);
		for (auto& _Light : PointLights)
		{
			for (_Light->Currentface = 0; _Light->Currentface < 6; ++_Light->Currentface)
			{
				Matrix viewproj{};
				_Light->CalculateViewProjection(viewproj);
				CurShadowFrustum->Render(Device, Fx, _Light->GetWorld());
			};
		}
		Fx->EndPass();
		Fx->End();
	}

	return S_OK;
};

HRESULT Renderer::RenderInsulatorMetal()&
{
	// 감마보정 X
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetTexture(0, RenderTargets["ALBM"]->GetTexture());
	Device->SetTexture(1, RenderTargets["SceneTarget"]->GetTexture());
	Device->SetTexture(2, RenderTargets["NRMR"]->GetTexture());
	Device->SetTexture(3, RenderTargets["Depth"]->GetTexture());
	Device->SetTexture(4, irradiance1);
	Device->SetTexture(5, irradiance2);
	Device->SetTexture(6, brdfLUT);

	for (int i = 0; i < 7; ++i)
	{
		Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}

	Vector2 pixelSize{};
	pixelSize.x = 1.f / (float)g_nWndCX;
	pixelSize.y = -1.f / (float)g_nWndCY;

	auto Fx = Shaders["InsulatorMetal"]->GetEffect();
	Fx->Begin(nullptr, 0);
	Fx->BeginPass(0);
	Fx->SetMatrix("matViewProjInv", &_RenderInfo.ViewProjectionInverse);
	Fx->SetVector("eyePos", &_RenderInfo.Eye);
	Fx->SetFloatArray("pixelSize", pixelSize, 2);
	_Quad->Render(Device, 1.f, 1.f, Fx);
	Fx->CommitChanges();
	Fx->EndPass();
	Fx->End();

	// 컬링 다시 켜기 . 
	// Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}
HRESULT Renderer::RenderMeasureLuminance()
{
	Device->SetSamplerState(0,
		D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0,
		D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0,
		D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0,
		D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0,
		D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	Device->SetRenderState(D3DRS_ZENABLE, FALSE);

	D3DVIEWPORT9 viewport;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.f;
	viewport.X = 0.0f;
	viewport.Y = 0.0f;
	Vector4 pixelsize{ 0, 0, 0, 1 };
	Vector4 texelsize{ 0,0,0,1 };

	for (int i = 0; i < 4; ++i)
	{
		viewport.Width = 64 >> (i * 2);
		viewport.Height = 64 >> (i * 2);

		pixelsize.x = 1.0f / (float)viewport.Width;
		pixelsize.y = -1.0f / (float)viewport.Height;

		auto measureeffect =
			Shaders["measureluminance"]->GetEffect();

		if (i == 0)
		{
			texelsize.x = 1.0f /
				(float)_RenderInfo.Viewport.Width;
			texelsize.y = 1.0f /
				(float)_RenderInfo.Viewport.Height;

			measureeffect->SetTechnique("avgluminital");

			Device->SetTexture(0,
				RenderTargets["SceneTarget"]->GetTexture());
		}
		else if (i == 3)
		{
			texelsize.x =
				1.0f / (float)(64 >> (2 * (i - 1)));
			texelsize.y =
				1.0f / (float)(64 >> (2 * (i - 1)));

			measureeffect->SetTechnique("avglumfinal");
			Device->SetTexture(0,
				RenderTargets["avgluminance"]->GetTexture());
		}
		else
		{
			texelsize.x = 1.0f / (float)(64 >> (2 * (i - 1)));
			texelsize.y = 1.0f / (float)(64 >> (2 * (i - 1)));

			measureeffect->SetTechnique("avglumiterative");
			Device->SetTexture(0,
				RenderTargets["avgluminance"]->GetTexture());
		}

		measureeffect->SetInt("prevLevel", (i - 1) * 2);
		measureeffect->SetVector("pixelSize", &pixelsize);
		measureeffect->SetVector("texelSize", &texelsize);

		Device->SetRenderTarget(0,
			RenderTargets["avgluminance"]->GetSurface(i));
		Device->SetViewport(&viewport);

		measureeffect->Begin(NULL, 0);
		measureeffect->BeginPass(0);
		{
			_Quad->Render(Device, 1.f, 1.f, measureeffect);
		}
		measureeffect->EndPass();
		measureeffect->End();
	}

	D3DLOCKED_RECT rect;

	Device->GetRenderTargetData(
		RenderTargets["avgluminance"]->GetSurface(3),
		RenderTargets["avglumsystemmem"]->GetSurface(0)
	);

	auto& avglumsystemmem = RenderTargets["avglumsystemmem"];
	auto* avglumsystemmem_tex = avglumsystemmem->GetTexture();
	avglumsystemmem_tex->LockRect
	(0, &rect, NULL, D3DLOCK_READONLY);
	{
		averageluminance =
			((Math::Float16*)rect.pBits)->operator float();
	}
	avglumsystemmem_tex->UnlockRect(0);

	Device->SetViewport(&_RenderInfo.Viewport);
	return S_OK;
}
HRESULT Renderer::AdaptLuminance(const float DeltaTime)&
{
	// 델타타임에 영향을 매우 많이 받음 . 
	adaptedluminance = adaptedluminance +
		(averageluminance - adaptedluminance) *
		(1.0f - powf(adaptedluminance_var[0], adaptedluminance_var[1] * DeltaTime));

	float two_ad_EV = adaptedluminance *
		(adaptedluminance_var[2] / adaptedluminance_var[3]);

	exposure = 1.0f / (adaptedluminance_var[4] * two_ad_EV) *
		adaptedluminance_var[5];

	return S_OK;
}
HRESULT Renderer::BrightPass()&
{
	D3DVIEWPORT9 viewport;
	viewport.X = 0.0f;
	viewport.Y = 0.0f;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.f;

	Vector4 pixelsize{ 0,0,0,1 };
	viewport.Width = _RenderInfo.Viewport.Width / 2;
	viewport.Height = _RenderInfo.Viewport.Height / 2;

	pixelsize.x = 1.0f / (float)viewport.Width;
	pixelsize.y = -1.0f / (float)viewport.Height;

	// 여기서 샘플링 .

	Device->SetRenderTarget(0,
		RenderTargets["dsampletargets0"]->GetSurface());
	Device->SetViewport(&viewport);

	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	auto hdreffects = Shaders["hdreffects"]->GetEffect();
	hdreffects->SetTechnique("brightpass");
	hdreffects->SetVector("pixelSize", &pixelsize);
	hdreffects->SetFloat("exposure", exposure);

	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);

	{
		Device->SetTexture(0,
			RenderTargets["SceneTarget"]->GetTexture());
		_Quad->Render(Device, 1.f, 1.f, hdreffects);
	}

	hdreffects->EndPass();
	hdreffects->End();
	std::string idxstr = std::to_string(currentafterimage);
	Device->SetRenderTarget(0,
		RenderTargets["afterimagetargets" + idxstr]->GetSurface(0));

	if (drawafterimage)
	{
		hdreffects->SetTechnique("afterimage");
		hdreffects->SetVector("pixelSize", &pixelsize);

		hdreffects->Begin(NULL, 0);
		hdreffects->BeginPass(0);
		{
			std::string idxstr = std::to_string(1 - currentafterimage);
			Device->SetTexture(0,
				RenderTargets["afterimagetargets" + idxstr]->GetTexture());
			Device->SetTexture(1,
				RenderTargets["dsampletargets0"]->GetTexture());
			_Quad->Render(Device, 1.f, 1.f, hdreffects);
		}
		hdreffects->EndPass();
		hdreffects->End();
	}
	else
	{
		Device->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
	};

	currentafterimage = 1 - currentafterimage;
	return S_OK;
}
HRESULT Renderer::DownSample()
{
	D3DVIEWPORT9 viewport;
	viewport.X = 0.0f;
	viewport.Y = 0.0f;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	viewport.Width = _RenderInfo.Viewport.Width / 2;
	viewport.Height = _RenderInfo.Viewport.Height / 2;

	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	auto hdreffects = Shaders["hdreffects"]->GetEffect();
	hdreffects->SetTechnique("downsample");

	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);

	Vector4 texelsize{ 0.f,0.f,0.f ,0.f };
	Vector4 pixelsize{ 0.f,0.f,0.f,1.f };
	{
		for (int i = 1; i < 5; ++i)
		{
			texelsize.x = 1.0f / (float)viewport.Width;
			texelsize.y = 1.0f / (float)viewport.Height;

			viewport.Width /= 2;
			viewport.Height /= 2;

			pixelsize.x = 1.0f / (float)viewport.Width;
			pixelsize.y = -1.0f / (float)viewport.Height;

			hdreffects->SetVector("pixelSize", &pixelsize);
			hdreffects->SetVector("texelSize", &texelsize);
			hdreffects->CommitChanges();

			std::string idxstr = std::to_string(i);
			Device->SetRenderTarget(0,
				RenderTargets["dsampletargets" + idxstr]->GetSurface(0));
			idxstr = std::to_string(i - 1);

			Device->SetTexture(0,
				RenderTargets["dsampletargets" + idxstr]->GetTexture());
			Device->SetViewport(&viewport);

			_Quad->Render(Device, 1.f, 1.f, hdreffects);

		}
	}

	hdreffects->EndPass();
	hdreffects->End();



	return S_OK;
}
HRESULT Renderer::Stars()
{
	D3DVIEWPORT9 viewport;
	viewport.Width = _RenderInfo.Viewport.Width / 4;
	viewport.Height = _RenderInfo.Viewport.Height / 4;
	viewport.X = viewport.Y = 0.0f;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.f;

	Vector4 pixelsize{ 0.f,0.f,0.f,1.f };
	pixelsize.x = 1.0f / (float)viewport.Width;
	pixelsize.y = -1.0f / (float)viewport.Height;

	Vector4 texelsize{ 0.f ,0.f ,0.f ,0.f };
	texelsize.x = 1.0f / (float)viewport.Width;
	texelsize.y = 1.0f / (float)viewport.Height;

	Device->SetViewport(&viewport);

	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

	auto hdreffects = Shaders["hdreffects"]->GetEffect();
	hdreffects->SetTechnique("star");
	hdreffects->SetVector("pixelSize", &pixelsize);
	hdreffects->SetVector("texelSize", &texelsize);
	hdreffects->SetFloat("StarScale", StarScale);
	hdreffects->SetFloat("StarFactor", StarFactor);

	
	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);
	{
		for (int i = 0; i < 4; ++i)
		{
			hdreffects->SetInt("starDirection", i);

			for (int j = 0; j < 3; ++j)
			{
				hdreffects->SetInt("starPass", j);
				hdreffects->CommitChanges();

				std::string idxstr = std::to_string(i) +
					std::to_string(j % 2);

				Device->SetRenderTarget(0,
					RenderTargets["startargets" + idxstr]->GetSurface(0));

				auto* _Tex = j == 0 ?
					RenderTargets["dsampletargets1"]->GetTexture() :
					RenderTargets[
						"startargets" + std::to_string(i)
							+ std::to_string(1 - j % 2)]->GetTexture();

						Device->SetTexture(0, _Tex);

						_Quad->Render(Device, 1.f, 1.f, hdreffects);
			}

		}
	}
	hdreffects->EndPass();
	hdreffects->End();

	for (int i = 1; i < 4; ++i)
	{
		Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}

	hdreffects->SetTechnique("starcombine");

	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);
	{
		Device->SetRenderTarget(0,
			RenderTargets["starresult"]->GetSurface(0));

		Device->SetTexture(0,
			RenderTargets["startargets" +
			std::to_string(0) +
			std::to_string(0)]
			->GetTexture());

		Device->SetTexture(1,
			RenderTargets["startargets" +
			std::to_string(1) +
			std::to_string(0)]
			->GetTexture());

		Device->SetTexture(2,
			RenderTargets["startargets" +
			std::to_string(2) +
			std::to_string(0)]
			->GetTexture());

		Device->SetTexture(3,
			RenderTargets["startargets" +
			std::to_string(3) +
			std::to_string(0)]
			->GetTexture());

		_Quad->Render(Device, 1.f, 1.f, hdreffects);
	}
	hdreffects->EndPass();
	hdreffects->End();


	return S_OK;
}
HRESULT Renderer::Bloom()
{
	D3DVIEWPORT9 viewport{};
	viewport.X = viewport.Y = 0.0f;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.f;

	viewport.Width = _RenderInfo.Viewport.Width / 2;
	viewport.Height = _RenderInfo.Viewport.Height / 2;

	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

	auto* hdreffects = Shaders["hdreffects"]->GetEffect();

	Vector4 pixelsize{ 0.f,0.f,0.f,1.f };
	Vector4 texelsize{ 0.f,0.f,0.f ,0.f };
	hdreffects->SetTechnique("blur");
	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);
	{
		for (int i = 0; i < 5; ++i)
		{
			Device->SetViewport(&viewport);

			pixelsize.x = 1.0f / (float)viewport.Width;
			pixelsize.y = -1.0f / (float)viewport.Height;

			texelsize.x = 1.0f / (float)viewport.Width;
			texelsize.y = 1.0f / (float)viewport.Height;

			hdreffects->SetVector("pixelSize", &pixelsize);
			hdreffects->SetVector("texelSize", &texelsize);

			// 가로 방향
			hdreffects->SetInt("blurDirection", 0);
			hdreffects->CommitChanges();

			Device->SetRenderTarget(0,
				RenderTargets[
					"blurtargets" + std::to_string(i)]->GetSurface(0));
			Device->SetTexture(0,
				RenderTargets[
					"dsampletargets" + std::to_string(i)]->GetTexture());
			_Quad->Render(Device, 1.f, 1.f, hdreffects);

			// 세로 방향
			hdreffects->SetInt("blurDirection", 1);
			hdreffects->CommitChanges();

			Device->SetRenderTarget(0,
				RenderTargets["dsampletargets" + std::to_string(i)]
				->GetSurface(0));

			Device->SetTexture(0,
				RenderTargets["blurtargets" + std::to_string(i)]
				->GetTexture());

			_Quad->Render(Device, 1.f, 1.f, hdreffects);

			viewport.Width /= 2;
			viewport.Height /= 2;
		}
	}
	hdreffects->EndPass();
	hdreffects->End();

	viewport.Width = _RenderInfo.Viewport.Width / 2;
	viewport.Height = _RenderInfo.Viewport.Height / 2;

	pixelsize.x = 1.0f / (float)viewport.Width;
	pixelsize.y = -1.0f / (float)viewport.Height;


	for (int i = 1; i < 5; ++i)
	{
		Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}

	hdreffects->SetTechnique("blurcombine");
	hdreffects->SetVector("pixelSize", &pixelsize);

	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);
	{
		Device->SetRenderTarget(0,
			RenderTargets["bloomresult"]->GetSurface(0));
		Device->SetViewport(&viewport);

		Device->SetTexture(0,
			RenderTargets["dsampletargets0"]->GetTexture());
		Device->SetTexture(1,
			RenderTargets["dsampletargets1"]->GetTexture());
		Device->SetTexture(2,
			RenderTargets["dsampletargets2"]->GetTexture());
		Device->SetTexture(3,
			RenderTargets["dsampletargets3"]->GetTexture());
		Device->SetTexture(4,
			RenderTargets["dsampletargets4"]->GetTexture());

		_Quad->Render(Device, 1.f, 1.f, hdreffects);
	}
	hdreffects->EndPass();
	hdreffects->End();

	return S_OK;
}
HRESULT Renderer::LensFlare()
{
	D3DVIEWPORT9 viewport{};
	viewport.X = viewport.Y = 0.0f;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.f;

	Vector4 pixelsize{ 0.f,0.f,0.f,1.f };
	Vector4 texelsize{ 0.f,0.f,0.f ,0.f };

	viewport.Width = _RenderInfo.Viewport.Width / 2;
	viewport.Height = _RenderInfo.Viewport.Height / 2;

	pixelsize.x = 1.0f / (float)viewport.Width;
	pixelsize.y = -1.0f / (float)viewport.Height;

	for (int i = 0; i < 3; ++i)
	{
		Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		Device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	}

	auto* hdreffects = Shaders["hdreffects"]->GetEffect();
	hdreffects->SetTechnique("lensflare");
	hdreffects->SetVector("pixelSize", &pixelsize);

	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);
	{
		Device->SetRenderTarget(0,
			RenderTargets["lensflaretargets0"]->GetSurface(0));
		Device->SetViewport(&viewport);

		Device->SetTexture(0,
			RenderTargets["dsampletargets0"]->GetTexture());
		Device->SetTexture(1,
			RenderTargets["dsampletargets1"]->GetTexture());
		Device->SetTexture(2,
			RenderTargets["dsampletargets2"]->GetTexture());

		_Quad->Render(Device, 1.f, 1.f, hdreffects);
	}
	hdreffects->EndPass();
	hdreffects->End();

	hdreffects->SetTechnique("lensflare_expand");

	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);
	{
		Device->SetRenderTarget(0,
			RenderTargets["lensflaretargets1"]->GetSurface(0));
		Device->SetViewport(&viewport);

		Device->SetTexture(0,
			RenderTargets["lensflaretargets0"]->GetTexture());
		_Quad->Render(Device, 1.f, 1.f, hdreffects);
	}
	hdreffects->EndPass();
	hdreffects->End();


	return S_OK;
};

HRESULT Renderer::ToneMap()
{
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	Vector4 pixelsize{ 0.f,0.f,0.f,1.f };

	pixelsize.x = 1.0f / (float)_RenderInfo.Viewport.Width;
	pixelsize.y = -1.0f / (float)_RenderInfo.Viewport.Height;

	Device->SetViewport(&_RenderInfo.Viewport);
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);

	auto* hdreffects = Shaders["hdreffects"]->GetEffect();
	hdreffects->SetTechnique("tonemap");
	hdreffects->SetVector("pixelSize", &pixelsize);
	hdreffects->SetFloat("exposure", exposure);
	hdreffects->SetMatrix("matViewProjInv", &_RenderInfo.ViewProjectionInverse);
	hdreffects->SetFloatArray("eyepos", _RenderInfo.Eye, 3u);
	hdreffects->SetFloatArray("fogcolor", FogColor, 3u);
	hdreffects->SetFloat("fogdistance", FogDistance);
	hdreffects->SetFloat("fogdensity", FogDensity);
	hdreffects->SetFloat("fogstart", FogStart);

	;
	;

	hdreffects->Begin(NULL, 0);
	hdreffects->BeginPass(0);
	{
		Device->SetTexture(0, RenderTargets["SceneTarget"]->GetTexture());
		Device->SetTexture(1, RenderTargets["bloomresult"]->GetTexture());
		Device->SetTexture(2, RenderTargets["starresult"]->GetTexture());
		Device->SetTexture(3, RenderTargets["lensflaretargets1"]->GetTexture());
		Device->SetTexture(4, RenderTargets["afterimagetargets" +
			std::to_string(1 - currentafterimage)]->GetTexture());
		Device->SetTexture(5, RenderTargets["Depth"]->GetTexture());

		Device->SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_NONE);


		_Quad->Render(Device, 1.f, 1.f, hdreffects);
	}
	hdreffects->EndPass();
	hdreffects->End();

	return S_OK;
}
HRESULT Renderer::RenderEmissive()
{
	DWORD ZEnable, ZWrite, AlphaEnable;

	Device->GetRenderState(D3DRS_ZENABLE, &ZEnable);
	Device->GetRenderState(D3DRS_ZWRITEENABLE, &ZWrite);
	Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &AlphaEnable);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	auto& _Group = RenderEntitys[RenderProperty::Order::Emissive];
	DrawInfo _DrawInfo{};
	_DrawInfo.BySituation.reset();
	_DrawInfo._Device = Device;
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto& [ShaderKey, Entitys] : _Group)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		Device->SetRenderTarget(1u, RenderTargets["Depth"]->GetSurface(0));

		_DrawInfo.Fx = Fx;
		for (auto& [Entity, Call] : Entitys)
		{
			UINT Passes{ 0u };
			Fx->Begin(&Passes, NULL);
			for (int32 i = 0; i < Passes; ++i)
			{
				_DrawInfo.PassIndex = i;
				Fx->BeginPass(i);
				{
					Call(_DrawInfo);
				}
				Fx->EndPass();
			}
			Fx->End();
		}
	}

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, AlphaEnable);
	Device->SetRenderState(D3DRS_ZENABLE, ZEnable);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, ZWrite);

	return S_OK;
};

HRESULT Renderer::RenderUV()
{
	return S_OK;
};

HRESULT Renderer::BlendDistortion()
{
	DWORD zwrite, zenable, alphablendenable, srcblend, destblend;
	Device->GetRenderState(D3DRS_ZWRITEENABLE,&zwrite);
	Device->GetRenderState(D3DRS_ZENABLE, &zenable);
	Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphablendenable);
	Device->GetRenderState(D3DRS_SRCBLEND,&srcblend);
	Device->GetRenderState(D3DRS_DESTBLEND, &destblend);

	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	Vector4 PixelSize{ 0.f,0.f,0.f,1.f };
	
	auto& DistortionRT = RenderTargets["SceneTarget"];

	PixelSize.x = 1.0f / (float)DistortionRT->GetInfo().Width;
	PixelSize.y = -1.0f / (float)DistortionRT->GetInfo().Height;

	Device->SetRenderTarget(0u, DistortionRT->GetSurface(0));
	auto Fx = Shaders["Distortion"]->GetEffect();
	Fx->SetVector("PixelSize", &PixelSize);

	Fx->SetFloat("Time", TimeSystem::GetInstance()->AccTime());
	Fx->SetFloat("Intencity", DistortionIntencity);

	Fx->Begin(nullptr,0);
	Fx->BeginPass(0);
	Fx->SetTexture("SceneMap",RenderTargets["SceneTarget"]->GetTexture());
	// Fx->SetTexture("DistortionMap", DistortionTex->GetTexture());
	Fx->SetTexture("DistortionMap", RenderTargets["Distortion"]->GetTexture());
	_Quad->Render(Device, 1.f, 1.f, Fx);
	Fx->EndPass();
	Fx->End();

	Device->SetRenderState(D3DRS_ZWRITEENABLE, zwrite);
	Device->SetRenderState(D3DRS_ZENABLE, zenable);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, alphablendenable);
	Device->SetRenderState(D3DRS_SRCBLEND, srcblend);
	Device->SetRenderState(D3DRS_DESTBLEND, destblend);

	return S_OK;
};

inline DWORD F2DW(FLOAT f)
{
	return *((DWORD*)&f);
}
void Renderer::EnableDepthBias()&
{
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,
		F2DW(SlpoeScaleDepthBias));
	Device->SetRenderState(D3DRS_DEPTHBIAS, F2DW(DepthBias));
}
void Renderer::DisableDepthBias()&
{
	Device->
		SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
	Device->SetRenderState(D3DRS_DEPTHBIAS, F2DW(0.0f));
}
;

void Renderer::LightSave(std::filesystem::path path)
{
	std::vector<FLight*> _Lights{};
	for (auto& _Light : DirLights)
	{
		_Lights.push_back(_Light.get());
	}
	for (auto& _Light : PointLights)
	{
		_Lights.push_back(_Light.get());
	}
	// 1번 래피드 제이슨 네임스페이스 열기 
	using namespace rapidjson;

	// 스트링 버퍼
	StringBuffer StrBuf{};

	PrettyWriter<StringBuffer> Writer(StrBuf);
	Writer.StartObject();
	Writer.Key("LightDataArray");
	Writer.StartArray();

	for (auto& _Light : _Lights)
	{
		// Writer.Key("LightData");
		Writer.StartObject();
		{
			Writer.Key("sinAngularRadius");
			Writer.Double(_Light->sinAngularRadius);

			Writer.Key("cosAngularRadius");
			Writer.Double(_Light->cosAngularRadius);

			Writer.Key("specularPower");
			Writer.Double(_Light->specularPower);

			Writer.Key("lightIlluminance");
			Writer.Double(_Light->lightIlluminance);

			Writer.Key("lightFlux");
			Writer.Double(_Light->lightFlux);

			Writer.Key("Color");
			Writer.StartArray();
			auto _Color = _Light->GetColor();
			Writer.Double(_Color.r);
			Writer.Double(_Color.g);
			Writer.Double(_Color.b);
			Writer.Double(_Color.a);
			Writer.EndArray();

			Writer.Key("Near");
			Writer.Double(_Light->GetNearPlane());

			Writer.Key("Far");
			Writer.Double(_Light->GetFarPlane());

			Writer.Key("Projparams");
			Writer.StartArray();
			Writer.Double(_Light->Projparams.x);
			Writer.Double(_Light->Projparams.y);
			Writer.Double(_Light->Projparams.z);
			Writer.Double(_Light->Projparams.w);
			Writer.EndArray();

			Writer.Key("PointRadius");
			Writer.Double(_Light->GetPointRadius());

			Writer.Key("Direction");
			Writer.StartArray();
			Writer.Double(_Light->Direction.x);
			Writer.Double(_Light->Direction.y);
			Writer.Double(_Light->Direction.z);
			Writer.EndArray();

			Writer.Key("Lradiance");
			Writer.StartArray();
			Writer.Double(_Light->Lradiance.x);
			Writer.Double(_Light->Lradiance.y);
			Writer.Double(_Light->Lradiance.z);
			Writer.EndArray();


			Writer.Key("Position");
			Writer.StartArray();
			Writer.Double(_Light->GetPosition().x);
			Writer.Double(_Light->GetPosition().y);
			Writer.Double(_Light->GetPosition().z);
			Writer.Double(_Light->GetPosition().w);
			Writer.EndArray();

			Writer.Key("shadowmin");
			Writer.Double(_Light->shadowmin);

			Writer.Key("shadowdepthbias");
			Writer.Double(_Light->shadowdepthbias);

			Writer.Key("ShadowMapSize");
			Writer.Double(_Light->GetShadowMapSize());

			Writer.Key("Type");
			std::string TypeStr = _Light->_Type == FLight::Type::Directional ? "Directional" : "Point";
			Writer.String(TypeStr.c_str());
		}
		Writer.EndObject();
	}

	Writer.EndArray();
	Writer.EndObject();

	// path.replace_extension("Light");
	std::ofstream Of{ path };
	Of << StrBuf.GetString();
};

void Renderer::LightLoad(const std::filesystem::path& path)
{
	using namespace rapidjson;

	std::ifstream Is{ path };

	if (!Is.is_open())
	{
		return;
	}

	IStreamWrapper Isw(Is);
	Document _Document{};
	_Document.ParseStream(Isw);

	if (_Document.HasParseError())
	{
		PRINT_LOG(L"Warning!", L"Animation Parse Error!");
		return;
	}

	const Value& LightData = _Document["LightDataArray"];
	DirLights.clear();
	PointLights.clear();
	// if (LightData.IsArray())
	{
		// auto LightArray=LightData.GetArray();
		for (auto iter = LightData.Begin();
			iter != LightData.End(); ++iter)
		{
			auto _Light = std::make_shared<FLight>();

			/*auto LightObject = iter->FindMember("LightData")->value.Get<Value::Object>();*/

			_Light->sinAngularRadius = iter->FindMember("sinAngularRadius")->value.GetDouble();
			_Light->cosAngularRadius = iter->FindMember("cosAngularRadius")->value.GetDouble();
			_Light->specularPower = iter->FindMember("specularPower")->value.GetDouble();
			_Light->lightIlluminance = iter->FindMember("lightIlluminance")->value.GetDouble();
			_Light->lightFlux = iter->FindMember("lightFlux")->value.GetDouble();

			auto ColorArr = iter->FindMember("Color")->value.GetArray();
			_Light->GetColor().r = ColorArr[0].GetDouble();
			_Light->GetColor().g = ColorArr[1].GetDouble();
			_Light->GetColor().b = ColorArr[2].GetDouble();
			_Light->GetColor().a = ColorArr[3].GetDouble();

			_Light->Projparams.z = iter->FindMember("Near")->value.GetDouble();
			_Light->Projparams.w = iter->FindMember("Far")->value.GetDouble();

			auto Projparams = iter->FindMember("Projparams")->value.GetArray();
			_Light->Projparams.x = Projparams[0].GetDouble();
			_Light->Projparams.y = Projparams[1].GetDouble();
			_Light->Projparams.z = Projparams[2].GetDouble();
			_Light->Projparams.w = Projparams[3].GetDouble();

			_Light->PointRadius = iter->FindMember("PointRadius")->value.GetDouble();

			auto Direction = iter->FindMember("Direction")->value.GetArray();
			_Light->Direction.x = Direction[0].GetDouble();
			_Light->Direction.y = Direction[1].GetDouble();
			_Light->Direction.z = Direction[2].GetDouble();


			auto Lradiance = iter->FindMember("Lradiance")->value.GetArray();
			_Light->Lradiance.x = Lradiance[0].GetDouble();
			_Light->Lradiance.y = Lradiance[1].GetDouble();
			_Light->Lradiance.z = Lradiance[2].GetDouble();


			auto Position = iter->FindMember("Position")->value.GetArray();
			_Light->Position.x = Position[0].GetDouble();
			_Light->Position.y = Position[1].GetDouble();
			_Light->Position.z = Position[2].GetDouble();
			_Light->Position.w = Position[3].GetDouble();

			_Light->shadowmin = iter->FindMember("shadowmin")->value.GetDouble();
			_Light->shadowdepthbias = iter->FindMember("shadowdepthbias")->value.GetDouble();

			_Light->ShadowMapSize = iter->FindMember("ShadowMapSize")->value.GetDouble();

			auto TypeStr = iter->FindMember("Type")->value.GetString();
			if (std::string(TypeStr) == "Directional")
			{
				_Light->_Type = FLight::Type::Directional;
				DirLights.push_back(_Light);
			}
			else
			{
				_Light->_Type = FLight::Type::Point;
				PointLights.push_back(_Light);
			}

			_Light->CreateShadowMap(Device, _Light->ShadowMapSize);
			_Light->InitRender();

		}
	};


	if (DirLights.empty() == false)
	{
		CurDirLight = DirLights[0].get();
	}
};


bool Renderer::TestShaderInit()
{
	if (FAILED(D3DXLoadMeshFromX(
		L"../../Media/MeshesDX/skullocc3.x", D3DXMESH_MANAGED, Device, NULL, NULL, NULL, NULL, &skull)))
		return false;

	if (FAILED(D3DXLoadMeshFromX(L"../../Media/MeshesDX/sky.x", D3DXMESH_MANAGED, Device, NULL, NULL, NULL, NULL, &skymesh)))
		return false;

	if (FAILED(D3DXCreateCubeTextureFromFile(Device, L"../../Media/Textures/grace.dds",
		&environment)))
		return false;

	if (FAILED(D3DXCreateCubeTextureFromFile(Device, L"../../Media/Textures/grace_diff_irrad.dds",
		&irradiance1)))
		return false;

	if (FAILED(D3DXCreateCubeTextureFromFile(Device, L"../../Media/Textures/grace_spec_irrad.dds",
		&irradiance2)))
		return false;

	if (FAILED(D3DXCreateTextureFromFile(Device, L"../../Media/Textures/brdf.dds", &brdfLUT)))
		return false;

	sky = Resources::Load<Texture >("../../Media/Textures/static_sky.jpg");

	return true;
}

void Renderer::TestShaderRelease()
{
	if (skymesh)
		skymesh->Release();

	if (skull)
		skull->Release();
}

// 포인트 라이트 회전 !
void Renderer::TestLightRotation()
{
	static float time = 0;
	PointLights[0]->GetPosition().x = cosf(time * 0.5f) * 2;
	PointLights[0]->GetPosition().z = sinf(time * 0.5f) * cosf(time * 0.5f) * 2;

	PointLights[1]->GetPosition().x = cosf(1.5f * time) * 2;
	PointLights[1]->GetPosition().z = sinf(1 * time) * 2;

	PointLights[2]->GetPosition().x = cosf(0.75f * time) * 1.5f;
	PointLights[2]->GetPosition().z = sinf(1.5f * time) * 1.5f;

	time += TimeSystem::GetInstance()->DeltaTime();
};



