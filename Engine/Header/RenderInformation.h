#ifndef _RENDERINFORMATION_H_
#define _RENDERINFORMATION_H_
#include "EngineTypedef.h"
#include "EngineDefine.h"
#include "RenderProperty.h"
#include "FMath.hpp"

BEGIN(ENGINE)
class ENGINE_DLL RenderInformation
{
public:
	D3DVIEWPORT9 Viewport{};
	Vector4 Eye{0,0,0,1};
	Matrix  View{}; 
	Matrix  ViewInverse{};
	Matrix  Projection{};
	Matrix  ProjectionInverse{};
	Matrix  ViewProjection{};
	Matrix  ViewProjectionInverse{};
	Matrix  Ortho{};
};
END
#endif // !_RENDERINFORMATION_H_



