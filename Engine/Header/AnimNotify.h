#ifndef _ANIMNOTIFY_H_
#define _ANIMNOTIFY_H_
#include "EngineDefine.h"
#include "EngineTypedef.h"
#include <string>
#include <map>
#include <functional>

BEGIN(ENGINE)
struct ENGINE_DLL AnimNotify
{
	std::map<double,std::function<bool()>,std::greater<float>> 
			Event{};
};
END
#endif // !_ANIMNOTIFY_H_




