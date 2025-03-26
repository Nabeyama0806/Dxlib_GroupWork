#include "Fade.h"
#include "Time.h"
#include "Lerp.h"
#include "DxLib.h"

//コンストラクタ
Fade::Fade() :
	m_phase(Phase::Idle),
	m_elapsedTime(0),
	m_duration(0),
	m_beginBright(0),
	m_endBright(0)
{
}

//更新
void Fade::Update(int screen)
{
	//フェード中じゃなければ何もしない
	if (!IsFade()) return;

	//時間経過
	m_elapsedTime += Time::GetInstance()->GetDeltaTime();

	//所要時間に対しての経過時間を0.0~1.0の割合で算出する
	float t = m_elapsedTime / m_duration;

	//経過時間が所要時間を超えていたら1.0に整える
	if (t >= 1.0f)
	{
		t = 1.0f;
		m_phase = Phase::Idle;
	}

	//輝度の設定を線形補間で作る
	int bright = Lerp::Exec(m_beginBright, m_endBright, t);

	//引数で受け取ったスクリーンに対して輝度を設定
	GraphFilter(screen, DX_GRAPH_FILTER_HSB, 0, 0, 0, bright);
}

//フェードイン
void Fade::StartFadeIn(float duration, Color color)
{
	m_phase = Phase::Fade;
	m_elapsedTime = 0;
	m_duration = duration;
	m_beginBright = (color == Color::Black) ? BrightBlack : BrightWhite;
	m_endBright = BrightNeutral;
}

//フェードアウト
void Fade::StartFadeOut(float duration, Color color)
{
	m_phase = Phase::Fade;
	m_elapsedTime = 0;
	m_duration = duration;
	m_beginBright = BrightNeutral;
	m_endBright = (color == Color::Black) ? BrightBlack : BrightWhite;
}