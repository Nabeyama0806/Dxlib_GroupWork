#pragma once
#include "SpriteActor.h"
#include "Bottle.h"

class UiBottle : public SpriteActor
{
private:
	//画像のファイルパス
	const char* FilePath[static_cast<int>(Bottle::Type::Length)] =
	{
		"Resouce/bottle_fire.png",
		"Resouce/bottle_thunder.png",
		"Resouce/bottle_water.png"
	};

	//画像の名前
	const char* TextureName[static_cast<int>(Bottle::Type::Length)] =
	{
		"Fire",
		"Thunder",
		"Water"
	};

	Bottle::Type m_type;	//自身の属性

protected:
	virtual void Update() override;	//更新
	virtual void Draw() override;	//描画

};
