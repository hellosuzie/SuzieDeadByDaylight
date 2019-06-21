#include "KeyMgr.h"
#include<Windows.h>
#include"SSDCore.h"
#include"GameWindow.h"
#include"DXTemplate.h"
#include"DXMacro.h"

typedef CKeyMgr::CKey CKey;
typedef CKeyMgr::CPadKey CPadKey;



std::map<std::wstring, SPTR<CKey>>::iterator			CKeyMgr::m_KeyStartIter;
std::map<std::wstring, SPTR<CKey>>::iterator			CKeyMgr::m_KeyEndIter;
std::map<std::wstring, SPTR<CKey>>						CKeyMgr::m_mapKey;
std::unordered_map<std::wstring, std::wstring>			CKeyMgr::m_hashBindingKey;

SPTR<CKeyMgr::CKey>										CKeyMgr::m_pCurCreate = nullptr;
bool													CKeyMgr::m_bUpdate =true;

bool													CKeyMgr::m_bWheel = false;
int														CKeyMgr::m_Zeta = 0;

POINT													CKeyMgr::MousePos = POINT{};
POINT													CKeyMgr::PrevMousePos = POINT{};
Vec2													CKeyMgr::MouseDir = Vec2();
Vec2													CKeyMgr::MouseNormalDir = Vec2();

bool													CKeyMgr::m_bLStick = false;
bool													CKeyMgr::m_bRStick = false;
POINT													CKeyMgr::m_LStickValue = POINT{};
POINT													CKeyMgr::m_RStickValue = POINT{};


INPUT_MODE												CKeyMgr::eInputMode = WINDOW_INPUT;
IDirectInput8*											CKeyMgr::pDirectInput = nullptr;
IDirectInputDevice8*									CKeyMgr::pKeyboard = nullptr;
IDirectInputDevice8*									CKeyMgr::pMouse = nullptr;

unsigned char											CKeyMgr::KeyboardState[256] = {0,};
DIMOUSESTATE											CKeyMgr::MouseState;


bool													CKeyMgr::bMouseClip = false;

/////////////////////// Key

CKey::CKey()
	: m_fDoubleTime(0.2f)
	, m_fCurDoubleTime(0.0f)
	, m_bMouseKey(false)
{
	m_bUp = false;
	m_bPush = false;
	m_bKey = false;
}

CKey::~CKey()
{
}

bool CKey::KeyCheck() 
{
	for (size_t i = 0; i < m_VecKey.size(); i++)
	{
		if (0 == (GetAsyncKeyState(m_VecKey[i]) & 0x8000)) 
		{
			return false;
		}

		//if (0 == (KeyboardState[m_VecKey[i]] & 0x80))
		//{
		//	return false;
		//}
	}

	return true;
}
bool CKey::DirectKeyCheck()
{
	if (m_bMouseKey)
	{
		return MouseKeyCheck(m_VecKey[0]);
	}

	for (size_t i = 0; i < m_VecKey.size(); i++)
	{
		if (0 == (KeyboardState[m_VecKey[i]] & 0x80))
		{
			return false;
		}
	}

	return true;
}

bool CKey::MouseKeyCheck(const int& _MouseVirKey)
{

	switch (_MouseVirKey)
	{
	case VK_LBUTTON:
	{
		if (MouseState.rgbButtons[0] & 0x80)
			return true;
		else
			return false;
	}
		break;
	case VK_RBUTTON:
	{
		if (MouseState.rgbButtons[1] & 0x80)
			return true;
		else
			return false;
	}
	break;
	case VK_MBUTTON:
	{
		if (MouseState.rgbButtons[2] & 0x80)
			return true;
		else
			return false;
	}
	break;
	default:
		return false;
	}

	return false;
}

void CKey::Update(const float& _DeltaTime) {

	if (0 < m_fCurDoubleTime)
	{
		m_fCurDoubleTime -= _DeltaTime;
	}

	// 키가 모두 눌렸다.
	if (true == KeyCheck())
	{
		if (false == m_bKey)
		{
			m_bPush = true;

			// 여기서는 두가지 상황
			// 이미 체크하고 있는가?

			if (0 >= m_fCurDoubleTime)
			{
				m_fCurDoubleTime = m_fDoubleTime;
			} 
			else 
			{
				m_bDouble = true;
			}
		}
		else {
			m_bDouble = false;
			m_bPush = false;
		}

		m_bKey = true;
		m_bUp = false;
	}
	else 
	{
		if (true == m_bKey)
		{
			m_bUp = true;
		}
		else {
			m_bUp = false;
		}

		m_bPush = false;
		m_bKey = false;
	}

}

