#pragma once
#include "Node.h"
#include "UiTransform.h"
#include "Sprite.h"
#include <string>

class Collider;
class Camera;

class UiActor : public Node
{
protected: 
	std::string m_name;		//オブジェクト名
	UiTransform m_uiTransform;	//姿勢情報
	Sprite* m_sprite;		//2D画像
	Collider* m_collider;	//衝突判定の形状

	virtual void Load() override;	//リソースの読み込み
	virtual void Release() override;	//リソースの開放
	virtual void Draw() override;	//リソースの開放

public:
	//コンストラクタ
	UiActor(const char* name,
		const char* textureName = nullptr,
		const Vector2& position = Vector2()
	);

	//子ノードを含む更新
	virtual void TreeUpdate() override;

	//姿勢の取得
	const UiTransform& GetUiTransform() const
	{
		return m_uiTransform;
	}

	//座標の取得
	const Vector2& GetPosition() const
	{
		return m_uiTransform.position;
	}

	//名前の取得
	const std::string& GetName()const
	{
		return m_name;
	}

	//衝突判定の形状取得
	const Collider* GetCollider() const
	{
		return m_collider;
	}

	//衝突イベント
	virtual void OnCollision(const UiActor* other) {}

};