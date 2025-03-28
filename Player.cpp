#include "Player.h"
#include "Input.h"
#include "Time.h"
#include "Quaternion.h"
#include "BoxCollider.h"
#include "ModelLoader.h"
#include "ModelAnimation.h"

//コンストラクタ
Player::Player(Camera* camera) :
	ModelActor("Player"),
	m_camera(camera)
{
	//アニメーションの登録
	m_model = new Model("Man/Man.mv1");
	for (int i = 0; i < AnimeAmount; ++i)
	{
		//アニメーションのファイルパスを渡す
		m_model->Register(AnimeFileName[i]);
	}

	//姿勢情報の調整
	m_transform.position = SpawnPos;
	m_transform.scale = Scale;

	/*
	Vector3 colliderScale = Vector3(100, 170, 100) * Scale.x;
	//衝突判定
	m_collider = new BoxCollider(colliderScale, Vector3(0,80,0) * Scale.x);
	*/
	m_collider = new BoxCollider(m_transform.scale);
}

//更新
void Player::Update()
{
	//本来の更新
	ModelActor::Update();

	//入力方向の取得
	Vector3 move = Vector3(0, 0, 0);
	float speedRate = 1.0f;
	if (Input::GetInstance()->IsKeyPress(KEY_INPUT_LSHIFT))	speedRate = DashSpeed;
	if (Input::GetInstance()->IsKeyPress(KEY_INPUT_W)) move.z =  1;
	if (Input::GetInstance()->IsKeyPress(KEY_INPUT_S)) move.z = -1;
	if (Input::GetInstance()->IsKeyPress(KEY_INPUT_D)) move.x =  1;
	if (Input::GetInstance()->IsKeyPress(KEY_INPUT_A)) move.x = -1;

	//カメラの正面ベクトルを作成
	Vector3 cameraForward = Vector3::Scale(m_camera->GetForward(), Vector3(1, 0, 1)).Normalized();

	//カメラの向きを考慮した移動量
	move = cameraForward * move.z + m_camera->GetRight() * move.x;

	//待機アニメーションを設定
	int animeIndex = static_cast<int>(Model::Anime::Idle);

	//動いている時
	if (!move.IsZero())
	{
		//移動
		move.Normalize();
		m_transform.position += move * Speed * speedRate;

		//回転
		m_transform.rotation = Quaternion::Slerp(
			m_transform.rotation,
			Quaternion::LookRotation(move),
			0.2f);

		//移動アニメーションを設定
		animeIndex = static_cast<int>(Model::Anime::Run);
	}
	
	//設定したアニメーションの再生
	m_model->PlayAnime(animeIndex);
}

//描画
void Player::Draw()
{
	ModelActor::Draw();
}

//衝突イベント
void Player::OnCollision(const ModelActor* other)
{
	//壁
	if (other->GetName() == "Wall")
	{
		m_transform.position = SpawnPos;
	}
}