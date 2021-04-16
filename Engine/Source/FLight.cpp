#include "FLight.h"
#include "imgui.h"
#include "FMath.hpp"


USING(ENGINE)

D3DXVECTOR3 DXCubeForward[6] = {
	D3DXVECTOR3(1, 0, 0),	// +X
	D3DXVECTOR3(-1, 0, 0),	// -X
	D3DXVECTOR3(0, 1, 0),	// +Y
	D3DXVECTOR3(0, -1, 0),	// -Y
	D3DXVECTOR3(0, 0, 1),	// +Z
	D3DXVECTOR3(0, 0, -1),	// -Z
};


D3DXVECTOR3 DXCubeUp[6] = {
	D3DXVECTOR3(0, 1, 0),
	D3DXVECTOR3(0, 1, 0),
	D3DXVECTOR3(0, 0, -1),
	D3DXVECTOR3(0, 0, 1),
	D3DXVECTOR3(0, 1, 0),
	D3DXVECTOR3(0, 1, 0),
};



FLight::FLight(
	const FLight::Type type, 
	const D3DXVECTOR4& position, 
	const D3DXCOLOR& color,
	const float blurIntencity)
{
	this->_Type = type;
	this->Position = position;
	this->Color = color;

	Projparams = D3DXVECTOR4(0, 0, 0, 0);
	Spotdirection = D3DXVECTOR3(0, 0, 0);
	Spotparams = D3DXVECTOR2(0, 0);

	BlurIntencity = blurIntencity;
	Cubeshadowmap = nullptr;
	Blurredcubeshadowmap = nullptr;
	Shadowmap = nullptr;
	Blurredshadowmap = nullptr;
	Currentface = 0;
	ShadowMapSize = 0;
	Blurred = false;
	PointRadius = 7.1f;

	switch (type) {
	case Point:
		this->Position.w = 1;
		break;

	case Directional:
		this->Position.w = 0;
		break;

	case Spot:
		// haha :)
		this->Position.w = 0.75f;
		break;

	default:
		break;
	}
}

FLight::~FLight()
{
	if (Cubeshadowmap != nullptr)
		Cubeshadowmap->Release();

	if (Blurredcubeshadowmap != nullptr)
		Blurredcubeshadowmap->Release();

	if (Shadowmap != nullptr)
		Shadowmap->Release();

	if (Blurredshadowmap != nullptr)
		Blurredshadowmap->Release();

	if (DepthStencil)
		DepthStencil->Release();
	
};

void FLight::Edit(const uint32 Idx)
{
	const std::string Id = "Light Index : " + std::to_string(Idx);
	if (ImGui::CollapsingHeader(Id.c_str()))
	{
		EditImplementation(Idx);
		ImGui::Separator();
	};
};

