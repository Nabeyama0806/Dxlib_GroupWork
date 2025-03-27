#pragma once
#include "Dxlib.h"
#include "ModelActor.h"
#include <vector>

class Transform;

class ModelAnimation
{
private:
	static constexpr int FPS = 60;					//フレームレート
	static constexpr float BlendDuration = 0.3f;	//アニメーションの必要時間

	const char* m_animeFileName;
	int m_modelHandle;			//アニメーションのアタッチ先
	int m_animeHandle;			//アニメーションモデル
	int m_attachIndex;			//アタッチするアニメーションの要素番号

	float m_animeTime;			//アニメの再生時間
	float m_animeTotalTime;		//アニメの総再生時間

	float m_blendStart;	//アニメーション開始の影響度
	float m_blendEnd;	//アニメーション終了の影響度
	float m_blendTime;	//アニメーション遷移開始からの経過時間

	bool m_isFarst;		//先頭のアニメかどうか
	
public:
	//デストラクタ
	virtual ~ModelAnimation();
	
	//コンストラクタ
	ModelAnimation(const char* animeFileName, const bool isFast);

	//リソースの読み込み
	void Load(const int modelHandle);

	//リソースの解放
	void Release();

	//更新
	void Update();

	//アニメーション開始
	void FadeIn()
	{
		m_blendTime = 0.0f;
		m_blendStart = 0.0f;
		m_blendEnd = 1.0f;
	}

	//アニメーション終了
	void FadeOut()
	{
		m_blendTime = 0.0f;
		m_blendStart = 1.0f;
		m_blendEnd = 0.0f;
	}

	bool IsFade() { return m_blendTime >= 0; }
};