void CKey::DirectUpdate(const float& _DeltaTime)
{
	if (0 < m_fCurDoubleTime)
	{
		m_fCurDoubleTime -= _DeltaTime;
	}

	// 키가 모두 눌렸다.
	if (true == DirectKeyCheck())
	{
		if (false == m_bKey)
		{
			m_bPush = true;

			// 여기서는 두가지 상황
			// 이미 체크하고 있는가?

			if (0 >= m_fCurDoubleTime)
			{
				m_fCurDoubleTime = m_fDoubleTime;
			}
			else
			{
				m_bDouble = true;
			}
		}
		else {
			m_bDouble = false;
			m_bPush = false;
		}

		m_bKey = true;
		m_bUp = false;
	}
	else
	{
		if (true == m_bKey)
		{
			m_bUp = true;
		}
		else {
			m_bUp = false;
		}

		m_bPush = false;
		m_bKey = false;
	}


}
void CKey::InvalidateKey()
{
	m_bDouble = false;
	m_bKey = false;
	m_bPush = false;
	m_bUp = false;
	m_fCurDoubleTime = 0.0f;
}

///////////////////////////////////////////////////////			PadKey				//////////////////////////////
XINPUT_STATE					CPadKey::PadState;

void CPadKey::UpdatePadState()
{
	ZeroMemory(&PadState, sizeof(XINPUT_STATE));

	DWORD dwResult = XInputGetState(0, &PadState);

	if (dwResult != ERROR_SUCCESS)
		ZeroMemory(&PadState, sizeof(XINPUT_STATE));

	
}
CPadKey::CPadKey(const int& _PadNum)
	:CKey(), m_PadNum(_PadNum), m_bLTrigger(false), m_bRTrigger(false)
{
	if (_PadNum == PAD_LTRIGGER)
		m_bLTrigger = true;
	else if (_PadNum == PAD_RTRIGGER)
		m_bRTrigger = true;
}
CPadKey::~CPadKey() 
{

}


bool CPadKey::KeyCheck()
{
	if (true == m_bLTrigger) 
	{
		if (0 != PadState.Gamepad.bLeftTrigger)
			return true;
		else
			return false;
	}

	if (true == m_bRTrigger)
	{
		if (0 != PadState.Gamepad.bRightTrigger)
			return true;
		else
			return false;
	}

	if (0 != (m_PadNum & PadState.Gamepad.wButtons) )
		return true;

	return false;
}



///////////////////////////////////////////////////////		 CInputMgr				//////////////////////////////

CKeyMgr::CKeyMgr() 
{
}

CKeyMgr::~CKeyMgr()
{
	m_mapKey.clear();
}

void CKeyMgr::Init(const HINSTANCE& _hInst, const HWND& _hWnd, const INPUT_MODE _Mode /*= DIRECT_INPUT*/)
{
	eInputMode = _Mode;

	if (eInputMode == DIRECT_INPUT) 
	{
		InitInputDevice(_hInst, _hWnd);
		CreateDirectKey();
	}
	else if (eInputMode == WINDOW_INPUT) 
	{
		CreateWindowKey();
	}


	//CKeyMgr::CreateKey(L"Tab", VK_TAB);
	//CKeyMgr::CreateKey(L"Ctrl", VK_CONTROL);
	//CKeyMgr::CreateKey(L"LShift", VK_LSHIFT);
	//CKeyMgr::CreateKey(L"RShift", VK_RSHIFT);

	CKeyMgr::CreatePadKey(L"PAD_LTrigger", CPadKey::PAD_LTRIGGER);
	CKeyMgr::CreatePadKey(L"PAD_RTrigger", CPadKey::PAD_RTRIGGER);

	CKeyMgr::CreatePadKey(L"PAD_A", 4096);
	CKeyMgr::CreatePadKey(L"PAD_B", 8192);
	CKeyMgr::CreatePadKey(L"PAD_X", 16384);
	CKeyMgr::CreatePadKey(L"PAD_Y", 32768);

}

const bool CKeyMgr::InitInputDevice(const HINSTANCE& _hInst, const HWND& _hWnd)
{
	HRESULT Check = DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDirectInput, NULL);
	
	if (FAILED(Check))
	{ 
		assert(nullptr);
		return false;
	}

	Check = pDirectInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL); 
	
	if (FAILED(Check))
	{
		assert(nullptr);
		return false;
	} 

	Check = pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	
	if(FAILED(Check))
	{
		assert(nullptr);
		return false;
	}
	Check = pDirectInput->CreateDevice(GUID_SysMouse, &pMouse, NULL);

	if (FAILED(Check))
	{
		assert(nullptr);
		return false;
	}

	Check = pMouse->SetDataFormat(&c_dfDIMouse);

	if (FAILED(Check))
	{
		assert(nullptr);
		return false;
	}

	Check = pKeyboard->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	
	if (FAILED(Check))
	{
		assert(nullptr);
		return false;
	}

	Check = pMouse->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	
	if (FAILED(Check))
	{
		assert(nullptr);
		return false;
	}

	Check = pKeyboard->Acquire();
	Check = pMouse->Acquire();
	
	//if (FAILED(Check))
	//{
	//	assert(nullptr);
	//	return false;
	//}

	
	//if (FAILED(Check))
	//{
	//	assert(nullptr);
	//	return false;
	//}

	return true;
}

