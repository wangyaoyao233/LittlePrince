#include "main.h"
#include "Input.h"

//BYTE Input::m_OldKeyState[256];
//BYTE Input::m_KeyState[256];
//
//
//void Input::Init()
//{
//
//	memset( m_OldKeyState, 0, 256 );
//	memset( m_KeyState, 0, 256 );
//
//}
//
//void Input::Uninit()
//{
//
//
//}
//
//void Input::Update()
//{
//
//	memcpy( m_OldKeyState, m_KeyState, 256 );
//
//	GetKeyboardState( m_KeyState );
//
//}
//
//bool Input::GetKeyPress(BYTE KeyCode)
//{
//	return (m_KeyState[KeyCode] & 0x80);
//}
//
//bool Input::GetKeyTrigger(BYTE KeyCode)
//{
//	return ((m_KeyState[KeyCode] & 0x80) && !(m_OldKeyState[KeyCode] & 0x80));
//}

DirectX::Mouse::State Input::m_MouseState{};
DirectX::Mouse::State Input::m_LastMouseState{};
DirectX::Mouse::ButtonStateTracker Input::m_MouseTracker{};

DirectX::Keyboard::State Input::m_KBState{};
DirectX::Keyboard::State Input::m_LastKBState{};
DirectX::Keyboard::KeyboardStateTracker Input::m_KBTracker{};


void Input::Init()
{
	Mouse::Get().SetWindow(GetWindow());
	Mouse::Get().SetMode(Mouse::Mode::MODE_ABSOLUTE);	
}

void Input::PreUpdate()
{
	//1. 获取当前鼠标状态
	m_MouseState = Mouse::Get().GetState();
	//2. 使用当前状态更新
	m_MouseTracker.Update(m_MouseState);

	//1. 获取当前键盘状态
	m_KBState = Keyboard::Get().GetState();
	//2. 使用当前状态更新
	m_KBTracker.Update(m_KBState);

}

void Input::PostUpdate()
{
	//3. 获取上一帧鼠标时间, 应当在Update之前使用, 否则获取当前帧状态
	m_LastMouseState = m_MouseTracker.GetLastState();

	m_LastKBState = m_KBTracker.GetLastState();
}

// 0 - MODE_ABSOLUTE
// 1 - MODE_RELATIVE
void Input::SetMouseMode(int mode)
{
	if (mode == 0)
		Mouse::Get().SetMode(Mouse::Mode::MODE_ABSOLUTE);
	if (mode == 1)
		Mouse::Get().SetMode(Mouse::Mode::MODE_RELATIVE);
}

// 0 - LEFTBUTTON
// 1 - RIGHTBUTTON
// 2 - MIDBUTTON
bool Input::GetMouseButtonDown(int n)
{
	if (n == 0) {
		return m_MouseState.leftButton == true && m_MouseTracker.leftButton == m_MouseTracker.PRESSED;
	}
	else if (n == 1) {
		return m_MouseState.rightButton == true && m_MouseTracker.rightButton == m_MouseTracker.PRESSED;
	}
	else if (n == 2) {
		return m_MouseState.middleButton == true && m_MouseTracker.middleButton == m_MouseTracker.PRESSED;
	}
}

// 0 - LEFTBUTTON
// 1 - RIGHTBUTTON
// 2 - MIDBUTTON
bool Input::GetMouseButtonUp(int n)
{
	if (n == 0) {
		return m_MouseTracker.leftButton == m_MouseTracker.RELEASED;
	}
	else if (n == 1) {
		return m_MouseTracker.rightButton == m_MouseTracker.RELEASED;
	}
	else if (n == 2) {
		return m_MouseTracker.middleButton == m_MouseTracker.RELEASED;
	}
}

// 0 - LEFTBUTTON
// 1 - RIGHTBUTTON
// 2 - MIDBUTTON
bool Input::GetMouseButton(int n)
{
	if (n == 0) {
		return m_MouseState.leftButton == true && m_MouseTracker.leftButton == m_MouseTracker.HELD;
	}
	else if (n == 1) {
		return m_MouseState.rightButton == true && m_MouseTracker.rightButton == m_MouseTracker.HELD;
	}
	else if (n == 2) {
		return m_MouseState.middleButton == true && m_MouseTracker.middleButton == m_MouseTracker.HELD;
	}
}

bool Input::GetKeyDown(KeyCode key)
{
	return m_KBState.IsKeyDown(key) && m_KBTracker.IsKeyPressed(key);
}

bool Input::GetKeyUp(KeyCode key)
{
	return m_KBState.IsKeyUp(key);
}

bool Input::GetKey(KeyCode key)
{
	return m_KBState.IsKeyDown(key);
}

float Input::GetAxisRaw(std::wstring str)
{
	if (str == L"Horizontal") {
		if (Input::GetKey(KeyCode::A) || Input::GetKey(KeyCode::Left))
			return -1.0f;
		else if (Input::GetKey(KeyCode::D) || Input::GetKey(KeyCode::Right))
			return 1.0f;
	}
	else if (str == L"Vertical") {
		if (Input::GetKey(KeyCode::W) || Input::GetKey(KeyCode::Up))
			return -1.0f;
		else if (Input::GetKey(KeyCode::S) || Input::GetKey(KeyCode::Down))
			return 1.0f;
	}
	return 0.0f;
}

float Input::GetAxis(std::wstring str)
{
	return 0.0f;
}
