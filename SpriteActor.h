#pragma once
#include "ActorBase.h"
#include "Transform.h"
#include <string>

class Sprite;

class SpriteActor : public ActorBase
{
protected: 
	Sprite* m_sprite;		//2D画像

	virtual void Load() override;	//リソースの読み込み
	virtual void Release() override;	//リソースの開放
	virtual void Draw() override;	//リソースの開放

public:
	//コンストラクタ
	SpriteActor(const char* name,
		const char* textureName = nullptr,
		const Vector3& position = Vector2()
	);

	//子ノードを含む更新
	virtual void TreeUpdate() override;
};