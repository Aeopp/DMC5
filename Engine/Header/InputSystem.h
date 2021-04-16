#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__
#include "Object.h"
BEGIN(ENGINE)
#define	MAX_KEY 256
class InputSystem final : public Object
{
	DECLARE_SINGLETON(InputSystem)
private:
	LPDIRECTINPUT8			m_pInputSDK;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

	BYTE					m_byKeyState[MAX_KEY];
	BYTE					m_byPrevKeyState[MAX_KEY];

	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			m_tPrevMouseState;
private:
	explicit InputSystem();
	virtual ~InputSystem() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	HRESULT	ReadyInputSystem();
	HRESULT UpdateInputSystem();
private:
	HRESULT CreateDevice();
	HRESULT UpdateKey();
	HRESULT	UpdateMouse();
public:
	bool	GetKey(const UINT _nKeyCode);
	bool	GetKeyDown(const UINT _nKeyCode);
	bool	GetKeyUp(const UINT _nKeyCode);

	LONG	GetMouseMove(const MOUSEAXIS _eAxis);

	bool	GetMouse(const MOUSEBUTTON _eMouseButton);
	bool	GetMouseDown(const MOUSEBUTTON _eMouseButton);
	bool	GetMouseUp(const MOUSEBUTTON _eMouseButton);
};
END
#endif // !__INPUT_SYSTEM_H__
