#pragma once

class Time
{
private:
	int m_oldTime;		// 直前の端末経過時間
	float m_deltaTime;	// 前回実行したUpdate関数からの経過時間

	Time();

public:
	// シングルトン
	static Time* GetInstance()
	{
		static Time instance;
		return &instance;
	}

	void Update();
	float GetDeltaTime(){ return m_deltaTime; }
};
