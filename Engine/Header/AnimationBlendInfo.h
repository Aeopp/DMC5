#ifndef _ANIMATIONBLENDINFO_H_
#define _ANIMATIONBLENDINFO_H_
#include "EngineDefine.h"
#include "EngineTypedef.h"
#include <string>

BEGIN(ENGINE)
struct  ENGINE_DLL AnimationBlendInfo
{
	std::string PrevAnimationName{ 0u };
	 double PrevAnimationWeight = 0.0;
	 double AnimationTime{ 0.0 };
};
END

#endif // !_ANIMATIONBLENDINFO_H_




