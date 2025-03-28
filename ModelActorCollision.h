#pragma once
#include <list>

class ModelActor;

class ModelActorCollision
{
private:
	std::list<ModelActor*> m_modelActorList;	//衝突判定を行うリスト

public:
	//コンストラクタ
	ModelActorCollision() {}

	//シングルトン
	static ModelActorCollision* GetInstance()
	{
		static ModelActorCollision instance;
		return &instance;
	}

	//登録
	void Register(ModelActor* actor)
	{
		m_modelActorList.push_back(actor);
	}

	//削除
	void Remove(ModelActor* actor)
	{
		m_modelActorList.remove(actor);
	}

	//更新
	void Update();
	// 描画
	void Draw();
};