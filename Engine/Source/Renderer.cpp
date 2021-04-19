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

USING(ENGINE)
IMPLEMENT_SINGLETON(Renderer)

Renderer::Renderer(){}

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

}

void Renderer::ReadyLights()
{
	// 달빛
	//DirLights.resize(1u);
	//DirLights[0] = std::make_shared<FLight>
	//	(FLight(FLight::Type::Directional,
	//	{ 0,0,0,0 }, (const D3DXCOLOR&)Color::sRGBToLinear(250, 250, 250)));
	//DirLights[0]->InitRender();
	//DirLights[0]->CreateShadowMap(Device, 2048);
	//DirLights[0]->GetPosition().x = -9.f;
	//DirLights[0]->GetPosition().y = 105.f;
	//DirLights[0]->GetPosition().z = -22.f;
	//DirLights[0]->Direction.x = 71.f;
	//DirLights[0]->Direction.y = -2.f;
	//DirLights[0]->Direction.z = -83.f;

	//DirLights[0]->SetProjectionParameters(60.f,60.f,-1.f,300.f);
	//DirLights[0]->lightFlux = 10.0f;
	//DirLights[0]->lightIlluminance = 1.5f;
	//DirLights[0]->specularPower = 80.0f;
	//DirLights[0]->SetPointRadius (5.0f); // meter

	// PointLights.resize(1u);



	//PointLights[0] = std::make_shared<FLight>(
	//	FLight(
	//		FLight::Type::Point, { 1.5f,0.5f, 0.0f ,1 },
	//		{ 1,0,0,1 }));
	//PointLights[0]->SetPointRadius(7.1f);

	//PointLights[1] = std::make_shared<FLight>(
	//	FLight(
	//		FLight::Type::Point, { -0.7f , 0.5f , 1.2f , 1.f },
	//		{ 0,1,0,1 }));
	//PointLights[1]->SetPointRadius(7.1f);

	//PointLights[2] = std::make_shared<FLight>(
	//	FLight(
	//		FLight::Type::Point,
	//		{ 0.0f,0.5f,0.0f,1 },
	//		{ 0,0,1,1 }));
	//PointLights[2]->SetPointRadius(7.1f);

	// 그림자맵 512 로 생성
	// DirLights[0]->CreateShadowMap(Device, 1024);
	// DirLights[0]->SetProjectionParameters(7.1f, 7.1f, -20.f, +20.f);

	// PointLights[0]->CreateShadowMap(Device, 512);
	/*PointLights[1]->CreateShadowMap(Device, 512);
	PointLights[2]->CreateShadowMap(Device, 512);*/
	
	// PointLights[0]->SetProjectionParameters(0, 0, 0.1f, 10.0f);
	/*PointLights[1]->SetProjectionParameters(0, 0, 0.1f, 10.0f);
	PointLights[2]->SetProjectionParameters(0, 0, 0.1f, 10.0f);*/
}

