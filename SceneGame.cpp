#include "SceneGame.h"
#include "SceneResult.h"
#include "ModelLoader.h"
#include "ImageLoader.h"
#include "Screen.h"
#include "Fade.h"
#include "Time.h"
#include "Input.h"
#include "Node.h"
#include "Sprite.h"
#include "ModelActor.h"
#include "SpriteActor.h"
#include "HitBox.h"
#include "Camera.h"
#include "Player.h"
#include "DxLib.h"

//初期化
void SceneGame::Initialize()
{
	//画像の事前読み込み
	for (auto image : ImagePreload)
	{
		ModelLoader::GetInstance()->Load(image);
	}

	//ルート
	m_rootNode = new Node();

	//メインカメラ
	m_mainCamera = new Camera();
	m_rootNode->AddChild(m_mainCamera);

	//背景
	m_rootNode->AddChild(new ModelActor("Ground", "Environment/Environment.mv1"));

	//アクターレイヤー
	Node* actorLayer = new Node();
	m_rootNode->AddChild(actorLayer);

	//UIレイヤー
	Node* uiLayer = new Node();
	m_rootNode->AddChild(uiLayer);

	//プレイヤー
	m_player = new Player(m_mainCamera);
	actorLayer->AddChild(m_player);
	m_mainCamera->SetlookAt(m_player);

	// Ui(仮置き)
	m_tmpUi = new SpriteActor("ui", "Resource/ポーション候補3.png", Screen::BottomLeft);
	m_tmpUi->ChangeScale(0.1f);
	uiLayer->AddChild(m_tmpUi);

	//壁
	Vector3 pos	 = Vector3(0, 100, 1500);
	Vector3 size = Vector3(300, 300, 300);
	actorLayer->AddChild(new HitBox(
		"Wall",
		pos,
		size
	));

	//魔法瓶の表示
	ModelActor* model = new ModelActor("kaminari", "Resource/kaminariポーション.mv1", pos);
	model->ChangeScale(70);
	actorLayer->AddChild(model);

	//BGM

}

//終了
void SceneGame::Finalize()
{
	//事前読み込みした画像の破棄
	for (auto image : ImagePreload)
	{
		ModelLoader::GetInstance()->Delete(image);
	}

	//ノードの削除
	m_rootNode->TreeRelease();
	delete m_rootNode;
	m_rootNode = nullptr;

	//BGM
	DeleteSoundMem(m_bgm);
}

//更新
SceneBase* SceneGame::Update()
{
	//ノードの更新
	m_rootNode->TreeUpdate();
	return this;
}

//描画
void SceneGame::Draw()
{
	//ノードの描画
	m_rootNode->TreeDraw();
}