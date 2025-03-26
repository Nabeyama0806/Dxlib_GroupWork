#pragma once
#include"SceneBase.h"
#include "Vector3.h"
#include <list>

//クラスの前方宣言
class Node;
class Player;
class Camera;

//	ゲームシーン
class SceneGame : public SceneBase
{
private:
	const std::list<const char*> ImagePreload =
	{
		//"title.png"
	};

	static constexpr float ResultTransitionTime = 2.0f;	//プレイヤーが死んでからリザルト画面に遷移するまでの時間
	Node* m_rootNode;	//ツリーノード
	Player* m_player;	//プレイヤー
	Camera* m_mainCamera;	//プレイヤー

	Vector3 cameraPos = Vector3(0, 0, -700);

	float m_resultTransitionTime;
	int m_bgm;
public:
	//コンストラクタ
	SceneGame() :
		m_rootNode(nullptr), 
		m_player(nullptr),
		m_mainCamera(nullptr),
		m_resultTransitionTime(ResultTransitionTime),
		m_bgm(0){}

	virtual void Initialize() override;		//初期化
	virtual void Finalize() override;		//終了
	virtual SceneBase* Update() override;	//更新
	virtual void Draw() override;			//描画
};