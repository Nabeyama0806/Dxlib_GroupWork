#include "Time.h"
#include "DxLib.h"

Time::Time()
{
	m_oldTime = GetNowCount();
	m_deltaTime = 0;
}

void Time::Update()
{
	// 端末が起動してからの経過時間（単位 ミリ秒）を取得
	int nowTime = GetNowCount();

	// 前回との差分を単位 秒で得る
	m_deltaTime = (nowTime - m_oldTime) / 1000.0f;

	// 端末の経過時間の保持
	m_oldTime = nowTime;
}