void FLight::EditImplementation(const uint32 Idx)
{
	std::string TypeString;
	switch (_Type)
	{
	case Directional:
		TypeString = "Directional";
		ImGui::Text("ShadowMap");
		ImGui::Image(reinterpret_cast<void**>(Shadowmap), { 128,128 });
		ImGui::Text("BlurShadowMap");
		ImGui::Image(reinterpret_cast<void**>(Blurredshadowmap), { 128,128 });
		break;
	case Point:
		TypeString = "Point";
		ImGui::Text("CubeShadowMap");
		ImGui::Image(reinterpret_cast<void**>(Cubeshadowmap), { 128,128 });
		ImGui::Text("CubeBlurShadowMap");
		ImGui::Image(reinterpret_cast<void**>(Blurredcubeshadowmap), { 128,128 });
		break;
	default:
		break;
	}
	ImGui::BulletText(TypeString.c_str());
	ImGui::Text("ShadowMapSize %d", ShadowMapSize);

	{
		ImGui::Text("BlurIntencity %3.3f",BlurIntencity);
		static float BlurIntencitySliderPower = 0.01f;
		ImGui::SliderFloat("BlurIntencitySliderPower", &BlurIntencitySliderPower, FLT_MIN, 1.f);
		ImGui::InputFloat("In BlurIntencity", &BlurIntencity);
		float AddBlurIntencity = 0.0f;
		if (ImGui::SliderFloat("Add BlurIntencity", &AddBlurIntencity, -1.f, +1.f, "%3.3f"))
		{
			AddBlurIntencity *= BlurIntencitySliderPower;
		}
		BlurIntencity += AddBlurIntencity;
		ImGui::Separator();


	}

	{
		ImGui::Text("Position : %4.3f, %4.3f, %4.3f", Position.x, Position.y, Position.z);
		static float PositionSliderPower = 0.01f;
		ImGui::SliderFloat("PositionSliderPower", &PositionSliderPower, FLT_MIN, 1.f);
		ImGui::InputFloat3("In Position", Position);
		Vector3 AddPosition{ 0,0,0 };
		if (ImGui::SliderFloat3("Add Position", AddPosition, -1.f, 1.f, "%3.3f"))
		{
			AddPosition *= PositionSliderPower;
		}
		Position.x += AddPosition.x;
		Position.y += AddPosition.y;
		Position.z += AddPosition.z;
		ImGui::Separator();
	}

	{
		ImGui::Text(
			"Direction : Yaw %4.3f, Pitch %4.3f, Roll %4.3f", 
			Direction.y, Direction.x, Direction.z);

		if (ImGui::InputFloat3("In Direction", Direction))
		{
			Direction.x = (Direction.x);
			Direction.y = (Direction.y);
			Direction.z = (Direction.z);
		}

		static float DirectionSliderPower = 0.001f;
		ImGui::SliderFloat("DirectionSliderPower",
			&DirectionSliderPower, FLT_MIN, 1.f);
		Vector3  AddDirection{ 0,0,0 };
		if (ImGui::SliderFloat3("Add AddDirection", AddDirection, 
			-360.f, 360.f, "%3.3f deg"))
		{
			AddDirection *= DirectionSliderPower;
			Direction += AddDirection;
			
		}
		Direction.x = (Direction.x);
		Direction.y = (Direction.y);
		Direction.z = (Direction.z);

		/*if (ImGui::SliderFloat3("Slider Direction", Direction,-360.f,360.f))
		{
			Direction.x = FMath::ToRadian(Direction.x);
			Direction.y = FMath::ToRadian(Direction.y);
			Direction.z = FMath::ToRadian(Direction.z);

		}*/
		ImGui::Separator();
	}

	{
		ImGui::Text("Point Radius : %4.3f ", PointRadius);
		static float PointRadiusSliderPower = 0.01f;
		ImGui::SliderFloat("PointRadiusSliderPower", 
			&PointRadiusSliderPower, FLT_MIN, 1.f);
		ImGui::InputFloat3("In PointRadius",&PointRadius);
		float  AddPointRadius{ 0.0f };
		if (ImGui::SliderFloat("Add PointRadius",&AddPointRadius, -1.f, 1.f, "%3.3f"))
		{
			AddPointRadius *= PointRadiusSliderPower;
		}
		PointRadius += AddPointRadius;
		ImGui::Separator();
	}


	{
		ImGui::Text("Projection Size : %4.4f , %4.4f", Projparams.x, Projparams.y);
		static float ProjectionSizeSliderPower = 0.01f;
		ImGui::SliderFloat("ProjectionSliderPower", &ProjectionSizeSliderPower, FLT_MIN, 1.f);
		ImGui::InputFloat2("In ProjectionSize", Projparams);
		Vector2 AddProjectionSize = { 0.f ,0.f };
		if (ImGui::SliderFloat2("Add ProjectionSize", AddProjectionSize, -1.f, 1.f))
		{
			AddProjectionSize *= ProjectionSizeSliderPower;
		}
		Projparams.x += AddProjectionSize.x;
		Projparams.y += AddProjectionSize.y;
		ImGui::Separator();
	}

	{
		ImGui::Text("Near Far %4.4f , %4.4f ", Projparams.z, Projparams.w);
		static float NearFarSliderPower = 0.01f;
		ImGui::SliderFloat("NearFarSliderPower", &NearFarSliderPower, FLT_MIN, 1.f);
		ImGui::InputFloat("In Near", &Projparams.z);
		{
			float AddNear = 0.0f;
			if (ImGui::SliderFloat("Add Near", &AddNear, -1.f, 1.f))
			{
				AddNear *= NearFarSliderPower;
			}
			Projparams.z += AddNear;
	
		}

		{
			float AddFar = 0.0f;
			ImGui::InputFloat("In Far", &Projparams.w);
			if (ImGui::SliderFloat("Add Far", &AddFar, -1.f, 1.f))
			{
				AddFar *= NearFarSliderPower;
			}
			Projparams.w += AddFar;
			ImGui::Separator();
			
			ImGui::Separator();
		}
	}

	{
		ImGui::ColorEdit4("Light Color", Color);
		ImGui::Separator();
	}

	{
		ImGui::Text("lightFlux : %4.4f", lightFlux);
		static float lightFluxSliderPower = 0.01f;
		ImGui::SliderFloat("lightFluxSliderPower", &lightFluxSliderPower, FLT_MIN, 1.f);
		float AddlightFlux = 0.0f;
		ImGui::InputFloat2("In lightFlux", &lightFlux);
		if (ImGui::SliderFloat("Add lightFlux", &AddlightFlux, -1.f, 1.f))
		{
			AddlightFlux *= lightFluxSliderPower;
		}
		lightFlux += AddlightFlux;
		
		ImGui::Separator();
	}

	{
		ImGui::Text("lightIlluminance : %4.4f", lightIlluminance);
		static float lightIlluminanceSliderPower = 0.01f;
		ImGui::SliderFloat("lightIlluminanceSliderPower", &lightIlluminanceSliderPower, FLT_MIN, 1.f);
		float AddlightIlluminance = 0.0f;
		ImGui::InputFloat2("lightIlluminance", &lightIlluminance);
		if (ImGui::SliderFloat("Add lightIlluminance", &AddlightIlluminance, -1.f, 1.f))
		{
			AddlightIlluminance *= lightIlluminanceSliderPower;
		}
		lightIlluminance += AddlightIlluminance;
		
		ImGui::Separator();

	}

	{
		ImGui::Text("specularPower : %4.4f", specularPower);
		static float specularPowerSliderPower = 0.01f;
		ImGui::SliderFloat("specularPowerSliderPower", &specularPowerSliderPower, FLT_MIN, 1.f);
		float AddspecularPower = 0.0f;
		ImGui::InputFloat2("In specularPower", &specularPower);

		if (ImGui::SliderFloat("Add specularPower", &AddspecularPower, -1.f, 1.f))
		{
			AddspecularPower *= specularPowerSliderPower;
		}
		specularPower += AddspecularPower;
		

		ImGui::Separator();
	}

	{
		ImGui::Text("cosAngularRadius : %4.4f", cosAngularRadius);
		static float AddcosAngularRadiusSliderPower = 0.01f;
		ImGui::SliderFloat("AddcosAngularRadiusSliderPower", &AddcosAngularRadiusSliderPower, FLT_MIN, 1.f);
		float AddcosAngularRadius = 0.0f;
		ImGui::InputFloat2("In cosAngularRadius", &cosAngularRadius);

		if (ImGui::SliderFloat("Add cosAngularRadius", &AddcosAngularRadius, -1.f, 1.f))
		{
			AddcosAngularRadius *= AddcosAngularRadiusSliderPower;
		}
		cosAngularRadius += AddcosAngularRadius;
		ImGui::Separator();

	}

	{
		ImGui::Text("sinAngularRadius : %4.4f", sinAngularRadius);
		static float sinAngularRadiusSliderPower = 0.01f;
		ImGui::SliderFloat("AddsinAngularRadiusSliderPower", &sinAngularRadiusSliderPower, FLT_MIN, 1.f);
		ImGui::InputFloat2("In sinAngularRadius", &sinAngularRadius);
		float AddsinAngularRadius = 0.0f;
		if (ImGui::SliderFloat("Add sinAngularRadius", &AddsinAngularRadius, -1.f, 1.f))
		{
			AddsinAngularRadius *= sinAngularRadiusSliderPower;
		}
		sinAngularRadius += AddsinAngularRadius;
		ImGui::Separator();
	}
};

