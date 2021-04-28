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

void Effect::Reset()
{
	// �ڽ��� Effect::Reset()�� �ݵ�� ȣ��
	
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