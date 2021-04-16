#ifndef _ANIMATIONTRACK_H_
#define _ANIMATIONTRACK_H_
#include "EngineDefine.h"
#include "EngineTypedef.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <map>

BEGIN(ENGINE)
struct ENGINE_DLL AnimationTrack
{
	//       �ִϸ��̼� �̸��� Ÿ�Ӷ��� ����...
		std::map<double, Vector3>   ScaleTimeLine{};
		std::map<double, Quaternion> QuatTimeLine{};
		std::map<double, Vector3>   PosTimeLine{};
};
END

#endif // !_ANIMATIONTRACK_H_