void FLight::CalculateViewProjection(D3DXMATRIX& out)
{
	if (_Type == Directional) {
		D3DXVECTOR3 eye = { Position.x , Position.y , Position.z };
		D3DXVECTOR3 look(0, 0, 0);
		D3DXVECTOR3 up(0, 1, 0);

		/*if (fabs(Position.y) > 0.999f)
			up = D3DXVECTOR3(1, 0, 0);*/
		
		Vector3 At = eye + FMath::MulNormal(Vector3{ 0,0,1 },
			FMath::Rotation({ 
				FMath::ToRadian( Direction.x  ) ,
				FMath::ToRadian( Direction.y ),
				FMath::ToRadian( Direction.z )  }));
		D3DXMatrixLookAtLH(&out, &eye, &At, &up);
		D3DXMatrixOrthoLH(&this->proj, 
			Projparams.x, Projparams.y, Projparams.z, Projparams.w);
		D3DXMatrixInverse(&this->viewinv, nullptr, &out);
		D3DXMatrixMultiply(&out, &out, &this->proj);
	}
	else if (_Type == Point) {
		D3DXVECTOR3 eye = { 
			Position.x , Position.y , Position.z }; 

		D3DXMatrixPerspectiveFovLH(&this->proj, 
			D3DX_PI / 2.f, 1.f, Projparams.z, Projparams.w);
		const Vector3 At = eye + DXCubeForward[Currentface];

		D3DXMatrixLookAtLH(&out, &eye, &At, &DXCubeUp[Currentface]);
		D3DXMatrixInverse(&this->viewinv, nullptr, &out);
		D3DXMatrixMultiply(&out, &out, &this->proj);
	}
	else if (_Type == Spot) {
		// TODO:
	}
}

