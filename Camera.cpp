#include "Camera.h"
#include "Input.h"
#include "Math.h"
#include "Screen.h"

//更新
void Camera::Update()
{
	// カメラの視点移動
	if (!Input::GetInstance()->IsKeyPress(KEY_INPUT_RSHIFT)) MouseCamera();

	Vector3 tempPosition1;
	Vector3 tempPosition2;
	if (m_lookAt != nullptr)
	{
		m_targetPos = m_lookAt->GetPosition();		//注視点を設定
		m_targetPos.y += LookAtHeight;				//注視点のオフセット
	}
	else
	{
		m_targetPos.y = LookAtHeight;
	}

	//最初に垂直角度を反映した位置を算出
	float sinParam = static_cast<float>(sin(Math::DegToRad(m_cameraVAngle)));
	float cosParam = static_cast<float>(cos(Math::DegToRad(m_cameraVAngle)));
	tempPosition1.x = 0.0f;
	tempPosition1.y = sinParam * CameraDistance;
	tempPosition1.z = -cosParam * CameraDistance;

	//次に水平角度を反映した位置を算出
	sinParam = static_cast<float>(sin(Math::DegToRad(m_cameraHAngle)));
	cosParam = static_cast<float>(cos(Math::DegToRad(m_cameraHAngle)));
	tempPosition2.x = cosParam * tempPosition1.x - sinParam * tempPosition1.z;
	tempPosition2.y = tempPosition1.y;
	tempPosition2.z = sinParam * tempPosition1.x + cosParam * tempPosition1.z;

	//算出した座標に注視点の位置を加算したものがカメラの位置
	m_cameraPos = tempPosition2 + m_targetPos;
}

//描画
void Camera::Draw()
{
	// カメラ座標の設定
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_targetPos);
}

void Camera::MouseCamera()
{
	//マウスでカメラの操作
	Input::GetInstance()->SetMouseDispFlag(false);
	Vector2 mousePos = Input::GetInstance()->GetMousePoint();
	m_cameraHAngle -= (mousePos.x - Screen::Center.x) * CameraAngleSpeed;
	m_cameraVAngle += (mousePos.y - Screen::Center.y) * CameraAngleSpeed;
	Input::GetInstance()->SetMousePoint(static_cast<int>(Screen::Center.x), static_cast<int>(Screen::Center.y));
	if (m_cameraVAngle >= CameraMaxVAngle) m_cameraVAngle = CameraMaxVAngle;
	if (m_cameraVAngle <= CameraMinVAngle) m_cameraVAngle = CameraMinVAngle;
}