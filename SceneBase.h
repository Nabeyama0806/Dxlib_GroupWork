#pragma once

//	シーン基底クラス
class SceneBase
{
public:
	virtual ~SceneBase(){}

	//純粋仮想関数
	virtual void Initialize() = 0;				//初期化
	virtual void Finalize() = 0;				//終了
	virtual SceneBase* Update() = 0;			//更新
	virtual void Draw() = 0;					//描画
};
