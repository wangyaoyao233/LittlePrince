#pragma once
#include "Mouse.h"
#include "Keyboard.h"

using KeyCode = DirectX::Keyboard::Keys;

class Input
{
public:
	static void Init();
	static void PreUpdate();
	static void PostUpdate();
	static void SetMouseMode(int mode);

	static DirectX::XMFLOAT3 GetMousePosition();
	static DirectX::XMFLOAT2 GetMouseScrollDelta();

	static bool GetMouseButtonDown(int n);
	static bool GetMouseButtonUp(int n);
	static bool GetMouseButton(int n);

	static bool GetKeyDown(KeyCode key);
	static bool GetKeyUp(KeyCode key);
	static bool GetKey(KeyCode key);

	static float GetAxisRaw(std::wstring str);
	static float GetAxis(std::wstring str);

private:
	static DirectX::Mouse::State m_MouseState;
	static DirectX::Mouse::State m_LastMouseState;
	static DirectX::Mouse::ButtonStateTracker m_MouseTracker;

	static DirectX::Keyboard::State m_KBState;
	static DirectX::Keyboard::State m_LastKBState;
	static DirectX::Keyboard::KeyboardStateTracker m_KBTracker;

	static DirectX::Mouse m_Mouse;
};