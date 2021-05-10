#ifndef __PARTICLEINSTANCEDESC_H_
#define __PARTICLEINSTANCEDESC_H_
#include "GameObject.h"
#include "RenderInterface.h"

namespace ParticleInstance
{
	struct Ice
	{
		float ColorIntencity = 0.33f;
	};

	struct Fire
	{
		float ColorIntencity = 0.33f;
	};

	struct Electric
	{
		float ColorIntencity = 0.33f;
		Vector3 Color{1.f,1.f,1.f};
	};
}


#endif //