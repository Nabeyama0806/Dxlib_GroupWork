#pragma once

class Fade
{
private:
	static constexpr int BrightBlack = -255;	//輝度 : 黒
	static constexpr int BrightWhite = 255;		//輝度 : 白
	static constexpr int BrightNeutral = 0;		//輝度 : 通常

	enum class Phase
	{
		Idle,	//何もしていない
		Fade,	//フェード中
	};

	Phase m_phase;
	float m_elapsedTime;	//経過時間
	float m_duration;		//所要時間
	int m_beginBright;		//フェード開始時の輝度
	int m_endBright;		//フェード終了時の輝度

	//コンストラクタ
	Fade();

public:
	//シングルトン
	static Fade* GetInstance()
	{
		static Fade instance;
		return &instance;
	}

	enum class Color
	{
		Black,
		White,
	};

	//更新
	void Update(int screen);

	//フェードイン
	void StartFadeIn(float duration = 0.5f, Color color = Color::Black);

	//フェードアウト
	void StartFadeOut(float duration = 0.5f, Color color = Color::Black);

	//フェード中かどうか
	bool IsFade()
	{
		return m_phase == Phase::Fade;
	}
};