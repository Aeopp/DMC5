#ifndef __DYNAMICLIGHT_H_
#define __DYNAMICLIGHT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "FLight.h"
#include <optional>

class DynamicLight
{
public:
	std::pair<float, float >PointRadius{ 1.f,2.f };
	std::pair<float, float > Flux = { 0.1f,0.2f };
	std::pair<Vector4, Vector4>Color{ Vector4{1.f,1.f,1.f,1.f }, Vector4{1.f,1.f,1.f,1.f} };
public:
	void PlayStart(const Vector3& Location,const float PlayTime);
	void Update(const float Dt, const Vector3& Location);
	void Update(const D3DXCOLOR& _Color, const float Radius, const float Flux, const Vector3& Location);
	void UpdatePosition(const Vector3& Location);
	void PlayEnd();
	void Editor();
private:
	std::weak_ptr<FLight> _PtLight{};
	float T = 0.0f;
	float PlayTime = 0.12f;
};
#endif //