void CKeyMgr::CreateWindowKey() 
{
	CKeyMgr::CreateKey(L"Escape", VK_ESCAPE);

	CKeyMgr::CreateKey(L"Tab", VK_TAB);
	CKeyMgr::CreateKey(L"Ctrl", VK_CONTROL);
	CKeyMgr::CreateKey(L"LShift", VK_LSHIFT);
	CKeyMgr::CreateKey(L"RShift", VK_RSHIFT);

	CKeyMgr::CreateKey(L"LMouse", VK_LBUTTON);
	CKeyMgr::CreateKey(L"RMouse", VK_RBUTTON);

	CKeyMgr::CreateKey(L"VK_UP", VK_UP);
	CKeyMgr::CreateKey(L"VK_DOWN", VK_DOWN);
	CKeyMgr::CreateKey(L"VK_LEFT", VK_LEFT);
	CKeyMgr::CreateKey(L"VK_RIGHT", VK_RIGHT);

	CKeyMgr::CreateKey(L"Q", 'Q');
	CKeyMgr::CreateKey(L"W", 'W');
	CKeyMgr::CreateKey(L"E", 'E');
	CKeyMgr::CreateKey(L"R", 'R');
	CKeyMgr::CreateKey(L"T", 'T');
	CKeyMgr::CreateKey(L"Y", 'Y');
	CKeyMgr::CreateKey(L"U", 'U');
	CKeyMgr::CreateKey(L"I", 'I');
	CKeyMgr::CreateKey(L"O", 'O');
	CKeyMgr::CreateKey(L"P", 'P');
	CKeyMgr::CreateKey(L"A", 'A');
	CKeyMgr::CreateKey(L"S", 'S');
	CKeyMgr::CreateKey(L"D", 'D');
	CKeyMgr::CreateKey(L"F", 'F');
	CKeyMgr::CreateKey(L"G", 'G');
	CKeyMgr::CreateKey(L"H", 'H');
	CKeyMgr::CreateKey(L"J", 'J');
	CKeyMgr::CreateKey(L"K", 'K');
	CKeyMgr::CreateKey(L"L", 'L');
	CKeyMgr::CreateKey(L"Z", 'Z');
	CKeyMgr::CreateKey(L"X", 'X');
	CKeyMgr::CreateKey(L"C", 'C');
	CKeyMgr::CreateKey(L"V", 'V');
	CKeyMgr::CreateKey(L"B", 'B');
	CKeyMgr::CreateKey(L"N", 'N');
	CKeyMgr::CreateKey(L"M", 'M');

	CKeyMgr::CreateKey(L"CtrlQ", VK_CONTROL, 'Q');
	CKeyMgr::CreateKey(L"CtrlW", VK_CONTROL, 'W');
	CKeyMgr::CreateKey(L"CtrlE", VK_CONTROL, 'E');
	CKeyMgr::CreateKey(L"CtrlR", VK_CONTROL, 'R');
	CKeyMgr::CreateKey(L"CtrlT", VK_CONTROL, 'T');
	CKeyMgr::CreateKey(L"CtrlY", VK_CONTROL, 'Y');
	CKeyMgr::CreateKey(L"CtrlU", VK_CONTROL, 'U');
	CKeyMgr::CreateKey(L"CtrlI", VK_CONTROL, 'I');
	CKeyMgr::CreateKey(L"CtrlO", VK_CONTROL, 'O');
	CKeyMgr::CreateKey(L"CtrlP", VK_CONTROL, 'P');
	CKeyMgr::CreateKey(L"CtrlA", VK_CONTROL, 'A');
	CKeyMgr::CreateKey(L"CtrlS", VK_CONTROL, 'S');
	CKeyMgr::CreateKey(L"CtrlD", VK_CONTROL, 'D');
	CKeyMgr::CreateKey(L"CtrlF", VK_CONTROL, 'F');
	CKeyMgr::CreateKey(L"CtrlG", VK_CONTROL, 'G');
	CKeyMgr::CreateKey(L"CtrlH", VK_CONTROL, 'H');
	CKeyMgr::CreateKey(L"CtrlJ", VK_CONTROL, 'J');
	CKeyMgr::CreateKey(L"CtrlK", VK_CONTROL, 'K');
	CKeyMgr::CreateKey(L"CtrlL", VK_CONTROL, 'L');
	CKeyMgr::CreateKey(L"CtrlZ", VK_CONTROL, 'Z');
	CKeyMgr::CreateKey(L"CtrlX", VK_CONTROL, 'X');
	CKeyMgr::CreateKey(L"CtrlC", VK_CONTROL, 'C');
	CKeyMgr::CreateKey(L"CtrlV", VK_CONTROL, 'V');
	CKeyMgr::CreateKey(L"CtrlB", VK_CONTROL, 'B');
	CKeyMgr::CreateKey(L"CtrlN", VK_CONTROL, 'N');
	CKeyMgr::CreateKey(L"CtrlM", VK_CONTROL, 'M');


	CKeyMgr::CreateKey(L"ShiftQ", VK_SHIFT, 'Q');
	CKeyMgr::CreateKey(L"ShiftW", VK_SHIFT, 'W');
	CKeyMgr::CreateKey(L"ShiftE", VK_SHIFT, 'E');
	CKeyMgr::CreateKey(L"ShiftR", VK_SHIFT, 'R');
	CKeyMgr::CreateKey(L"ShiftT", VK_SHIFT, 'T');
	CKeyMgr::CreateKey(L"ShiftY", VK_SHIFT, 'Y');
	CKeyMgr::CreateKey(L"ShiftU", VK_SHIFT, 'U');
	CKeyMgr::CreateKey(L"ShiftI", VK_SHIFT, 'I');
	CKeyMgr::CreateKey(L"ShiftO", VK_SHIFT, 'O');
	CKeyMgr::CreateKey(L"ShiftP", VK_SHIFT, 'P');
	CKeyMgr::CreateKey(L"ShiftA", VK_SHIFT, 'A');
	CKeyMgr::CreateKey(L"ShiftS", VK_SHIFT, 'S');
	CKeyMgr::CreateKey(L"ShiftD", VK_SHIFT, 'D');
	CKeyMgr::CreateKey(L"ShiftF", VK_SHIFT, 'F');
	CKeyMgr::CreateKey(L"ShiftG", VK_SHIFT, 'G');
	CKeyMgr::CreateKey(L"ShiftH", VK_SHIFT, 'H');
	CKeyMgr::CreateKey(L"ShiftJ", VK_SHIFT, 'J');
	CKeyMgr::CreateKey(L"ShiftK", VK_SHIFT, 'K');
	CKeyMgr::CreateKey(L"ShiftL", VK_SHIFT, 'L');
	CKeyMgr::CreateKey(L"ShiftZ", VK_SHIFT, 'Z');
	CKeyMgr::CreateKey(L"ShiftX", VK_SHIFT, 'X');
	CKeyMgr::CreateKey(L"ShiftC", VK_SHIFT, 'C');
	CKeyMgr::CreateKey(L"ShiftV", VK_SHIFT, 'V');
	CKeyMgr::CreateKey(L"ShiftB", VK_SHIFT, 'B');
	CKeyMgr::CreateKey(L"ShiftN", VK_SHIFT, 'N');
	CKeyMgr::CreateKey(L"ShiftM", VK_SHIFT, 'M');


	CKeyMgr::CreateKey(L",", ',');
	CKeyMgr::CreateKey(L".", '.');
	CKeyMgr::CreateKey(L"`", '`');

	CKeyMgr::CreateKey(L"1", '1');
	CKeyMgr::CreateKey(L"2", '2');
	CKeyMgr::CreateKey(L"3", '3');
	CKeyMgr::CreateKey(L"4", '4');
	CKeyMgr::CreateKey(L"5", '5');
	CKeyMgr::CreateKey(L"6", '6');
	CKeyMgr::CreateKey(L"7", '7');
	CKeyMgr::CreateKey(L"8", '8');
	CKeyMgr::CreateKey(L"9", '9');
	CKeyMgr::CreateKey(L"0", '0');

	CKeyMgr::CreateKey(L"F1", VK_F1);
	CKeyMgr::CreateKey(L"F2", VK_F2);
	CKeyMgr::CreateKey(L"F3", VK_F3);
	CKeyMgr::CreateKey(L"F4", VK_F4);
	CKeyMgr::CreateKey(L"F5", VK_F5);
	CKeyMgr::CreateKey(L"F6", VK_F6);
	CKeyMgr::CreateKey(L"F7", VK_F7);
	CKeyMgr::CreateKey(L"F8", VK_F8);
	CKeyMgr::CreateKey(L"F9", VK_F9);
	CKeyMgr::CreateKey(L"F10", VK_F10);
	CKeyMgr::CreateKey(L"F11", VK_F11);
	CKeyMgr::CreateKey(L"F12", VK_F12);

	CKeyMgr::CreateKey(L"SPACE", VK_SPACE);
	CKeyMgr::CreateKey(L"INSERT", VK_INSERT);
	CKeyMgr::CreateKey(L"HOME", VK_HOME);
	CKeyMgr::CreateKey(L"PAGE_UP", VK_PRIOR);
	CKeyMgr::CreateKey(L"DELETE" , VK_DELETE);
	CKeyMgr::CreateKey(L"END", VK_END);
	CKeyMgr::CreateKey(L"PAGE_DOWN", VK_NEXT);
	
	CKeyMgr::BindingKey(L"GizmoHold", L"LMouse");
	CKeyMgr::BindingKey(L"GizmoMode", L"CtrlG");

	CKeyMgr::BindingKey(L"TerrainDebugMode", L"CtrlT");

	CKeyMgr::BindingKey(L"DebugKey", L"CtrlD");

	CKeyMgr::BindingKey(L"LB", L"LMouse");
	CKeyMgr::BindingKey(L"RB", L"RMouse");
}
void CKeyMgr::CreateDirectKey() 
{
	CKeyMgr::CreateKey(L"Escape", DIK_ESCAPE);
	CKeyMgr::CreateKey(L"Enter", DIK_RETURN);
	CKeyMgr::CreateKey(L"Tab", DIK_TAB);
	CKeyMgr::CreateKey(L"LControl", DIK_LCONTROL);
	CKeyMgr::CreateKey(L"RControl", DIK_RCONTROL);
	CKeyMgr::CreateKey(L"LShift", DIK_LSHIFT);
	CKeyMgr::CreateKey(L"RShift", DIK_RSHIFT);

	CKeyMgr::CreateKey(L"LMouse", VK_LBUTTON)->SetMouseKey(true);
	CKeyMgr::CreateKey(L"RMouse", VK_RBUTTON)->SetMouseKey(true);

	CKeyMgr::CreateKey(L"VK_UP", DIK_UP);
	CKeyMgr::CreateKey(L"VK_DOWN", DIK_DOWN);
	CKeyMgr::CreateKey(L"VK_LEFT", DIK_LEFT);
	CKeyMgr::CreateKey(L"VK_RIGHT", DIK_RIGHT);

	CKeyMgr::CreateKey(L"Q", DIK_Q);
	CKeyMgr::CreateKey(L"W", DIK_W);
	CKeyMgr::CreateKey(L"E", DIK_E);
	CKeyMgr::CreateKey(L"R", DIK_R);
	CKeyMgr::CreateKey(L"T", DIK_T);
	CKeyMgr::CreateKey(L"Y", DIK_Y);
	CKeyMgr::CreateKey(L"U", DIK_U);
	CKeyMgr::CreateKey(L"I", DIK_I);
	CKeyMgr::CreateKey(L"O", DIK_O);
	CKeyMgr::CreateKey(L"P", DIK_P);
	CKeyMgr::CreateKey(L"A", DIK_A);
	CKeyMgr::CreateKey(L"S", DIK_S);
	CKeyMgr::CreateKey(L"D", DIK_D);
	CKeyMgr::CreateKey(L"F", DIK_F);
	CKeyMgr::CreateKey(L"G", DIK_G);
	CKeyMgr::CreateKey(L"H", DIK_H);
	CKeyMgr::CreateKey(L"J", DIK_J);
	CKeyMgr::CreateKey(L"K", DIK_K);
	CKeyMgr::CreateKey(L"L", DIK_L);
	CKeyMgr::CreateKey(L"Z", DIK_Z);
	CKeyMgr::CreateKey(L"X", DIK_X);
	CKeyMgr::CreateKey(L"C", DIK_C);
	CKeyMgr::CreateKey(L"V", DIK_V);
	CKeyMgr::CreateKey(L"B", DIK_B);
	CKeyMgr::CreateKey(L"N", DIK_N);
	CKeyMgr::CreateKey(L"M", DIK_M);

	CKeyMgr::CreateKey(L"CtrlQ", DIK_LCONTROL, DIK_Q);
	CKeyMgr::CreateKey(L"CtrlW", DIK_LCONTROL, DIK_W);
	CKeyMgr::CreateKey(L"CtrlE", DIK_LCONTROL, DIK_E);
	CKeyMgr::CreateKey(L"CtrlR", DIK_LCONTROL, DIK_R);
	CKeyMgr::CreateKey(L"CtrlT", DIK_LCONTROL, DIK_T);
	CKeyMgr::CreateKey(L"CtrlY", DIK_LCONTROL, DIK_Y);
	CKeyMgr::CreateKey(L"CtrlU", DIK_LCONTROL, DIK_U);
	CKeyMgr::CreateKey(L"CtrlI", DIK_LCONTROL, DIK_I);
	CKeyMgr::CreateKey(L"CtrlO", DIK_LCONTROL, DIK_O);
	CKeyMgr::CreateKey(L"CtrlP", DIK_LCONTROL, DIK_P);
	CKeyMgr::CreateKey(L"CtrlA", DIK_LCONTROL, DIK_A);
	CKeyMgr::CreateKey(L"CtrlS", DIK_LCONTROL, DIK_S);
	CKeyMgr::CreateKey(L"CtrlD", DIK_LCONTROL, DIK_D);
	CKeyMgr::CreateKey(L"CtrlF", DIK_LCONTROL, DIK_F);
	CKeyMgr::CreateKey(L"CtrlG", DIK_LCONTROL, DIK_G);
	CKeyMgr::CreateKey(L"CtrlH", DIK_LCONTROL, DIK_H);
	CKeyMgr::CreateKey(L"CtrlJ", DIK_LCONTROL, DIK_J);
	CKeyMgr::CreateKey(L"CtrlK", DIK_LCONTROL, DIK_K);
	CKeyMgr::CreateKey(L"CtrlL", DIK_LCONTROL, DIK_L);
	CKeyMgr::CreateKey(L"CtrlZ", DIK_LCONTROL, DIK_Z);
	CKeyMgr::CreateKey(L"CtrlX", DIK_LCONTROL, DIK_X);
	CKeyMgr::CreateKey(L"CtrlC", DIK_LCONTROL, DIK_C);
	CKeyMgr::CreateKey(L"CtrlV", DIK_LCONTROL, DIK_V);
	CKeyMgr::CreateKey(L"CtrlB", DIK_LCONTROL, DIK_B);
	CKeyMgr::CreateKey(L"CtrlN", DIK_LCONTROL, DIK_N);
	CKeyMgr::CreateKey(L"CtrlM", DIK_LCONTROL, DIK_M);

	CKeyMgr::CreateKey(L"ShiftQ", DIK_LSHIFT, DIK_Q);
	CKeyMgr::CreateKey(L"ShiftW", DIK_LSHIFT, DIK_W);
	CKeyMgr::CreateKey(L"ShiftE", DIK_LSHIFT, DIK_E);
	CKeyMgr::CreateKey(L"ShiftR", DIK_LSHIFT, DIK_R);
	CKeyMgr::CreateKey(L"ShiftT", DIK_LSHIFT, DIK_T);
	CKeyMgr::CreateKey(L"ShiftY", DIK_LSHIFT, DIK_Y);
	CKeyMgr::CreateKey(L"ShiftU", DIK_LSHIFT, DIK_U);
	CKeyMgr::CreateKey(L"ShiftI", DIK_LSHIFT, DIK_I);
	CKeyMgr::CreateKey(L"ShiftO", DIK_LSHIFT, DIK_O);
	CKeyMgr::CreateKey(L"ShiftP", DIK_LSHIFT, DIK_P);
	CKeyMgr::CreateKey(L"ShiftA", DIK_LSHIFT, DIK_A);
	CKeyMgr::CreateKey(L"ShiftS", DIK_LSHIFT, DIK_S);
	CKeyMgr::CreateKey(L"ShiftD", DIK_LSHIFT, DIK_D);
	CKeyMgr::CreateKey(L"ShiftF", DIK_LSHIFT, DIK_F);
	CKeyMgr::CreateKey(L"ShiftG", DIK_LSHIFT, DIK_G);
	CKeyMgr::CreateKey(L"ShiftH", DIK_LSHIFT, DIK_H);
	CKeyMgr::CreateKey(L"ShiftJ", DIK_LSHIFT, DIK_J);
	CKeyMgr::CreateKey(L"ShiftK", DIK_LSHIFT, DIK_K);
	CKeyMgr::CreateKey(L"ShiftL", DIK_LSHIFT, DIK_L);
	CKeyMgr::CreateKey(L"ShiftZ", DIK_LSHIFT, DIK_Z);
	CKeyMgr::CreateKey(L"ShiftX", DIK_LSHIFT, DIK_X);
	CKeyMgr::CreateKey(L"ShiftC", DIK_LSHIFT, DIK_C);
	CKeyMgr::CreateKey(L"ShiftV", DIK_LSHIFT, DIK_V);
	CKeyMgr::CreateKey(L"ShiftB", DIK_LSHIFT, DIK_B);
	CKeyMgr::CreateKey(L"ShiftN", DIK_LSHIFT, DIK_N);
	CKeyMgr::CreateKey(L"ShiftM", DIK_LSHIFT, DIK_M);


	CKeyMgr::CreateKey(L",", DIK_COMMA);
	CKeyMgr::CreateKey(L".", DIK_PERIOD);
	CKeyMgr::CreateKey(L"`", DIK_APOSTROPHE);

	CKeyMgr::CreateKey(L"1", DIK_1);
	CKeyMgr::CreateKey(L"2", DIK_2);
	CKeyMgr::CreateKey(L"3", DIK_3);
	CKeyMgr::CreateKey(L"4", DIK_4);
	CKeyMgr::CreateKey(L"5", DIK_5);
	CKeyMgr::CreateKey(L"6", DIK_6);
	CKeyMgr::CreateKey(L"7", DIK_7);
	CKeyMgr::CreateKey(L"8", DIK_8);
	CKeyMgr::CreateKey(L"9", DIK_9);
	CKeyMgr::CreateKey(L"0", DIK_0);

	CKeyMgr::CreateKey(L"F1", DIK_F1);
	CKeyMgr::CreateKey(L"F2", DIK_F2);
	CKeyMgr::CreateKey(L"F3", DIK_F3);
	CKeyMgr::CreateKey(L"F4", DIK_F4);
	CKeyMgr::CreateKey(L"F5", DIK_F5);
	CKeyMgr::CreateKey(L"F6", DIK_F6);
	CKeyMgr::CreateKey(L"F7", DIK_F7);
	CKeyMgr::CreateKey(L"F8", DIK_F8);
	CKeyMgr::CreateKey(L"F9", DIK_F9);
	CKeyMgr::CreateKey(L"F10", DIK_F10);
	CKeyMgr::CreateKey(L"F11", DIK_F11);
	CKeyMgr::CreateKey(L"F12", DIK_F12);

	CKeyMgr::CreateKey(L"SPACE", DIK_SPACE);
	CKeyMgr::CreateKey(L"INSERT", DIK_INSERT);
	CKeyMgr::CreateKey(L"HOME", DIK_HOME);
	CKeyMgr::CreateKey(L"PAGE_UP", DIK_PRIOR);
	CKeyMgr::CreateKey(L"DELETE", DIK_PRIOR);
	CKeyMgr::CreateKey(L"END", DIK_END);
	CKeyMgr::CreateKey(L"PAGE_DOWN", DIK_NEXT);


	CKeyMgr::BindingKey(L"GizmoHold", L"LMouse");
	CKeyMgr::BindingKey(L"GizmoMode", L"CtrlG");
	CKeyMgr::BindingKey(L"TerrainDebugMode", L"CtrlT");

	CKeyMgr::BindingKey(L"DebugKey", L"CtrlD");

	CKeyMgr::BindingKey(L"LB", L"LMouse");
	CKeyMgr::BindingKey(L"RB", L"RMouse");
}
void CKeyMgr::Release()
{
	ShutdownInputDevice();
}
void CKeyMgr::ShutdownInputDevice()
{

	if (nullptr != pMouse)
	{
		pMouse->Unacquire();
		pMouse->Release();
		pMouse = nullptr;
	}


	if (nullptr != pKeyboard)
	{ 
		pKeyboard->Unacquire();
		pKeyboard->Release();
		pKeyboard = nullptr;
	}

	if(nullptr != pDirectInput)
	{
		pDirectInput->Release();
		pDirectInput = nullptr;
	} 
	
	return;

}
SPTR<CKey> CKeyMgr::FindKey(const std::wstring& _Key)
{
	return MAPFINDSPTR<SPTR<CKey>>(m_mapKey,_Key);
}
std::wstring CKeyMgr::FindKeyName(const std::wstring& _BindKey)
{
	return MAPFINDVALUE<std::wstring>(m_hashBindingKey, _BindKey);
}
void CKeyMgr::CreatePadKey(const wchar_t* _Key, const int& _PadNum)
{
	m_mapKey.insert(std::map<std::wstring, SPTR<CKey>>::value_type(_Key, new CPadKey(_PadNum)));
}

