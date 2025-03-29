#include "WaterBottle.h"
#include "Model.h"

//コンストラクタ
WaterBottle::WaterBottle(Vector3& position, Vector3& addForce) :
	Bottle(position)
{
	m_model = new Model("Resource/honouポーション.mv1");

	m_addForce = addForce;
}

//更新
void WaterBottle::Update()
{
	//本来の更新
	ModelActor::Update();

	//投げる
	Throw(m_transform.position, m_addForce);
}

//描画
void WaterBottle::Draw()
{
	//本来の描画
	ModelActor::Draw();
}

//効果発動
void WaterBottle::ActiveEffect()
{
	//自身を削除
	Destroy();
}