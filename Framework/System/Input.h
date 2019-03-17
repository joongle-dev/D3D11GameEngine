#pragma once
#include "Subsystem.h"

#define MAX_INPUT 256

enum KeyCode : UINT
{
	Num0 = 0x30, Num1 = 0x31, Num2 = 0x32, Num3 = 0x33, Num4 = 0x34, 
	Num5 = 0x35, Num6 = 0x36, Num7 = 0x37, Num8 = 0x38, Num9 = 0x39,
	A = 0x41, B = 0x42, C = 0x43, D = 0x44, E = 0x45, F = 0x46,
	G = 0x47, H = 0x48, I = 0x49, J = 0x4A, K = 0x4B, L = 0x4C,
	M = 0x4D, N = 0x4E, O = 0x4F, P = 0x50, Q = 0x51, R = 0x52,
	S = 0x53, T = 0x54, U = 0x55, V = 0x56, W = 0x57, X = 0x58,
	Y = 0x59, Z = 0x5A,
};

class Input final : public Subsystem<Input>
{
public:
	Input(class Context* context);
	~Input();

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	void Update();

	const bool KeyDown(const UINT& key) const { return m_newKeyState[key]; }
	const bool KeyUp(const UINT& key) const { return !m_newKeyState[key]; }
	const bool KeyPress(const UINT& key) const { return m_newKeyState[key] && !m_oldKeyState[key]; }
	const bool KeyRelease(const UINT& key) const { return !m_newKeyState[key] && m_oldKeyState[key]; }

	const Vector2& GetMousePosition() const { return m_newMousePosition; }
	const Vector2& GetMouseMove() const { return m_delMousePosition; }

private:
	byte m_oldKeyState[MAX_INPUT];
	byte m_newKeyState[MAX_INPUT];

	Vector2 m_oldMousePosition;
	Vector2 m_newMousePosition;
	Vector2 m_delMousePosition;
};