void CKeyMgr::InitMosuePos()
{
	PrevMousePos = CGameWindow::SetMousePoint(Vec2(0.f, 0.f));
	MouseDir = Vec2();
}
void CKeyMgr::Update(const float& _DeltaTime)
{
	CPadKey::UpdatePadState();
	
	POINT MPos;
	MPos = CGameWindow::GetMousePoint();

	if (PrevMousePos.x != MPos.x || PrevMousePos.y != MPos.y)
	{
		PrevMousePos = MousePos;
		MouseDir = Vec2( (float)(MPos.x - PrevMousePos.x), (float)(MPos.y - PrevMousePos.y));
		MouseNormalDir = MouseDir.GetNormalizeVec();

		if (true == bMouseClip) 
		{
			if (false == Core::CheckMouseOnClampScreen()) 
			{
				MPos = PrevMousePos;
				CGameWindow::SetMousePoint(MPos);
			}
	
			/*		
			MPos.x = DXMath::Clamp(MPos.x, (LONG)0, Core::GetClientWidth() - 1);
			MPos.y = DXMath::Clamp(MPos.y, (LONG)0, Core::GetClientHeight() - 1);
			CGameWindow::SetMousePoint(MPos);
			*/
		}

		MousePos = MPos;
	}

	LoopSetting();
	
	switch (eInputMode)
	{
	case WINDOW_INPUT:
		WindowKeyUpdate(_DeltaTime);
		break;
	case DIRECT_INPUT:
		DirectKeyUpdate(_DeltaTime);
		break;
	default:
		break;
	}

	UpdatePadStick();
	return;
}

