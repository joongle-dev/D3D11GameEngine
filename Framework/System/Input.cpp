#include "Framework.h"
#include "Input.h"


Input::Input(Context* context) :
	Subsystem<Input>(context), 
	m_oldMousePosition(0, 0), 
	m_newMousePosition(0, 0),
	m_delMousePosition(0, 0)
{
	ZeroMemory(m_oldKeyState, sizeof(byte) * MAX_INPUT);
	ZeroMemory(m_newKeyState, sizeof(byte) * MAX_INPUT);
}

Input::~Input()
{
}

void Input::Update()
{
	memcpy(m_oldKeyState, m_newKeyState, MAX_INPUT);
	GetKeyboardState(m_newKeyState);
	for (auto& key : m_newKeyState)
		key = key & 0x80 ? 1 : 0;

	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(m_context->GetSubsystem<Window>()->GetHandle(), &cursor);

	m_oldMousePosition = m_newMousePosition;
	m_newMousePosition = { static_cast<float>(cursor.x), static_cast<float>(cursor.y) };
	m_delMousePosition = { m_newMousePosition.x - m_oldMousePosition.x, m_newMousePosition.y - m_oldMousePosition.y };
}
