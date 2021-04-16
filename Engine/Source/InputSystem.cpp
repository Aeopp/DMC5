#include "..\Header\InputSystem.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(InputSystem)

InputSystem::InputSystem()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
	, m_byKeyState{ 0, }
	, m_byPrevKeyState{ 0, }
{
	ZeroMemory(&m_tMouseState, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_tPrevMouseState, sizeof(DIMOUSESTATE));
}

void InputSystem::Free()
{
	SafeRelease(m_pKeyBoard);
	SafeRelease(m_pMouse);
	SafeRelease(m_pInputSDK);
}

HRESULT InputSystem::ReadyInputSystem()
{
	if (FAILED(CreateDevice()))
		return E_FAIL;
	return S_OK;
}

HRESULT InputSystem::UpdateInputSystem()
{
	if (FAILED(UpdateKey()))
		return E_FAIL;
	if (FAILED(UpdateMouse()))
		return E_FAIL;
	return S_OK;
}

HRESULT InputSystem::CreateDevice()
{
	if (FAILED(DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to DirectInput8Create."));
		return E_FAIL;
	}

	/****************************************키보드 객체 생성************************************************/
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to Create KeyBoard."));
		return E_FAIL;
	}

	//생성된 키보드 객체 정보를 COM 객체에 전달.
	if (FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to SetDataFormat(KeyBoard)."));
		return E_FAIL;
	}

	//장치에 대한 독점권을 설정해 주는 함수(클라이언트 상태에 따라 키 입력을 받을지 말지를 결정.)
	if (FAILED(m_pKeyBoard->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to SetCooperativeLevel(KeyBoard)."));
		return E_FAIL;
	}

	//장치에 대한 접근 권한 얻어옴.
	if (FAILED(m_pKeyBoard->Acquire()))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to Acquire(KeyBoard)"));
		return E_FAIL;
	}

	/****************************************마우스 객체 생성************************************************/
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to Create Mouse."));
		return E_FAIL;
	}

	//생성된 키보드 객체 정보를 COM 객체에 전달.
	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to SetDataFormat(Mouse)."));
		return E_FAIL;
	}

	//장치에 대한 독점권을 설정해 주는 함수(클라이언트 상태에 따라 키 입력을 받을지 말지를 결정.)
	if (FAILED(m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to SetCooperativeLevel(Mouse)."));
		return E_FAIL;
	}

	//장치에 대한 접근 권한 얻어옴.
	if (FAILED(m_pMouse->Acquire()))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to Acquire(Mouse)"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT InputSystem::UpdateKey()
{
	if (nullptr == m_pKeyBoard)
		return E_FAIL;

	//이전 키 상태 저장.
	memcpy_s(m_byPrevKeyState, sizeof(BYTE) * 256, m_byKeyState, sizeof(BYTE) * 256);
	//현재 키 상태 받아옴.
	HRESULT hResult = m_pKeyBoard->GetDeviceState(256, (LPVOID)m_byKeyState);
	//현재 키 상태를 받아오는데 실패한 경우.
	if (FAILED(hResult))
	{
		//키보드가 포커스를 잃었거나 획득되지 않은 경우 컨트롤 다시 가져옴.
		if ((DIERR_INPUTLOST == hResult) || (DIERR_NOTACQUIRED == hResult))
		{
			if (FAILED(m_pKeyBoard->Acquire()))
				return E_FAIL;
		}
		else
			return E_FAIL;
	}
	return S_OK;
}

HRESULT InputSystem::UpdateMouse()
{
	if (nullptr == m_pMouse)
		return E_FAIL;
	//이전 마우스 상태 저장.
	memcpy_s(&m_tPrevMouseState, sizeof(DIMOUSESTATE), &m_tMouseState, sizeof(DIMOUSESTATE));
	//마우스 상태 받아옴.
	HRESULT hResult = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_tMouseState);
	//현재 마우스 상태를 받아오는데 실패한 경우.
	if (FAILED(hResult))
	{
		//마우스가 포커스를 잃었거나 획득되지 않은 경우 컨트롤 다시 가져옴.
		if ((DIERR_INPUTLOST == hResult) || (DIERR_NOTACQUIRED == hResult))
		{
			if (FAILED(m_pMouse->Acquire()))
				return E_FAIL;
		}
		else
			return E_FAIL;
	}
	return S_OK;
}

bool InputSystem::GetKey(const UINT _nKeyCode)
{
	if ((m_byKeyState[_nKeyCode] & 0x80))
		return true;
	return false;
}

bool InputSystem::GetKeyDown(const UINT _nKeyCode)
{
	if (!(m_byPrevKeyState[_nKeyCode] & 0x80) && (m_byKeyState[_nKeyCode] & 0x80))
		return true;
	return false;
}

bool InputSystem::GetKeyUp(const UINT _nKeyCode)
{
	if ((m_byPrevKeyState[_nKeyCode] & 0x80) && !(m_byKeyState[_nKeyCode] & 0x80))
		return true;
	return false;
}

LONG InputSystem::GetMouseMove(const MOUSEAXIS _eAxis)
{
	return *(((LONG*)(&m_tMouseState)) + _eAxis);
}

bool InputSystem::GetMouse(const MOUSEBUTTON _eMouseButton)
{
	if ((m_tMouseState.rgbButtons[_eMouseButton] & 0x80))
		return true;
	return false;
}

bool InputSystem::GetMouseDown(const MOUSEBUTTON _eMouseButton)
{
	if (!(m_tPrevMouseState.rgbButtons[_eMouseButton] & 0x80) && (m_tMouseState.rgbButtons[_eMouseButton] & 0x80))
		return true;
	return false;
}

bool InputSystem::GetMouseUp(const MOUSEBUTTON _eMouseButton)
{
	if ((m_tPrevMouseState.rgbButtons[_eMouseButton] & 0x80) && !(m_tMouseState.rgbButtons[_eMouseButton] & 0x80))
		return true;
	return false;
}
