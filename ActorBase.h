#pragma once
#include "Node.h"
#include "Transform.h"
#include <string>

class ActorBase : public Node
{
protected:
	std::string m_name;				//オブジェクト名
	Transform m_transform;			//姿勢情報

	virtual void Load() override = 0;		//リソースの読み込み
	virtual void Release() override = 0;	//リソースの開放
	virtual void Draw() override = 0;		//リソースの開放

public:
	//コンストラクタ
	ActorBase(const char* name,
		const char* textureName = nullptr,
		const Vector3& position = Vector3()) :
		m_name(name)
	{
		//座標の設定
		m_transform.position = position;
	}	

	//子ノードを含む更新
	virtual void TreeUpdate() override = 0;

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
	const std::string& GetName()const
	{
		return m_name;
	}

	//大きさの変更
	void ChangeScale(float scale) 
	{
		m_transform.scale *= scale; 
	}
};