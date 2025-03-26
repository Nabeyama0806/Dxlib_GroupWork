#pragma once

class SceneManager;

//ゲームメインループクラス
class GameMain
{
private:
	SceneManager* m_sceneManager;	//シーン遷移
	int m_screen;	//自作スクリーン
public:
	//コンストラクタ(実行時に自動的に呼び出される関数)
	GameMain() :
		m_sceneManager(nullptr),
		m_screen(0)
	{
	}

	//デストラクタ
	virtual ~GameMain();

	//メインループ実行
	void Run();
};