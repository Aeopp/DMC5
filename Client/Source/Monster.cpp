#include "stdafx.h"
#include "Monster.h"

#include <iostream>


void Monster::Free()
{
	Unit::Free();
}

std::string Monster::GetName()
{
	return "Monster";
}


void Monster::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void Monster::Set_Snatch(bool _bSnatch)
{
	m_bSnatch = _bSnatch;
}
