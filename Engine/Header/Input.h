#ifndef __INPUT_H__
#define __INPUT_H__
#include "EngineStdafx.h"
BEGIN(ENGINE)
class InputSystem;
class ENGINE_DLL Input abstract
{
private:
	static std::weak_ptr<InputSystem>	m_pInputSystem;
public:
	static bool	GetKey(const UINT _nKeyCode);
	static bool	GetKeyDown(const UINT _nKeyCode);
	static bool	GetKeyUp(const UINT _nKeyCode);

	static LONG	GetMouseMove(const MOUSEAXIS _eAxis);

	static bool	GetMouse(const MOUSEBUTTON _eMouseButton);
	static bool	GetMouseDown(const MOUSEBUTTON _eMouseButton);
	static bool	GetMouseUp(const MOUSEBUTTON _eMouseButton);
};
END
#endif // !__INPUT_H__
