#ifndef __TEXTUREINFORMATION_H__
#define __TEXTUREINFORMATION_H__
#include "assimp/material.h"
#include "Resource.h"
#include "EngineTypedef.h"

BEGIN(ENGINE)
class ENGINE_DLL TextureInformation 
{
public:
	uint32 UVIdx       = { 0u };
	float  BlendFactor = { 0.f };
	aiTextureOp TextureOperator =  { aiTextureOp::_aiTextureOp_Force32Bit };
	aiTextureMapMode MappingMode = { aiTextureMapMode::_aiTextureMapMode_Force32Bit };
	aiTextureMapping MappingType = { aiTextureMapping::_aiTextureMapping_Force32Bit };
};
END
#endif // !__TEXTUREINFORMATION_H__