void FLight::CalculateScissorRect(RECT& out, const D3DXMATRIX& view, const D3DXMATRIX& proj, float radius, int32_t width, int32_t height)
{
	// 카메라의 right 벡터를 얻어온다 . 
	/*
	std::memcpy(&camera_right, &viewinv._11, sizeof(Vector3));*/

	static const Vector3 to_RT = FMath::RotationVecNormal({ 1,0,0 }, { 0,0,1 }, FMath::ToRadian(45.f));
	static const Vector3 to_LB = FMath::RotationVecNormal({ 1,0,0 }, { 0,0,1 }, FMath::ToRadian(225.f));

	Vector3 view_position{};
	D3DXVec3TransformCoord(&view_position,(Vector3*)&Position, &view);

	Vector3 RT = view_position + (to_RT * PointRadius);
	Vector3 LB = view_position + (to_LB * PointRadius);

	RT = FMath::Mul(RT, proj);
	LB = FMath::Mul(LB, proj);

	
	Vector2 RTScreenPos = FMath::NDCToScreenCoord(RT.x, RT.y, (float)width, (float)height);
	Vector2 LBScreenPos = FMath::NDCToScreenCoord(LB.x, LB.y, (float)width, (float)height);

	out.left = LBScreenPos.x;
	out.bottom = LBScreenPos.y;

	out.right = RTScreenPos.x;
	out.top = RTScreenPos.y;
	LastScissorRect = out;
	//if (_Type != Point)
	//	return;

	//D3DXVECTOR4 Q;
	//D3DXVECTOR3 L, P1, P2;
	//float u, v;

	//out.left = 0;
	//out.right = width;
	//out.top = 0;
	//out.bottom = height;

	//D3DXVec3TransformCoord(&L, (D3DXVECTOR3*)&Position, &view);

	//float D = 4 * (radius * radius * L.x * L.x - (L.x * L.x + L.z * L.z) * (radius * radius - L.z * L.z));

	//if (D > 0.0f) {
	//	float Nx1 = (radius * L.x + sqrtf(D * 0.25f)) / (L.x * L.x + L.z * L.z);
	//	float Nx2 = (radius * L.x - sqrtf(D * 0.25f)) / (L.x * L.x + L.z * L.z);
	//	float Nz1 = (radius - Nx1 * L.x) / L.z;
	//	float Nz2 = (radius - Nx2 * L.x) / L.z;

	//	P1.x = L.x - Nx1 * radius;
	//	P1.y = 0.0f;
	//	P1.z = L.z - Nz1 * radius;

	//	P2.x = L.x - Nx2 * radius;
	//	P2.y = 0.0f;
	//	P2.z = L.z - Nz2 * radius;

	//	// left-handed: >
	//	if (P1.z < 0.0f && P2.z < 0.0f) {
	//		D3DXVec3Transform(&Q, &P1, &proj);

	//		Q /= Q.w;
	//		u = (Q.x * 0.5f + 0.5f) * width;

	//		D3DXVec3Transform(&Q, &P2, &proj);

	//		Q /= Q.w;
	//		v = (Q.x * 0.5f + 0.5f) * width;

	//		if (P2.x < L.x) {
	//			out.left = (LONG)max(v, 0.0f);
	//			out.right = (LONG)min(u, width);
	//		}
	//		else {
	//			out.left = (LONG)max(u, 0.0f);
	//			out.right = (LONG)min(v, width);
	//		}
	//	}
	//}

	//D = 4 * (radius * radius * L.y * L.y - (L.y * L.y + L.z * L.z) * (radius * radius - L.z * L.z));

	//if (D >= 0.0f) {
	//	float Ny1 = (radius * L.y + sqrtf(D * 0.25f)) / (L.y * L.y + L.z * L.z);
	//	float Ny2 = (radius * L.y - sqrtf(D * 0.25f)) / (L.y * L.y + L.z * L.z);
	//	float Nz1 = (radius - Ny1 * L.y) / L.z;
	//	float Nz2 = (radius - Ny2 * L.y) / L.z;

	//	P1.x = 0.0f;
	//	P1.y = L.y - Ny1 * radius;
	//	P1.z = L.z - Nz1 * radius;

	//	P2.x = 0.0f;
	//	P2.y = L.y - Ny2 * radius;
	//	P2.z = L.z - Nz2 * radius;

	//	// left-handed: >
	//	if (P1.z < 0.0f && P2.z < 0.0f) {
	//		D3DXVec3Transform(&Q, &P1, &proj);

	//		Q /= Q.w;
	//		u = (Q.y * -0.5f + 0.5f) * height;

	//		D3DXVec3Transform(&Q, &P2, &proj);

	//		Q /= Q.w;
	//		v = (Q.y * -0.5f + 0.5f) * height;

	//		if (P2.y < L.y) {
	//			out.top = (LONG)max(u, 0.0f);
	//			out.bottom = (LONG)min(v, height);
	//		}
	//		else {
	//			out.top = (LONG)max(v, 0.0f);
	//			out.bottom = (LONG)min(u, height);
	//		}
	//	}
	//}
}

