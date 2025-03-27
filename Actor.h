#pragma once
#include "Node.h"
#include "Transform.h"
#include <string>

class Model;
class Sprite;

class Actor : public Node
{
protected:
	std::string m_name;					//オブジェクト名
	Transform m_transform;				//姿勢情報
	Model* m_model;
	Sprite* m_sprite;

	virtual void Load() override;		//リソースの読み込み
	virtual void Release() override;	//リソースの解放
	virtual void Draw() override;		//描画

public:
	//コンストラクタ
	Actor(
		const char* name,
		const char* modelFileName = nullptr,
		const Vector3& position = Vector3()
	);

	//子ノードを含む更新
	virtual void TreeUpdate() override;

	//姿勢の取得
	const Transform& GetTransform() const
	{
		return m_transform;
	}

	//座標の取得
	const Vector3& GetPosition() const
	{
		return m_transform.position;
	}

	//名前の取得
	const std::string& GetName() const
	{
		return m_name;
	}

	//衝突イベント
/*	virtual void OnCollision(const Actor* other)
	{
		//何もしない
	}*/
};