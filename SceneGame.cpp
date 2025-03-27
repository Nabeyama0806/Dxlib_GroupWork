#include "SceneGame.h"
#include "SceneResult.h"
#include "Sprite.h"
#include "Screen.h"
#include "Input.h"
#include "Node.h"
#include "ModelActor.h"
#include "SpriteActor.h"
#include "Time.h"
#include "Fade.h"
#include "Camera.h"
#include "Player.h"
#include "ImageLoader.h"
#include "ModelLoader.h"
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
	//uiLayer->AddChild(new SpriteActor("ui", "Resource/title.png", Screen::Center));

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