void FLight::CreateShadowMap(LPDIRECT3DDEVICE9 device, uint16_t size)
{
	ShadowMapSize = size;

	if (_Type == Directional) {
		device->CreateTexture(size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G32R32F, D3DPOOL_DEFAULT, &Shadowmap, NULL);
		device->CreateTexture(size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G32R32F, D3DPOOL_DEFAULT, &Blurredshadowmap, NULL);
	}
	else if (_Type == Point) {
		device->CreateCubeTexture(size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G32R32F, D3DPOOL_DEFAULT, &Cubeshadowmap, NULL);
		device->CreateCubeTexture(size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G32R32F, D3DPOOL_DEFAULT, &Blurredcubeshadowmap, NULL);
	}

	D3DVIEWPORT9 viewport; 
	device->GetViewport(&viewport);
	if (size > viewport.Y)
	{
		device->CreateDepthStencilSurface(size, size,
			D3DFMT_D24X8, D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE,
			0, TRUE, &DepthStencil, nullptr);
	}
}

void FLight::RenderShadowMap(
	LPDIRECT3DDEVICE9 _Device, 
	std::function<void(FLight*)> CallBack)
{
	D3DVIEWPORT9 OldViewPort;
	D3DVIEWPORT9 Viewport;

	LPDIRECT3DSURFACE9 OldSurface = NULL;
	LPDIRECT3DSURFACE9 Surface = NULL;

	_Device->GetRenderTarget(0, &OldSurface);
	_Device->GetViewport(&OldViewPort);

	if (_Type == Directional) {
		Shadowmap->GetSurfaceLevel(0, &Surface);

		Viewport.X = Viewport.Y = 0;
		Viewport.Width = Viewport.Height = ShadowMapSize;
		Viewport.MinZ = 0;
		Viewport.MaxZ = 1;

		_Device->SetRenderTarget(0, Surface);
		_Device->SetViewport(&Viewport);


		LPDIRECT3DSURFACE9 OldDepthStencil = NULL;
		if (DepthStencil)
		{
			_Device->GetDepthStencilSurface(&OldDepthStencil);
			_Device->SetDepthStencilSurface(DepthStencil);
		}
		{
			CallBack(this);
		}
		Surface->Release();
		if (DepthStencil)
		{
			_Device->SetDepthStencilSurface(OldDepthStencil);
			OldDepthStencil->Release();
		}
	}
	else if (_Type == Point) {
		for (Currentface= 0; Currentface < 6; ++Currentface) {

			Cubeshadowmap->GetCubeMapSurface(
				(D3DCUBEMAP_FACES)Currentface, 0, &Surface);

			Viewport.X = Viewport.Y = 0;
			Viewport.Width = Viewport.Height = ShadowMapSize;
			Viewport.MinZ = 0;
			Viewport.MaxZ = 1;

			_Device->SetRenderTarget(0, Surface);
			_Device->SetViewport(&Viewport);
			LPDIRECT3DSURFACE9 OldDepthStencil = NULL;
			if (DepthStencil)
			{
				_Device->GetDepthStencilSurface(&OldDepthStencil);
				_Device->SetDepthStencilSurface(DepthStencil);
			}
			{
				CallBack(this);
			}
			Surface->Release();
			if (DepthStencil)
			{
				_Device->SetDepthStencilSurface(OldDepthStencil);
				OldDepthStencil->Release();
			}
		}
	}

	Blurred = false;

	_Device->SetRenderTarget(0, OldSurface);
	_Device->SetViewport(&OldViewPort);
	OldSurface->Release();
}

