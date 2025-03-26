#pragma once

//クラスの前方宣言
class SceneBase;

//シーン遷移クラス
class SceneManager
{
private:

	//フェーズ
	enum class Phase
	{
		Run,		//実行中
		FadeOut,	//フェードアウト
		Transition,	//遷移
	};
private:

	SceneBase* m_scene;				//実行中のシーン
	SceneBase* m_nextScene;			//遷移先のシーン
	Phase m_phase;					//フェーズ

public:

	//コンストラクタ
	SceneManager(SceneBase* initScene) :
		m_scene(nullptr),
		m_nextScene(initScene),
		m_phase(Phase::Transition)  {}

	//デストラクタ
	virtual ~SceneManager();

	void Updeta();		//更新
	void Draw();		//描画
};