void Renderer::ReadyRenderTargets()
{
	static const Vector2 RenderTargetDebugRenderSize{ 80.f,80.f };

	 const  float InitX =
		g_nWndCX / -2.f + (RenderTargetDebugRenderSize.x);
	 const  float InitY =
		 g_nWndCY / +2.f - (RenderTargetDebugRenderSize.y);

	 const float YOffset = -RenderTargetDebugRenderSize.y *2.f ;
	 const float XOffset = RenderTargetDebugRenderSize.x * 2.f;
	 const float Interval = 5.f;

	{
		auto& SceneTarget = RenderTargets["SceneTarget"] = 
			std::make_shared<RenderTarget>();

		RenderTarget::Info InitInfo;
		InitInfo.Width = g_nWndCX;
		InitInfo.Height = g_nWndCY ;
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
		InitInfo.Width = g_nWndCX ;
		InitInfo.Height = g_nWndCY ;
		InitInfo.Levels = 1;
		InitInfo.Usages = D3DUSAGE_RENDERTARGET;
		InitInfo.Format = D3DFMT_A8R8G8B8;
		InitInfo._D3DPool = D3DPOOL_DEFAULT;
		ALBM->Initialize(InitInfo);
		ALBM->DebugBufferInitialize(
			{ InitX,InitY + (YOffset  *1.f ) + Interval },
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
			{ InitX + (XOffset* 1.f ) + Interval ,InitY  + Interval },
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
		InitInfo.Width  = 1;
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
			{ InitX + (XOffset * 1.f) + Interval , InitY + (YOffset * 3.f) + Interval } ,
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
				  InitY + (YOffset * i) + ( i > 0 ? Interval : 0 ) },
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
				{     InitX + (XOffset * 6) + Interval,
					  InitY + (YOffset * (i+2)) + (i > 0 ? Interval : 0) },
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

				for (const auto& [_EntityOrder,ShaderKeyCallMap] : _EntityRenderProp.RenderOrders)
				{
					for (const auto& [ShaderKey,Call]: ShaderKeyCallMap)
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
	RenderReady();
	RenderBegin();
	//  쉐도우 패스 
	RenderShadowMaps();
	// 기하 패스
	RenderGBuffer();
	// 디퍼드 렌더링 .
	DeferredShading();
	// 백버퍼로 백업 . 
	Device->SetRenderTarget(0, BackBuffer);
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	RenderSky();
	Tonemapping();
	AlphaBlendEffectRender();
	UIRender();

	ResetState();
	RenderTargetDebugRender();
	RenderDebug();
	LightFrustumRender();
	RendererCollider();
	RenderDebugBone();
	ImguiRender();
	GraphicSystem::GetInstance()->End();
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

		ImGui::Checkbox("LightRender", &bLightRender);
		ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.f);

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
				_Insert->CreateShadowMap(Device, ShadowMapSize);
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
				_Insert->CreateShadowMap(Device, ShadowMapSize);
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

void Renderer::RenderReady()&
{
	RenderReadyEntitys();
	ReadyRenderInfo();
	// TestLightRotation();
};

void Renderer::RenderBegin()&
{
	GraphicSystem::GetInstance()->Begin();
	Device->GetRenderTarget(0, &BackBuffer);
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
	Device->SetViewport(&_RenderInfo.Viewport);
};

void Renderer::RenderEnd()&
{
	_PrevRenderInfo = _RenderInfo;
	RenderEntityClear();

	for (auto iter = DirLights.begin(); iter != DirLights.end(); )
	{
		if(iter->get()->bRemove)
			iter = DirLights.erase(iter);
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
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	auto shadowmap = Shaders["Shadow"]->GetEffect();
	auto Blur = Shaders["Blur"]->GetEffect();

	for (auto& DirLight : DirLights)
	{
		DirLight->RenderShadowMap(Device, [&](FLight* light) {
			D3DXMATRIX  viewproj;
			D3DXVECTOR4 clipplanes(light->GetNearPlane(), light->GetFarPlane(), 0, 0);

			light->CalculateViewProjection(viewproj);

			shadowmap->SetTechnique("variance");
			shadowmap->SetVector("clipPlanes", &clipplanes);
			shadowmap->SetBool("isPerspective", FALSE);

			Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
			
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
						_Call(_DrawInfo);
					}
					Fx->EndPass();
				}
				Fx->End();
			}
			// 렌더 엔드 ... 

			});
	};

	//for (auto& DirLight : DirLights)
	//{
	//	DirLight->BlurShadowMap(Device, [&](FLight* light) {
	//		D3DXVECTOR4 pixelsize(1.0f / light->GetShadowMapSize(),
	//			1.0f / light->GetShadowMapSize(), 0, 0);
	//		D3DXVECTOR4 TexelSize = DirLight->BlurIntencity * pixelsize;
	//		// make it more blurry
	//		Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	//		Blur->SetTechnique("boxblur3x3");
	//		Blur->SetVector("pixelSize", &pixelsize);
	//		Blur->SetVector("texelSize", &TexelSize);

	//		Blur->Begin(NULL, 0);
	//		Blur->BeginPass(0);
	//		{
	//			_Quad->Render(Device);
	//		}
	//		Blur->EndPass();
	//		Blur->End();

	//		Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	//	});
	//}

	// point lights
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	shadowmap->SetBool("isPerspective", TRUE);

	for (auto& PointLight : PointLights)
	{
		Sphere PtlightSphere{};
		PtlightSphere.Center = (D3DXVECTOR3&)PointLight->GetPosition();
		PtlightSphere.Radius = PointLight->GetFarPlane();
		if (false == CameraFrustum->IsIn(PtlightSphere))continue;
		if (PointLight->GetShadowMapSize() <= 0) continue;

		PointLight->RenderShadowMap(Device, [&](FLight* light) {
			D3DXMATRIX viewproj;
			D3DXVECTOR4 clipplanes(
				light->GetNearPlane(), light->GetFarPlane(), 0, 0);

			light->CalculateViewProjection(viewproj);

			shadowmap->SetTechnique("variance");
			shadowmap->SetVector("lightPos", &light->GetPosition());
			shadowmap->SetVector("clipPlanes", &clipplanes);

			Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

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
						_Call(_DrawInfo);
					}
					Fx->EndPass();
				}
				Fx->End();
			}
		});
	}

	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
