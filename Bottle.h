#pragma once
#include "ModelActor.h"

class Bottle : public ModelActor
{
public:
	//ボトルの属性
	enum class  Type
	{
		Fire,		//炎
		Water,		//水
		Thunder,	//雷
		Wind,		//風

		Length
	};

	//モデル名
	const char* ModelNames[static_cast<int>(Type::Length)] =
	{
		"Fire",
		"Water",
		"Thunder",
		"Wind"
	};

	//モデルのファイルパス
	const char* ModelFilePath[static_cast<int>(Type::Length)] =
	{
		"Resource/honouポーション.mv1",
		"Resource/mizuポーション.mv1",
		"Resource/kaminariポーション.mv1",
		"Resource/kaminariポーション.mv1"
	};

private:
	static constexpr float Gravity = 1.2f;	//重力

protected:
	virtual void Update() override;	//更新
	virtual void Draw() override;	//描画

public:
	//コンストラクタ
	Bottle(Vector3& position) :
		ModelActor("Bottle", nullptr, position)
	{
	}
};