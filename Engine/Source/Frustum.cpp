#include "Frustum.h"
#include "FMath.hpp"
#include <algorithm>

USING(ENGINE)
void Frustum::Initialize(IDirect3DDevice9* const Device)&
{
	Device->CreateVertexBuffer(
		8 * sizeof(Vector3),
		D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ,
		D3DPOOL_MANAGED,
		&VertexBuffer,
		0);

	Vector3* Vertices;
	VertexBuffer->Lock(0, 0, (void**)&Vertices, 0);

	Vertices[0] = Vector3(-1.0f, -1.0f, 0.f);
	Vertices[1] = Vector3(-1.0f, 1.0f, 0.f);
	Vertices[2] = Vector3(1.0f, 1.0f, 0.f);
	Vertices[3] = Vector3(1.0f, -1.0f, 0.f);
	Vertices[4] = Vector3(-1.0f, -1.0f, 1.0f);
	Vertices[5] = Vector3(-1.0f, 1.0f, 1.0f);
	Vertices[6] = Vector3(1.0f, 1.0f, 1.0f);
	Vertices[7] = Vector3(1.0f, -1.0f, 1.0f);

	VertexBuffer->Unlock();

	Device->CreateIndexBuffer(
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&IndexBuffer,
		0);

	WORD* Indices = 0;
	IndexBuffer->Lock(0, 0, (void**)&Indices, 0);

	// front side
	Indices[0] = 0; Indices[1] = 1; Indices[2] = 2;
	Indices[3] = 0; Indices[4] = 2; Indices[5] = 3;

	// back side
	Indices[6] = 4; Indices[7] = 6; Indices[8] = 5;
	Indices[9] = 4; Indices[10] = 7; Indices[11] = 6;

	// left side
	Indices[12] = 4; Indices[13] = 5; Indices[14] = 1;
	Indices[15] = 4; Indices[16] = 1; Indices[17] = 0;

	// right side
	Indices[18] = 3; Indices[19] = 2; Indices[20] = 6;
	Indices[21] = 3; Indices[22] = 6; Indices[23] = 7;

	// top
	Indices[24] = 1; Indices[25] = 5; Indices[26] = 6;
	Indices[27] = 1; Indices[28] = 6; Indices[29] = 2;

	// bottom
	Indices[30] = 4; Indices[31] = 0; Indices[32] = 3;
	Indices[33] = 4; Indices[34] = 3; Indices[35] = 7;

	IndexBuffer->Unlock();
}

void Frustum::Release()&
{
	if (VertexBuffer)
		VertexBuffer->Release();
	if (IndexBuffer)
		IndexBuffer->Release();

	
}

void Frustum::Make(const Matrix& CameraWorld, const Matrix& Projection)&
{
	// 시계 방향 되감기 순서로 좌측 하단부터 시작하였을때
	// Near 평면 0->4->5->1
	// Far 평면 3->7->6->2
	Points[0].x = -1.f, Points[0].y = -1.f, Points[0].z = 0.f;
	Points[1].x = +1.f, Points[1].y = -1.f, Points[1].z = 0.f;
	Points[2].x = +1.f, Points[2].y = -1.f, Points[2].z = 1.f;
	Points[3].x = -1.f, Points[3].y = -1.f, Points[3].z = 1.f;

	Points[4].x = -1.f, Points[4].y = +1.f, Points[4].z = 0.f;
	Points[5].x = +1.f, Points[5].y = +1.f, Points[5].z = 0.f;
	Points[6].x = +1.f, Points[6].y = +1.f, Points[6].z = 1.f;
	Points[7].x = -1.f, Points[7].y = +1.f, Points[7].z = 1.f;

	// 원근투영행렬을 p라 하고 절두체를 f라 하였을때.
	// 렌더링 할때 View 좌표시스템에서 f 의 내부에 존재한 정점들을
	// -1 ~ +1 (xy) 0 ~ +1 (z) 로 정규화한 좌표시스템(NDC)으로 바꿀수 있다.

	// 반대로 NDC 좌표에서의 모든 정점을 포함하는
	// Box를 모델링 한 다음 p^-1 를 곱하면
	// p를 정의할때 모델링한 f를 얻을 수 있다.
	// f 안의 정점들을 NDC로 (p를 곱해서) 바꾸었으니
	// NDC를 모델링한 Box에 p^-1 를 곱하면 f를 얻을 수 있는 것. 
	// 여기서 얻은 f는 f의 로컬좌표상이므로 카메라의 월드행렬을 곱해서
	// f를 카메라 위치로 이동시켜서 월드좌표에서의 f로 만든다.

	const Matrix InvProjection = FMath::Inverse(Projection);
	World = InvProjection * CameraWorld;

	std::transform(std::begin(Points), std::end(Points), std::begin(Points),
		[_World = World](const Vector3& Point)
		{
			return FMath::Mul(Point, _World);
		});

	// Near
	D3DXPlaneFromPoints(&Planes[0], &Points[1], &Points[4], &Points[0]);
	// Far
	D3DXPlaneFromPoints(&Planes[1], &Points[3], &Points[6], &Points[2]);
	// Left
	D3DXPlaneFromPoints(&Planes[2], &Points[0], &Points[4], &Points[3]);
	// Right
	D3DXPlaneFromPoints(&Planes[3], &Points[2], &Points[5], &Points[1]);
	// Top
	D3DXPlaneFromPoints(&Planes[4], &Points[5], &Points[7], &Points[4]);
	// Botton
	D3DXPlaneFromPoints(&Planes[5], &Points[2], &Points[1], &Points[3]);
}

bool Frustum::IsIn(const Vector3& Point)&
{
	return std::all_of(std::begin(Planes), std::end(Planes),
		[Point](const D3DXPLANE& Plane)
		{
			return D3DXPlaneDotCoord(&Plane, &Point) > 0.f;
		});
};

bool Frustum::IsIn(const Sphere& _Sphere)&
{
	return std::all_of(std::begin(Planes), std::end(Planes),
		[_Sphere](const D3DXPLANE& Plane)
		{
			const float Distance = D3DXPlaneDotCoord(&Plane, &_Sphere.Center);
			return  (Distance >= _Sphere.Radius) || (std::fabsf(Distance) <= _Sphere.Radius);
		});
};

void Frustum::DebugRender(IDirect3DDevice9* const Device)&
{
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetTransform(D3DTS_WORLD, &World);
	Device->SetStreamSource(0, VertexBuffer, 0, sizeof(Vector3));
	Device->SetIndices(IndexBuffer);
	Device->SetFVF(D3DFVF_XYZ);
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
}