void Renderer::RenderGBuffer()
{
	auto* const device = Device;
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);

	device->SetRenderTarget (0, RenderTargets["ALBM"]->GetSurface());
	device->SetRenderTarget (1, RenderTargets["NRMR"]->GetSurface());
	device->SetRenderTarget (2, RenderTargets["Depth"]->GetSurface());

	device->SetSamplerState (0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState (0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState (0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState (0, D3DSAMP_MAXANISOTROPY, 8);
	device->SetSamplerState (0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState (0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	// device->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 16);
	device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	auto& GBufferGroup = RenderEntitys[RenderProperty::Order::GBuffer];
	DrawInfo _DrawInfo{};
	_DrawInfo.BySituation.reset();
	_DrawInfo._Device = Device;
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto&  [ ShaderKey , Entitys ] : GBufferGroup)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		Fx->SetMatrix("matViewProj", &_RenderInfo.ViewProjection);
		_DrawInfo.Fx = Fx;
		for (auto&  [ Entity ,Call ] : Entitys)
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
						for (auto& _Light : DirLights)
						{
							_Light->Render(_DrawInfo);
						}
						for (auto& _Light : PointLights)
						{
							_Light->Render(_DrawInfo);
						}
					}

				}
				Fx->EndPass();
			}
			Fx->End();
		}
	}

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	device->SetRenderTarget(1, NULL);
	device->SetRenderTarget(2, NULL);
}