void CKeyMgr::WindowKeyUpdate(const float& _DeltaTime)
{
	if (true == m_bUpdate)
	{
		for (; m_KeyStartIter != m_KeyEndIter; m_KeyStartIter++)
		{
			m_KeyStartIter->second->Update(_DeltaTime);
		}
	}
	else
	{
		for (; m_KeyStartIter != m_KeyEndIter; m_KeyStartIter++)
		{
			m_KeyStartIter->second->InvalidateKey();
		}
	}
}
void CKeyMgr::DirectKeyUpdate(const float& _DeltaTime) 
{
	ReadKeyboardState();

	if (true == m_bUpdate)
	{
		for (; m_KeyStartIter != m_KeyEndIter; m_KeyStartIter++)
		{
			m_KeyStartIter->second->DirectUpdate(_DeltaTime);
		}
	}
	else
	{
		for (; m_KeyStartIter != m_KeyEndIter; m_KeyStartIter++)
		{
			m_KeyStartIter->second->InvalidateKey();
		}
	}
}
void CKeyMgr::ReadKeyboardState()
{
	HRESULT Check;
	Check = pKeyboard->GetDeviceState(sizeof(KeyboardState), (LPVOID)&KeyboardState);
	
	if (FAILED(Check))
	{
		// If the keyboard lost focus or was not acquired then try to get control back. 
		if ((Check == DIERR_INPUTLOST) || (Check == DIERR_NOTACQUIRED))
		{
			pKeyboard->Acquire();
		}
		else
		{
			return ;
		}
	}

	Check = pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&MouseState);

	if (FAILED(Check))
	{
		// If the keyboard lost focus or was not acquired then try to get control back. 
		if ((Check == DIERR_INPUTLOST) || (Check == DIERR_NOTACQUIRED))
		{
			pMouse->Acquire();
		}
		else
		{
			return;
		}
	}
}
void CKeyMgr::UpdatePadStick()
{
	float LX = (float)CPadKey::PadState.Gamepad.sThumbLX;
	float LY = (float)CPadKey::PadState.Gamepad.sThumbLY;
	float RX = (float)CPadKey::PadState.Gamepad.sThumbRX;
	float RY = (float)CPadKey::PadState.Gamepad.sThumbRY;

	if (sqrtf(LX*LX + LY * LY) <= (float)LSTICK_DEADZONE) 
	{
		m_bLStick = false;
		m_LStickValue = POINT{ 0,0 };
	}
	else 
	{
		m_bLStick = true;
		m_LStickValue = POINT{ (LONG)LX ,(LONG)LY };
	}

	if (sqrtf(RX * RX + RY * RY) <= (float)RSTICK_DEADZONE) 
	{
		m_bRStick = false;
		m_RStickValue = POINT{ 0,0 };
	}
	else 
	{
		m_bRStick = true;
		m_RStickValue = POINT{ (LONG)RX ,(LONG)RY };
	}

}
bool CKeyMgr::GetKey(const std::wstring& _Key)
{
	SPTR<CKey> pKey = FindKey(FindKeyName(_Key));

	if (nullptr == pKey)
	{
		return false;
	}

	return pKey->GetKey();
}

