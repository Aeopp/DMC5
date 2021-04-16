#include "stdafx.h"
#include "..\Header\Effect.h"
//#include "Transform.h"
//#include "Subset.h"
//#include "TextureType.h"
//#include "Renderer.h"


void Effect::Free()
{

}

std::string Effect::GetName()
{
	return "Effect";
}


void Effect::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
	}
}