#ifndef _FLIGHT_H_

#define _FLIGHT_H_
#include "EngineDefine.h"
#include "EngineTypedef.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <functional>
#include "StaticMesh.h"
#include "Texture.h"
#include "FMath.hpp"



BEGIN(ENGINE)

class ENGINE_DLL FLight
{
public:
	enum Type : uint8 
	{
		Point = 0,
		Directional,
		Spot,
	};
private:

public:
	FLight();
	FLight(Type _Type, 
		const D3DXVECTOR4& position, const D3DXCOLOR& color,
		const float blurintercity=4.f);
	~FLight();

	void CalculateViewProjection(D3DXMATRIX& Out);
	void CalculateScissorRect(RECT& Out, const D3DXMATRIX& View, const D3DXMATRIX& Projection, float Radius, int32_t Width, int32_t Height);

	void InitRender();
	void Render(const DrawInfo& _Info);

	void CreateShadowMap(LPDIRECT3DDEVICE9 _Device, const uint16_t Size);
	void RenderShadowMap(LPDIRECT3DDEVICE9 _Device, std::function<void(FLight*)> CallBack);

	Matrix GetWorld();
	// �ٸ� �����ä����κ��� ������ �� . 
	void BlurShadowMap(LPDIRECT3DDEVICE9 _Device, std::function<void(FLight*)> CallBack);

	// ������ ���� near far 
	void SetProjectionParameters(const float XSize, const float YSize, const float ZNear, const float ZFar);
	// ���� inner outer cos ����.
	void SetSpotParameters(const D3DXVECTOR3& Direction , const float Inner, const float Outer);

	inline void SetPosition(const D3DXVECTOR4& Newpos) { Position = Newpos; }

	inline float GetNearPlane() const { return Projparams.z; }
	inline float GetFarPlane() const { return Projparams.w; }
	inline uint16_t GetShadowMapSize() const { return ShadowMapSize; }
	inline const D3DXVECTOR2& GetSpotParameters() const { return Spotparams; }

	inline D3DXVECTOR4& GetPosition()     { return Position; }
	inline D3DXVECTOR3& GetSpotDirection() { return Spotdirection; }
	inline D3DXCOLOR& GetColor() { return Color; }
	inline float GetPointRadius()const& { return PointRadius;  };
	inline void  SetPointRadius(const float _PtRadius)& { PointRadius = _PtRadius;  };
	inline bool IsPerspective()const& {return _Type!=Directional;};
	inline LPDIRECT3DTEXTURE9 GetShadowMap() { return (Blurred ? Blurredshadowmap : Shadowmap); }
	inline LPDIRECT3DCUBETEXTURE9 GetCubeShadowMap() { return (Blurred ? Blurredcubeshadowmap : Cubeshadowmap); }

	void Save();
	void Load();

	std::shared_ptr<StaticMesh> _Mesh{};
	std::shared_ptr<Texture> _Texture{};

	RECT LastScissorRect{ 0,0,0,0 };

public:
	Vector3 GetDirection()
	{
		return FMath::Normalize(FMath::MulNormal(Vector3{ 0,0,1 },
			FMath::Rotation({
			FMath::ToRadian(Direction.x) ,
			FMath::ToRadian(Direction.y),
			FMath::ToRadian(Direction.z) })));
	}

	D3DXVECTOR4				Position;	// or direction
	D3DXVECTOR3				Spotdirection;
	D3DXVECTOR2				Spotparams;	// cos(inner), cos(outer)
	float				    PointRadius;
	D3DXCOLOR				Color;
	LPDIRECT3DCUBETEXTURE9	Cubeshadowmap;
	std::array<LPDIRECT3DSURFACE9,6u> CubeshadowmapSurface;
	LPDIRECT3DCUBETEXTURE9	Blurredcubeshadowmap;
	std::array<LPDIRECT3DSURFACE9, 6u> BlurredcubeshadowmapSurface;
	LPDIRECT3DTEXTURE9		Shadowmap;
	LPDIRECT3DSURFACE9      ShadowmapSurface{ nullptr };
	LPDIRECT3DTEXTURE9      CacheShadowMap{ nullptr };
	LPDIRECT3DSURFACE9      CacheShadowMapSurface{ nullptr };
	LPDIRECT3DTEXTURE9		Blurredshadowmap;
	LPDIRECT3DSURFACE9      BlurredshadowmapSurface{ nullptr };
	LPDIRECT3DSURFACE9      DepthStencil{};
	LPDIRECT3DSURFACE9      CacheDepthStencil{nullptr};
	
	Vector3 Lradiance{ 1,1,1 };


	uint16_t				ShadowMapSize;
	bool					Blurred;


	Type				    _Type;
	bool bRemove = false;
	float shadowmin = 0.0f;
	D3DXVECTOR4				Projparams;
	int						Currentface;
	D3DXVECTOR3             Direction{ 0,0,0 };
	float                   BlurIntencity;
	float  lightFlux;
	float  lightIlluminance = 1.5f;
	float  specularPower = 80.f;
	float  cosAngularRadius = 0.999989f; 
	float  sinAngularRadius = 0.004625f;
	float  shadowdepthbias = 1e-5f;

	Matrix viewinv;
	Matrix proj;
public:
	void Edit(const uint32 Idx);
private:
	void EditImplementation(const uint32 Idx);
};
END

#endif // !_FLIGHT_H_

