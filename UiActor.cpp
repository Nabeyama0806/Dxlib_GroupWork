#include "UiActor.h"

//コンストラクタ
UiActor::UiActor(const char* name, const char* textureName, const Vector2& position) :
	m_name(name),
	m_sprite(nullptr),
	m_collider(nullptr)
{
	m_uiTransform.position = position;

	//画像が設定されていればスプライトを起動
	if (textureName)
	{
		m_sprite = new Sprite();
		m_sprite->Register(textureName);
	}
}
//リソースの読み込み
void UiActor::Load()
{
	if (m_sprite)
	{
		m_sprite->Load();
	}

}

//リソースの解放
void UiActor::Release()
{
	if (m_sprite)
	{
		delete m_sprite;
		m_sprite = nullptr;
	}
}

//描画
void UiActor::Draw()
{
	if (m_sprite)
	{
		m_sprite->Draw(m_uiTransform);
	}
}

//子ノードを含む更新
void UiActor::TreeUpdate()
{
	//Node本来のTreeUpdate
	Node::TreeUpdate();

	//スプライトの更新
	if (m_sprite)
	{
		m_sprite->Update();
	}
}