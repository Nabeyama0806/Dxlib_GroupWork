#pragma once
#include "ActorBase.h"
#include "Transform.h"
#include <string>

class Model;
class Collider;

class ModelActor : public ActorBase
{
protected:
	Model* m_model;						//3Dモデル		
	Collider* m_collider;				//衝突判定	

	virtual void Load() override;		//リソースの読み込み
	virtual void Release() override;	//リソースの解放
	virtual void Draw() override;		//描画

public:
	//コンストラクタ
	ModelActor(
		const char* name,
		const char* modelFileName = nullptr,
		const Vector3& position = Vector3()
	);

	//子ノードを含む更新
	virtual void TreeUpdate() override;

	//衝突判定の形状取得
	const Collider* GetCollider() const
	{
		return m_collider;
	}

	//衝突イベント
	virtual void OnCollision(const ModelActor* other) {}
};