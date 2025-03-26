#include "Node.h"

//デストラクタ
Node::~Node()
{
	//子ノードを全てdeleteする
	for (Node* node : m_children)
	{
		delete node;
	}
	m_children.clear();
}

//自身の削除
void Node::Destroy()
{
	//親がいなければなにもしない
	if (!m_parent) return;

	//既に削除済みなら何もしない
	if (m_isDestroyed) return;

	//自身の削除
	m_isDestroyed = true;

	//親に自身を削除させる
	m_parent->RemoveChild(this);
}

//子ノードを含むリソースの読み込み
void Node::TreeLoad()
{
	//自身
	if (!m_isLoaded)
	{
		Load();
		m_isLoaded = true;
	}

	//子
	for (Node* node : m_children)
	{
		node->TreeLoad();
	}
}

//子ノードを含むリソースの解放
void Node::TreeRelease()
{
	//自身
	if (m_isLoaded)
	{
		Release();
		m_isLoaded = false;
	}

	//子
	for (Node* node : m_children)
	{
		node->TreeRelease();
	}
}

//子ノードを含む更新
void Node::TreeUpdate()
{
	//自身
	Update();

	//子
	for (Node* node : m_children)
	{
		node->TreeUpdate();
	}

	//子の削除
	for (Node* node : m_removeChildren)
	{
		node->TreeRelease();
		m_children.remove(node);
		delete node;
	}
	m_removeChildren.clear();
}

//子ノードを含む描画
void Node::TreeDraw()
{
	//自身
	Draw();

	//子
	for (Node* node : m_children)
	{
		node->TreeDraw();
	}
}

//子ノードの追加
void Node::AddChild(Node* node)
{
	//子に対して親を設定
	node->SetParent(this);

	//子を登録
	m_children.push_back(node);

	//子のリソース読み込み
	node->TreeLoad();
}

//子ノードの削除
void Node::RemoveChild(Node* node)
{
	m_removeChildren.push_back(node);
}