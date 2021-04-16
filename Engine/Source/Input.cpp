#include "..\Header\Input.h"
#include "InputSystem.h"

USING(ENGINE)

std::weak_ptr<InputSystem> Input::m_pInputSystem = InputSystem::GetInstance();

bool Input::GetKey(const UINT _nKeyCode)
{
	return m_pInputSystem.lock()->GetKey(_nKeyCode);
}

bool Input::GetKeyDown(const UINT _nKeyCode)
{
	return m_pInputSystem.lock()->GetKeyDown(_nKeyCode);
}

bool Input::GetKeyUp(const UINT _nKeyCode)
{
	return m_pInputSystem.lock()->GetKeyUp(_nKeyCode);
}

LONG Input::GetMouseMove(const MOUSEAXIS _eAxis)
{
	return m_pInputSystem.lock()->GetMouseMove(_eAxis);
}

bool Input::GetMouse(const MOUSEBUTTON _eMouseButton)
{
	return m_pInputSystem.lock()->GetMouse(_eMouseButton);
}

bool Input::GetMouseDown(const MOUSEBUTTON _eMouseButton)
{
	return m_pInputSystem.lock()->GetMouseDown(_eMouseButton);
}

bool Input::GetMouseUp(const MOUSEBUTTON _eMouseButton)
{
	return m_pInputSystem.lock()->GetMouseUp(_eMouseButton);
}
