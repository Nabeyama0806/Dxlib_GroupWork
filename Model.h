#pragma once
#include "Animation.h"
#include <vector>

class Transform;

class Model
{
public:
	enum class Anime
	{
		Idle,
		Run,

		Length,
	};

private:
	std::vector<Animation*> m_animeList;	//アニメーション配列
	Animation* m_prevAnime;					//以前のアニメ
	Animation* m_nowAnime;					//現在のアニメ
	float m_elapsedTime;					//経過時間(秒)
	int m_modelHandle;						//モデルのID
	const char* m_modelFileName;			//モデルのファイルパス

public:
	//コンストラクタ
	Model(const char* modelFileName);

	//デストラクタ
	virtual ~Model();

	//モデルの読み込み
	void Load();

	//モデルの解放
	void Release();

	//更新
	void Update();

	//描画
	void Draw(const Transform& transform);

	//アニメーションの登録
	void Register(const char* animeFileName);

	//アニメーションの切り替え
	void PlayAnime(int animeIndex);
};