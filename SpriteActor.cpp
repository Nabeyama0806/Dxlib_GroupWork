#include "SpriteActor.h"
#include "Sprite.h"

//コンストラクタ
SpriteActor::SpriteActor(const char* name, const char* textureName, const Vector3& position) :
	ActorBase(name, textureName, position),
	m_sprite(nullptr)
{
	m_name = name;
	m_transform.position = position;

	//画像が設定されていればスプライトを起動
	if (textureName)
	{
		m_sprite = new Sprite();
		m_sprite->Register(textureName);
	}
}

//リソースの読み込み
void SpriteActor::Load()
{
	if (m_sprite)
	{
		m_sprite->Load();
	}

}

//リソースの解放
void SpriteActor::Release()
{
	if (m_sprite)
	{
		delete m_sprite;
		m_sprite = nullptr;
	}
}

//描画
void SpriteActor::Draw()
{
	if (m_sprite)
	{
		m_sprite->Draw(m_transform);
	}
}

//子ノードを含む更新
void SpriteActor::TreeUpdate()
{
	//Node本来のTreeUpdate
	Node::TreeUpdate();

	//スプライトの更新
	if (m_sprite)
	{
		m_sprite->Update();
	}
}