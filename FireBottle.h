#pragma once
#include "Bottle.h"

class FireBottle : public Bottle
{
protected:
	virtual void Update() override;			//更新
	virtual void Draw() override;			//描画
	virtual void ActiveEffect() override;	//効果発動

public:
	//コンストラクタ
	FireBottle(Vector3& position, Vector3& addForce);
};