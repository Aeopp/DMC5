#include "framework.h"
#include "Input.h"

Input Input::Instance;

Input::Input()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
	, m_byKeyState{ 0, }
	, m_byPrevKeyState{ 0, }
{
	ZeroMemory(&m_tMouseState, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_tPrevMouseState, sizeof(DIMOUSESTATE));
}

Input::~Input()
{
	SafeRelease(m_pKeyBoard);
	SafeRelease(m_pMouse);
	SafeRelease(m_pInputSDK);
}

HRESULT Input::ReadyInput()
{
	if (FAILED(DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&Instance.m_pInputSDK, NULL)))
	{
		return E_FAIL;
	}

	/****************************************Ű���� ��ü ����************************************************/
	if (FAILED(Instance.m_pInputSDK->CreateDevice(GUID_SysKeyboard, &Instance.m_pKeyBoard, nullptr)))
	{
		return E_FAIL;
	}

	//������ Ű���� ��ü ������ COM ��ü�� ����.
	if (FAILED(Instance.m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//��ġ�� ���� �������� ������ �ִ� �Լ�(Ŭ���̾�Ʈ ���¿� ���� Ű �Է��� ������ ������ ����.)
	if (FAILED(Instance.m_pKeyBoard->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		return E_FAIL;
	}

	//��ġ�� ���� ���� ���� ����.
	if (FAILED(Instance.m_pKeyBoard->Acquire()))
	{
		return E_FAIL;
	}

	/****************************************���콺 ��ü ����************************************************/
	if (FAILED(Instance.m_pInputSDK->CreateDevice(GUID_SysMouse, &Instance.m_pMouse, nullptr)))
	{
		return E_FAIL;
	}

	//������ Ű���� ��ü ������ COM ��ü�� ����.
	if (FAILED(Instance.m_pMouse->SetDataFormat(&c_dfDIMouse)))
	{
		return E_FAIL;
	}

	//��ġ�� ���� �������� ������ �ִ� �Լ�(Ŭ���̾�Ʈ ���¿� ���� Ű �Է��� ������ ������ ����.)
	if (FAILED(Instance.m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		return E_FAIL;
	}

	//��ġ�� ���� ���� ���� ����.
	if (FAILED(Instance.m_pMouse->Acquire()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Input::UpdateInput()
{
	if (FAILED(UpdateKey()))
		return E_FAIL;
	if (FAILED(UpdateMouse()))
		return E_FAIL;
	return S_OK;
}

HRESULT Input::UpdateKey()
{
	if (nullptr == Instance.m_pKeyBoard)
		return E_FAIL;

	//���� Ű ���� ����.
	memcpy_s(Instance.m_byPrevKeyState, sizeof(BYTE) * 256, Instance.m_byKeyState, sizeof(BYTE) * 256);
	//���� Ű ���� �޾ƿ�.
	HRESULT hResult = Instance.m_pKeyBoard->GetDeviceState(256, (LPVOID)Instance.m_byKeyState);
	//���� Ű ���¸� �޾ƿ��µ� ������ ���.
	if (FAILED(hResult))
	{
		//Ű���尡 ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� ��Ʈ�� �ٽ� ������.
		if ((DIERR_INPUTLOST == hResult) || (DIERR_NOTACQUIRED == hResult))
		{
			if (FAILED(Instance.m_pKeyBoard->Acquire()))
				return E_FAIL;
		}
		else
			return E_FAIL;
	}
	return S_OK;
}

HRESULT Input::UpdateMouse()
{
	if (nullptr == Instance.m_pMouse)
		return E_FAIL;
	//���� ���콺 ���� ����.
	memcpy_s(&Instance.m_tPrevMouseState, sizeof(DIMOUSESTATE), &Instance.m_tMouseState, sizeof(DIMOUSESTATE));
	//���콺 ���� �޾ƿ�.
	HRESULT hResult = Instance.m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&Instance.m_tMouseState);
	//���� ���콺 ���¸� �޾ƿ��µ� ������ ���.
	if (FAILED(hResult))
	{
		//���콺�� ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� ��Ʈ�� �ٽ� ������.
		if ((DIERR_INPUTLOST == hResult) || (DIERR_NOTACQUIRED == hResult))
		{
			if (FAILED(Instance.m_pMouse->Acquire()))
				return E_FAIL;
		}
		else
			return E_FAIL;
	}
	return S_OK;
}

bool Input::GetKey(const UINT _nKeyCode)
{
	if ((Instance.m_byKeyState[_nKeyCode] & 0x80))
		return true;
	return false;
}

bool Input::GetKeyDown(const UINT _nKeyCode)
{
	if (!(Instance.m_byPrevKeyState[_nKeyCode] & 0x80) && (Instance.m_byKeyState[_nKeyCode] & 0x80))
		return true;
	return false;
}

bool Input::GetKeyUp(const UINT _nKeyCode)
{
	if ((Instance.m_byPrevKeyState[_nKeyCode] & 0x80) && !(Instance.m_byKeyState[_nKeyCode] & 0x80))
		return true;
	return false;
}

LONG Input::GetMouseMove(const MOUSEAXIS _eAxis)
{
	return *(((LONG*)(&Instance.m_tMouseState)) + _eAxis);
}

bool Input::GetMouse(const MOUSEBUTTON _eMouseButton)
{
	if ((Instance.m_tMouseState.rgbButtons[_eMouseButton] & 0x80))
		return true;
	return false;
}

bool Input::GetMouseDown(const MOUSEBUTTON _eMouseButton)
{
	if (!(Instance.m_tPrevMouseState.rgbButtons[_eMouseButton] & 0x80) && (Instance.m_tMouseState.rgbButtons[_eMouseButton] & 0x80))
		return true;
	return false;
}

bool Input::GetMouseUp(const MOUSEBUTTON _eMouseButton)
{
	if ((Instance.m_tPrevMouseState.rgbButtons[_eMouseButton] & 0x80) && !(Instance.m_tMouseState.rgbButtons[_eMouseButton] & 0x80))
		return true;
	return false;
}