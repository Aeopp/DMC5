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
	if (!_IsPlaying)
		return;

	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void Effect::Reset()
{
	// 자식은 Effect::Reset()을 반드시 호출
	
	if (!_Loop)
	{
		RenderInterface::_RenderProperty.bRender = false;
		_IsPlaying = false;
	}

	_AccumulateTime = 0.f;
}

void Effect::SetScale(const float AllScale)
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		Sptransform->SetScale({ AllScale, AllScale, AllScale });
	}
}

void Effect::SetPosition(const Vector3& Pos)
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		Sptransform->SetPosition(Pos);
	}
}

void Effect::SetRotation(const Vector3& Rot)
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		Sptransform->SetRotation(Rot);
	}
}

void Effect::PlayStart(const float PlayingSpeed)
{
	if (_IsPlaying)
		Reset();

	RenderInterface::_RenderProperty.bRender = true;

	_IsPlaying = true; 
	_PlayingSpeed = PlayingSpeed;
}

UINT Effect::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	
	if (_IsPlaying)
		_AccumulateTime += _PlayingSpeed * _fDeltaTime;
	
	return 0;
}