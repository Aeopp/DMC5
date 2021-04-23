#ifndef __INPUT_H__
#define __INPUT_H__
#define	MAX_KEY 256
class Input
{
public:
	static Input Instance;
private:
	LPDIRECTINPUT8			m_pInputSDK;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

	BYTE					m_byKeyState[MAX_KEY];
	BYTE					m_byPrevKeyState[MAX_KEY];

	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			m_tPrevMouseState;
public:
	Input();
	~Input();
public:
	static HRESULT	ReadyInput();
	static HRESULT UpdateInput();
public:
	static HRESULT UpdateKey();
	static HRESULT	UpdateMouse();
public:
	static bool	GetKey(const UINT _nKeyCode);
	static bool	GetKeyDown(const UINT _nKeyCode);
	static bool	GetKeyUp(const UINT _nKeyCode);
	 
	static LONG	GetMouseMove(const MOUSEAXIS _eAxis);
	 
	static bool	GetMouse(const MOUSEBUTTON _eMouseButton);
	static bool	GetMouseDown(const MOUSEBUTTON _eMouseButton);
	static bool	GetMouseUp(const MOUSEBUTTON _eMouseButton);
};

#endif // !__INPUT_H__
