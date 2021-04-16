#ifndef _Frustum_H_
#define _Frustum_H_ 
#include "EngineDefine.h"
#include "EngineTypedef.h"
#include "MathStruct.hpp"
#include <d3d9.h>
#include <d3dx9.h>
#include <array>
BEGIN(ENGINE)
	class ENGINE_DLL Frustum  
	{
	public:
		void Initialize(IDirect3DDevice9* const Device)&;
		void Release()&;
		~Frustum()noexcept { Release(); };
	public:
		void Make(const Matrix& CameraWorld, const Matrix& Projection)&;
		bool IsIn(const Vector3& Point)&;
		bool IsIn(const Sphere& _Sphere)&;
		void DebugRender(IDirect3DDevice9* const Device)&;
	private:
		Matrix World{};
		IDirect3DVertexBuffer9* VertexBuffer{ nullptr };
		IDirect3DIndexBuffer9* IndexBuffer{ nullptr };
		std::array<D3DXPLANE, 6u> Planes;
		std::array<Vector3, 8u> Points;
	};
END

#endif // Frustum