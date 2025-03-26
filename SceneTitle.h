#pragma once
#include "SceneBase.h"
#include "Node.h"

class Node;

//タイトルシーン
class SceneTitle : public SceneBase
{
private:
	Node* m_rootNode;

public:
	//コンストラクタ
	SceneTitle() :
		m_rootNode(nullptr){}

	virtual void Initialize() override;				//初期化
	virtual void Finalize() override;				//終了
	virtual SceneBase* Update() override;			//更新
	virtual void Draw() override;					//描画
};