#pragma once
#include <list>

class Node
{
private:
	Node* m_parent;						//親のポインター
	std::list<Node*>m_children;			//子のポインタリスト
	std::list<Node*> m_removeChildren;	 //削除する子(予約)のポインタリスト

	bool m_isLoaded;					//Load関数を実行したかどうか
	bool m_isDestroyed;					//Destroy関数を実行したかどうか

protected:
	virtual void Load() {}		//リソースの読み込み
	virtual void Release() {}	//リソースの開放
	virtual void Update() {}	//更新
	virtual void Draw() {}		//描画

	void Destroy();		//自身の削除

	//親を設定
	void SetParent(Node* node)
	{
		m_parent = node;
	}

	//親を取得
	Node* GetParent()
	{
		return m_parent;
	}

public:
	//コンストラクタ
	Node() :
		m_parent(nullptr),
		m_isLoaded(false),
		m_isDestroyed(false) {}

	//デストラクタ
	virtual ~Node();

	virtual void TreeLoad();	//子ノードを含むリソースの読み込み
	virtual void TreeRelease();	//子ノードを含むリソースの開放
	virtual void TreeUpdate();	//子ノードを含む更新
	virtual void TreeDraw();	//子ノードを含む描画

	void AddChild(Node* node);		//子ノードの追加
	void RemoveChild(Node* node);	//子ノードの削除
};