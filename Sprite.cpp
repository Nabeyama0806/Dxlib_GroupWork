#include "Sprite.h"
#include "Time.h"
#include "ImageLoader.h"
#include "DxLib.h"

//デストラクタ
Sprite::~Sprite()
{
	Release();
}

// リソースの読み込み
void Sprite::Load()
{
	int firstTexture = 0;
	for (auto& pair : m_spriteAnimationList)
	{
		//リストから<アニメーション名：データ>のペアを取り出す
		//pair.first : アニメーション名
		//pair.secon：データ

		//データから画像パスを読み込んで決める
		pair.second.textureId = ImageLoader::GetInstance()->Load(pair.second.textureName);

		if (!firstTexture)
		{
			firstTexture = pair.second.textureId;
		}
	}

	//グリッドサイズを指定しなかった場合は、画像からサイズを取得
	if (gridSize.IsZero())
	{
		int w, h;
		GetGraphSize(firstTexture, &w, &h);
		gridSize = Vector2(w, h);
	}
}
//リソースの開放
void Sprite::Release()
{
	for (auto& pair : m_spriteAnimationList)
	{
		ImageLoader::GetInstance()->Delete(pair.second.textureName);
	}

	m_spriteAnimationList.clear();
	m_runningAnime = nullptr;
}
//更新
void Sprite::Update()
{
	//アニメーションが終了していないか
	if (IsFinishAnime()) return;

	//経過時間の加算
	m_elapsedTime += Time::GetInstance()->GetDeltaTime();

	//経過時間がそうアニメーション時間を超えたらループ処理
	if (IsFinishAnime() && m_runningAnime->loopFlag)
	{
		//最初のコマに戻る
		m_elapsedTime = 0;
	}
}

//描画
void Sprite::Draw(const Transform& transform)
{
	//再生するものがなければ何もしない
	if (!m_runningAnime) return;
	//描画するコマを計算
	int gridIndex;
	if (IsFinishAnime())
	{
		gridIndex = m_runningAnime->gridAmount - 1;
	}
	else
	{
		//経過時間
		gridIndex = static_cast<int> (m_elapsedTime / GetFrameTime());

	}
	//描画
	DrawRectRotaGraph(
		static_cast<int>(transform.position.x), static_cast<int>(transform.position.y),
		static_cast<int>(gridSize.x) * gridIndex, 0,
		static_cast<int>(gridSize.x), static_cast<int>(gridSize.y),
		transform.scale.x,
		transform.rotation.x,
		m_runningAnime->textureId,
		false,
		flipX, flipY
	);

#ifdef _DEBUG

	//グリッド枠の表示
	/*DrawBox(
		static_cast<int>(transform.position.x - gridSize.x * transform.scale / 2),
		static_cast<int>(transform.position.y - gridSize.y * transform.scale / 2),
		static_cast<int>(transform.position.x + gridSize.x * transform.scale / 2),
		static_cast<int>(transform.position.y + gridSize.y * transform.scale / 2),
		GetColor(255, 0, 0), false
	);*/
#endif // _DEBUG2
}

//アニメーション登録

void Sprite::Register(const char* textureName)
{
	Register("-", SpriteAnimation(textureName, 1, 1, false));

}

void Sprite::Register(const char* animeName, const SpriteAnimation& spriteAnimetion)
{
	m_spriteAnimationList.emplace(animeName, spriteAnimetion);

	//最初に登録されたアニメーションを再生中アニメーションとする
	if (!m_runningAnime)
	{
		m_runningAnime = &m_spriteAnimationList.begin()->second;
	}
}

//アニメーション再生
void Sprite::Play(const char* animeName)
{
	//指定されたアニメーションをリストから取得
	const auto& pair = m_spriteAnimationList.find(animeName);

	//未登録のアニメーション名
	if (pair == m_spriteAnimationList.end()) return;

	//現在再生中のアニメーションと同じ
	if (m_runningAnime == &pair->second) return;

	//アニメーションの切り替え
	m_runningAnime = &pair->second;
	m_elapsedTime = 0;
}
