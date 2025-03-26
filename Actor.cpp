#include "Actor.h"
#include "Model.h"
#include "Animation.h"

//コンストラクタ
Actor::Actor(const char* name, const char* modelFileName, const Vector3& position) :
	m_name(name),
	m_model(nullptr)
{
	//ファイルパスが指定されていればロードする
	if (modelFileName)
	{
		m_model = new Model(modelFileName);
	}
}

//リソースの読み込み
void Actor::Load()
{
	//モデル
	if (m_model)
	{
		m_model->Load();
	}

	//衝突判定の追加
}

//リソースの解放
void Actor::Release()
{
	//モデル
	if (m_model)
	{
		delete m_model;
		m_model = nullptr;
	}
	
	//衝突判定システムから除外する
}

//描画
void Actor::Draw()
{
	//モデル
	if (m_model)
	{
		m_model->Draw(m_transform);
	}

}

//子ノードを含む更新
void Actor::TreeUpdate()
{
	//本来のUpdate
	Node::TreeUpdate();
	
	//モデル
	if (m_model)
	{
		m_model->Update();
	}
	
}