void Renderer::DeferredShading()
{
	D3DXVECTOR4			pixelsize(1, 1, 1, 1);

	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// STEP 2: deferred shading
	pixelsize.x = 1.0f / (float)_RenderInfo.Viewport.Width;
	pixelsize.y = -1.0f / (float)_RenderInfo.Viewport.Height;
	// Device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);

	auto device = Device;

	auto scenesurface = RenderTargets["SceneTarget"]->GetSurface();
	auto albedo = RenderTargets["ALBM"]->GetTexture();
	auto normals= RenderTargets["NRMR"]->GetTexture();
	auto depth = RenderTargets["Depth"]->GetTexture();

	auto deferred = Shaders["DeferredShading"]->GetEffect();

	device->SetRenderTarget(0, scenesurface);
	device->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);


	device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, TRUE);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 16);

	for (int32 i = 1; i < 3; ++i)
	{
		device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(i, D3DSAMP_SRGBTEXTURE, FALSE);
	};

	for (int i = 3; i < 5; ++i) {
		device->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0xffffffff);
		device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		device->SetSamplerState(i, D3DSAMP_SRGBTEXTURE, FALSE);
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

		for (auto& DirLight : DirLights)
		{
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
				deferred->SetFloat("ShadowDepthBias",DirLight->shadowdepthbias);
				deferred->SetFloat("ShadowDepthMapHeight", DirLight->GetShadowMapSize());
				deferred->SetFloat("ShadowDepthMapWidth", DirLight->GetShadowMapSize());
				
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
		// device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

		for (auto& PointLight : PointLights)
		{
			Sphere PtLtSp{};

			PtLtSp.Center = (D3DXVECTOR3&)PointLight->GetPosition();
			PtLtSp.Radius = PointLight->GetPointRadius();
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
			deferred->SetFloat("ShadowDepthMapHeight",PointLight->GetShadowMapSize());
			deferred->SetFloat("ShadowDepthMapWidth",PointLight->GetShadowMapSize());
			deferred->SetBool("IsPoint", true);
			deferred->SetVector("clipPlanes", &clipplanes);
			deferred->SetVector("lightColor", (D3DXVECTOR4*)&PointLight->GetColor());
			deferred->SetVector("lightPos", &PointLight->GetPosition());
			deferred->SetFloat("lightRadius", PointLight->GetPointRadius());
			device->SetTexture(4, PointLight->GetCubeShadowMap());
			deferred->CommitChanges();
			_Quad->Render(Device);
		}

		device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}

	deferred->EndPass();
	deferred->End();

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, FALSE);

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
			for (auto& [_Entity,Call]: _EntityArr)
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
	if (g_bDebugBoneToRoot ==false)return S_OK;

	auto& _Order = RenderEntitys[RenderProperty::Order::DebugBone];
	DrawInfo _DrawInfo{};
	_DrawInfo._Device = Device;
	_DrawInfo.BySituation.reset();
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto& [ShaderKey, _EntityArr] : _Order)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		_DrawInfo.Fx = Fx;
		Vector4 DebugColor {0.3f,0.7f,0.1f,0.5f};
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
			for (auto& [Entity ,Call]: _EntityArr)
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
	auto screenquad = Shaders["ScreenQuad"]->GetEffect();
	screenquad->SetTechnique("screenquad");
	screenquad->Begin(NULL, 0);
	screenquad->BeginPass(0);
	Device->SetTexture(0, sky->GetTexture());
	_Quad->Render(Device);
	screenquad->EndPass();
	screenquad->End(); 

	//Matrix skyview = _RenderInfo.View;
	//skyview._41 = 0.0f;
	//skyview._42 = 0.0f;
	//skyview._43 = 0.0f;
	//const Matrix viewproj = 
	//	skyview* _RenderInfo.Projection;

	//Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//Device->SetSamplerState(0, D3DSAMP_MAGFILTER,
	//	D3DTEXF_LINEAR);
	//Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	//Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	//Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	//DWORD Cull{};
	//DWORD ZWrite{};
	//Device->GetRenderState(D3DRS_CULLMODE, &Cull);
	//Device->GetRenderState(D3DRS_ZWRITEENABLE, &ZWrite);

	//Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//auto Fx =Shaders["sky"]->GetEffect();
	//Fx->SetMatrix("matViewProj", &viewproj);
	//static float skyrotationyaw = 0.0f;
	//skyrotationyaw += (1.f / 60.f);
	//
	//const Matrix rotation= 
	//	FMath::Rotation(Vector3{ 0.f,skyrotationyaw ,0.f });
	//Fx->SetMatrix("matSkyRotation", &rotation);
	//
	//Fx->Begin(NULL, 0);
	//Fx->BeginPass(0);
	//Device->SetTexture(0u, environment);
	//Fx->CommitChanges();
	//skymesh->DrawSubset(0);
	//Fx->EndPass();
	//Fx->End();

	//Device->SetSamplerState(
	//	0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//Device->SetSamplerState
	//	(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//Device->SetSamplerState
	//	(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//Device->SetSamplerState
	//	(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//Device->SetSamplerState
	//	(1, D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
	//Device->SetSamplerState
	//	(1, D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);

	//Device->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//Device->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//Device->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	//Device->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	//Device->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);


	//
	//






	//Device->SetRenderState(D3DRS_CULLMODE, Cull);
	//Device->SetRenderState(D3DRS_ZWRITEENABLE, ZWrite);

	

	return S_OK;
}

