#pragma once
#include "Node.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Transform.h"
#include "Actor.h"

class Camera : public Node
{
private:
	static constexpr Vector3 SpawnPos = Vector3(0, 170, -400);
	static constexpr float LookAtHeight = 10;			//注視点からの高さのオフセット
	static constexpr float m_cameraDistance = 900;		//カメラ座標までの距離
	static constexpr float m_cameraAngleSpeed = 1.5f;	//カメラの回転速度

	Transform* m_transform;		//姿勢情報
	Actor* m_lookAt;			//注視するオブジェクト
	Vector3 m_targetPos;		//注視点座標
	Vector3 m_cameraPos;		//カメラ座標
	Vector3 m_cameraAngle;		//カメラ角度
	
	float m_cameraHAngle;		//横回転
	float m_cameraVAngle;		//縦回転

protected:
	virtual void Update();	//更新
	virtual void Draw();	//描画

public:
	//コンストラクタ
	Camera() :
		m_transform(nullptr),
		m_lookAt(nullptr),
		m_cameraHAngle(0),
		m_cameraVAngle(21)
	{
		m_transform = new Transform();
	}

	//注視点の取得
	void SetlookAt(Actor* actor)
	{
		m_lookAt = actor;
	}

	float GetHAngle()
	{
		return m_cameraHAngle;
	}

	float GetVAngle()
	{
		return m_cameraVAngle;
	}

	Vector3 GetForward() const
	{
		return (m_targetPos - m_cameraPos).Normalized();
	}

	Vector3 GetRight() const
	{
		return Vector3::Cross(Vector3(0, 1, 0), GetForward()).Normalized();
	}
};