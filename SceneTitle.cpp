#include "SceneTitle.h"
#include"SceneGame.h"
#include "DxLib.h"
#include "Input.h"
#include "Screen.h"

//初期化
void SceneTitle::Initialize()
{
	m_rootNode = new Node();

	//背景
}

//終了
void SceneTitle::Finalize()
{
	m_rootNode->TreeRelease();
	delete m_rootNode;
	m_rootNode = nullptr;
}

//更新
SceneBase* SceneTitle::Update()
{
	//いずれかのキーが押されたらゲームシーンへ移動
	if (Input::GetInstance()->IsAnyKeyDown())
	{
		return new SceneGame();
	}

	//ノードの更新
	m_rootNode->TreeUpdate();

	return this;
}

//描画
void SceneTitle::Draw()
{
	//ノードの描画
	m_rootNode->TreeDraw();

}