void FLight::BlurShadowMap(LPDIRECT3DDEVICE9 device, std::function<void(FLight*)> callback)
{
	D3DVIEWPORT9 oldviewport;
	D3DVIEWPORT9 viewport;

	LPDIRECT3DSURFACE9 oldsurface = NULL;
	LPDIRECT3DSURFACE9 surface = NULL;

	device->GetRenderTarget(0, &oldsurface);
	device->GetViewport(&oldviewport);

	if (_Type == Directional) {
		Blurredshadowmap->GetSurfaceLevel(0, &surface);
		// 디렉셔녈 라이트.

		viewport.X = viewport.Y = 0;
		viewport.Width = viewport.Height = ShadowMapSize;
		viewport.MinZ = 0;
		viewport.MaxZ = 1;

		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		device->SetRenderTarget(0, surface);
		device->SetViewport(&viewport);
		device->SetTexture(0, Shadowmap);
		{
			callback(this);
		}
		surface->Release();
	}
	else if(_Type == Point){
		for (Currentface = 0; Currentface < 6; ++Currentface) {
			/*Blurredcubeshadowmap->GetCubeMapSurface
				((D3DCUBEMAP_FACES)Currentface, 0, &surface);
			
			viewport.X = viewport.Y = 0;
			viewport.Width = viewport.Height = ShadowMapSize;
			viewport.MinZ = 0;
			viewport.MaxZ = 1;

			device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

			device->SetRenderTarget(0, surface);
			device->SetViewport(&viewport);
			device->SetTexture(0, Cubeshadowmap);
			
			callback(this);

			surface->Release();*/
		}
	}

	Blurred = true;

	device->SetRenderTarget(0, oldsurface);
	device->SetViewport(&oldviewport);

	oldsurface->Release();
}

void FLight::SetProjectionParameters(float xsize, float ysize, float znear, float zfar)
{
	Projparams.x = xsize;
	Projparams.y = ysize;
	Projparams.z = znear;
	Projparams.w = zfar;
}



void FLight::SetSpotParameters(const D3DXVECTOR3& dir, float inner, float outer)
{
	Spotdirection = dir;
	Spotparams.x = cosf(inner);
	Spotparams.y = cosf(outer);
}


