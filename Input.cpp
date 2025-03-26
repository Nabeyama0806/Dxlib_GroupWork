#include "Input.h"
#include "DxLib.h"
#include <cstring>

//更新
void Input::Update()
{
	DxLib::SetMouseDispFlag(FALSE);	//マウスカーソルを表示
	//キーボード

	//前フレームのキー入力状態を保存
	std::memcpy(m_keyStatePost, m_keyState, sizeof(char) * KeyStateNum);

	//現フレームのキー入力状態を取得
	GetHitKeyStateAll(m_keyState);

	//WASDが押されたらカーソルキーが入力されたことにする（プレイヤー騙し）
	/*if (m_keyState[KEY_INPUT_W]) m_keyState[KEY_INPUT_UP] = 1;
	if (m_keyState[KEY_INPUT_A]) m_keyState[KEY_INPUT_LEFT] = 1;
	if (m_keyState[KEY_INPUT_S]) m_keyState[KEY_INPUT_DOWN] = 1;
	if (m_keyState[KEY_INPUT_D]) m_keyState[KEY_INPUT_RIGHT] = 1;*/

	//マウス

	//前フレームのマウスボタン入力状態を保存
	m_mouseButtonPost = m_mouseButton;

	//現在のマウスボタン入力状態を取得
	m_mouseButton = DxLib::GetMouseInput();

	//マウス位置を取得
	int mouseX = 0;
	int mouseY = 0;
	DxLib::GetMousePoint(&mouseX, &mouseY);
	m_mousePoint = Vector2(mouseX, mouseY);

	//マウスホイールを取得
	m_mouseWheel = DxLib::GetMouseWheelRotVol();

	//マウスの左ボタンが押されたら、Zキーが入力されたことにする
	if (m_mouseButton & MOUSE_INPUT_LEFT) m_keyState[KEY_INPUT_Z] = 1;
}

//何かキーが押された瞬間
bool Input::IsAnyKeyDown()
{
	//マウス
	if (m_mouseButton && (m_mouseButton ^ m_mouseButtonPost))
	{
		return true;
	}

	//キーボード

	//配列の比較
	if (std::memcpy(m_keyStatePost, m_keyState, sizeof(char) * KeyStateNum) == 0)
	{
		return false;
	}

	//配列から１を探し出す
	return std::memchr(m_keyState, 1, sizeof(char) * KeyStateNum) != nullptr;
}

//何かキーが押されている
bool Input::IsAnyKeyPress()
{
	//マウス
	if (m_mouseButton)
	{
		return true;
	}

	//キーボード

	//配列から１を探し出す
	return std::memchr(m_keyState, 1, sizeof(char) * KeyStateNum) != nullptr;
}
