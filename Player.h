#pragma once
#include "ModelActor.h"
#include "Model.h"
#include "Vector3.h"
#include "Camera.h"
#include <vector>

class Camera;

class Player : public ModelActor
{
private:
	static constexpr Vector3 SpawnPos = Vector3(0, 100, 400);	//開始時の座標
	static constexpr Vector3 Scale = Vector3(1.5f, 1.5f, 1.5f);	//自身のサイズ
	static constexpr Vector3 ColliderOffset = Vector3(0, 80, 0);	// コライダーのオフセット
	static constexpr float Speed = 7.5f * Scale.y;	//自身のサイズに合せた移動速度
	static constexpr float GravityScale = 0.8f;			// 重力
	static constexpr float DashSpeed = 1.4f;		//ダッシュ時の速度倍率
	static constexpr int AnimeAmount = 2;			//アニメーションの総数

	const char* AnimeFileName[AnimeAmount] =
	{
		"Man/Idle.mv1",		//待機
		"Man/Sprint.mv1"	//ダッシュ
	};

	Camera* m_camera;

	bool m_onGround;	// 地面についているかどうか
	bool m_onWall;		// 壁に当たっているかどうか
protected:
	virtual void Update() override;	//更新
	virtual void Draw() override;	//描画

public:
	//コンストラクタ
	Player(Camera* camera);

	//衝突イベント
	virtual void OnCollision(const ModelActor* other) override;
};