bool CKeyMgr::GetPush(const std::wstring& _Key)
{
	
	SPTR<CKey> pKey = FindKey(FindKeyName(_Key));

	if (nullptr == pKey)
	{
		return false;
	}

	if (false == Core::CheckMouseOnScreen())
		return false;

	return pKey->GetPush();
}

bool CKeyMgr::GetUp(const std::wstring& _Key)
{
	SPTR<CKey> pKey = FindKey(FindKeyName(_Key));

	if (nullptr == pKey)
	{
		return false;
	}

	return pKey->GetUp();
}

bool CKeyMgr::GetDouble(const std::wstring& _Key)
{
	SPTR<CKey> pKey = FindKey(FindKeyName(_Key));

	if (nullptr == pKey)
	{
		return false;
	}

	return pKey->GetDouble();
}
 bool CKeyMgr::GetKeyOrPush(const std::wstring& _Key)
{
	 SPTR<CKey> pKey = FindKey(FindKeyName(_Key));

	 if (nullptr == pKey)
	 {
		 return false;
	 }

	 return pKey->GetKey() || pKey->GetPush();
}
 void CKeyMgr::ResetWheelValue()
 {
	 m_bWheel = false;
	 m_Zeta = 0;
 }
 void CKeyMgr::MouseWheelEvent(const int _Zeta) 
 {
	 m_bWheel = true;
	 m_Zeta = _Zeta;
 }
 bool CKeyMgr::GetWheelEvent() 
 {
	 return m_bWheel;
 }
 int CKeyMgr::GetWheelSign() 
 {
	 return m_Zeta;
 }

 bool CKeyMgr::IsUpdate() 
 {
	 return m_bUpdate; 
 }
 void CKeyMgr::UpdateOn() 
 {
	 m_bUpdate = true; 
 }
 void CKeyMgr::UpdateOff() 
 {
	 m_bUpdate = false;
 }