HRESULT Renderer::Tonemapping()&
{
	//                     감마보정 수행 . 
	Device->SetRenderState(
		D3DRS_SRGBWRITEENABLE, TRUE);
	Device->SetRenderState(
		D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(
		D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

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
}
HRESULT Renderer::AlphaBlendEffectRender()&
{
	auto& _Group = RenderEntitys[RenderProperty::Order::AlphaBlendEffect];
	DrawInfo _DrawInfo{};
	_DrawInfo.BySituation.reset();
	_DrawInfo._Device = Device;
	_DrawInfo._Frustum = CameraFrustum.get();
	for (auto& [ShaderKey, Entitys] : _Group)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		
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
	return S_OK;
}
HRESULT Renderer::UIRender()&
{
	auto& _Group = RenderEntitys[RenderProperty::Order::UI];
	DrawInfo _DrawInfo{};
	_DrawInfo.BySituation.reset();
	_DrawInfo._Device = Device;
	for (auto& [ShaderKey, Entitys] : _Group)
	{
		auto Fx = Shaders[ShaderKey]->GetEffect();
		Fx->SetMatrix("Ortho", &_RenderInfo.Ortho);
		_DrawInfo.Fx = Fx;
		for (auto& [Entity, Call] : Entitys)
		{
			UINT Passes{ 0u };
			Fx->Begin(&Passes, NULL);
			Call(_DrawInfo);
			Fx->End();
		}
	}

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
		Vector4 DebugColor{ 255.f/255.f,240.f /255.f,140.f/255.f,0.5f };
		const Matrix ScaleOffset = FMath::Scale({ 0.01f,0.01f,0.01f });
		Fx->SetVector("DebugColor", &DebugColor);
		Fx->SetMatrix("ViewProjection", &_RenderInfo.ViewProjection);
		UINT Passes = 0u;
		Fx->Begin(&Passes, NULL);
		for (auto& [Entity, Call] : _EntityArr)
		{
			Fx->BeginPass(0);
			Call(_DrawInfo);
			Fx->EndPass();
		}
		Fx->End();
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
	using namespace rapidjson;

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
		return;

	IStreamWrapper Isw(Is);
	Document _Document{};
	_Document.ParseStream(Isw);

	if (_Document.HasParseError())
	{
		PRINT_LOG(L"Warning!", L"Animation Parse Error!");
		return;
	}
	
	const Value& LightData = _Document["LightDataArray"];
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

};


bool Renderer::TestShaderInit()
{
	if (FAILED(D3DXLoadMeshFromX(
		L"../../Media/MeshesDX/skullocc3.x", D3DXMESH_MANAGED, Device, NULL, NULL, NULL, NULL, &skull)))
		return false;

	if (FAILED(D3DXLoadMeshFromX(L"../../Media/MeshesDX/sky.x", D3DXMESH_MANAGED, Device, NULL, NULL, NULL, NULL, &skymesh)))
		return false;


	if (FAILED(D3DXCreateCubeTextureFromFile(Device, L"../../Media/Textures/grace.dds", &environment)))
		return false;

	if (FAILED(D3DXCreateCubeTextureFromFile(Device, L"../../Media/Textures/grace_diff_irrad.dds", &irradiance1)))
		return false;

	if (FAILED(D3DXCreateCubeTextureFromFile(Device, L"../../Media/Textures/grace_spec_irrad.dds", &irradiance2)))
		return false;

	if (FAILED(D3DXCreateTextureFromFile(Device, L"../../Media/Textures/brdf.dds", &brdfLUT)))
		return false;

	sky = Resources::Load<Texture >("../../Media/Textures/static_sky.jpg");

	RenderTargets;
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
