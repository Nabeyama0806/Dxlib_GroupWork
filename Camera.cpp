#include "Camera.h"
#include "Input.h"
#include "Math.h"

//更新
void Camera::Update()
{
	//方向キーでカメラの操作
	if (CheckHitKey(KEY_INPUT_RIGHT) == 1)
	{
		m_cameraHAngle += m_cameraAngleSpeed;
		if (m_cameraHAngle >= 180.0f) m_cameraHAngle -= 360.0f;
	}
	if (CheckHitKey(KEY_INPUT_LEFT) == 1)
	{
		m_cameraHAngle -= m_cameraAngleSpeed;
		if (m_cameraHAngle <= -180.0f) m_cameraHAngle += 360.0f;
	}
	if (CheckHitKey(KEY_INPUT_UP) == 1)
	{
		m_cameraVAngle += m_cameraAngleSpeed;
		if (m_cameraVAngle >= 80.0f) m_cameraVAngle = 80.0f;
	}
	if (CheckHitKey(KEY_INPUT_DOWN) == 1)
	{
		m_cameraVAngle -= m_cameraAngleSpeed;
		if (m_cameraVAngle <= 0.0f) m_cameraVAngle = 0.0f;
	}

	Vector3 TempPosition1;
	Vector3 TempPosition2;
	Vector3 CameraLookAtPosition;						//注視点
	CameraLookAtPosition = m_lookAt->GetPosition();		//注視点を設定
	CameraLookAtPosition.y += LookAtHeight;				//注視点のオフセット

	//最初に垂直角度を反映した位置を算出
	float sinParam = sin(Math::DegToRad(m_cameraVAngle));
	float cosParam = cos(Math::DegToRad(m_cameraVAngle));
	TempPosition1.x = 0.0f;
	TempPosition1.y = sinParam * m_cameraDistance;
	TempPosition1.z = -cosParam * m_cameraDistance;

	//次に水平角度を反映した位置を算出
	sinParam = sin(Math::DegToRad(m_cameraHAngle));
	cosParam = cos(Math::DegToRad(m_cameraHAngle));
	TempPosition2.x = cosParam * TempPosition1.x - sinParam * TempPosition1.z;
	TempPosition2.y = TempPosition1.y;
	TempPosition2.z = sinParam * TempPosition1.x + cosParam * TempPosition1.z;

	//算出した座標に注視点の位置を加算したものがカメラの位置
	m_cameraPos = TempPosition2 + CameraLookAtPosition;
	m_targetPos = CameraLookAtPosition;
}

//描画
void Camera::Draw()
{
	// カメラ座標の設定
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_targetPos);
}