const Vec2 CKeyMgr::GetLStick() 
{
	Vec2 LStick = Vec2((float)m_LStickValue.x, (float)m_LStickValue.y);
	return LStick.GetNormalizeVec();
}
const Vec2 CKeyMgr::GetRStick()
{
	Vec2 RStick = Vec2((float)m_RStickValue.x, (float)m_RStickValue.y);
	return RStick.GetNormalizeVec();
}

const Vec2 CKeyMgr::GetMouseDir()
{
	return MouseDir;
}
const Vec2 CKeyMgr::GetMouseNormalDir()
{
	return MouseNormalDir;
}
void CKeyMgr::SetMouseClip(const bool _bClip)
{
	bMouseClip = _bClip;
	
}
const bool CKeyMgr::ChangeMouseClipMode()
{
	bMouseClip = !bMouseClip;

	ShowCursor(!bMouseClip);
	return bMouseClip;
}

void CKeyMgr::SetMouseMiddlePos()
{
	PrevMousePos =  CGameWindow::SetMousePoint(Vec2(0.f, 0.f));
	MouseDir = Vec2(0.f, 0.f);
}

 void CKeyMgr::BindingKey(const std::wstring& _BindKey, const std::wstring& _Key)
 {
	 std::unordered_map<std::wstring, std::wstring>::iterator Find = m_hashBindingKey.find(_Key);

	 if (Find == m_hashBindingKey.end())
		m_hashBindingKey.insert(std::unordered_map<std::wstring, std::wstring>::value_type(_BindKey, _Key));
	

 }