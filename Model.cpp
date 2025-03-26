#include "Model.h"
#include "ModelLoader.h"
#include "Transform.h"
#include "Math.h"
#include "Dxlib.h"

//デストラクタ
Model::~Model()
{
	Release();
}

//コンストラクタ
Model::Model(const char* modelFileName) :
	m_modelHandle(0),
	m_modelFileName(modelFileName),
	m_prevAnime(nullptr),
	m_nowAnime(nullptr),
	m_elapsedTime(0)
{
}

//リソースの読み込み
void Model::Load()
{
	m_modelHandle = ModelLoader::GetInstance()->Load(m_modelFileName);
	MV1SetupCollInfo(m_modelHandle, 0, 8, 8, 8);
	for (auto anime : m_animeList)
	{
		anime->Load(m_modelHandle);
	}
}

//リソースの解放
void Model::Release()
{
	//モデルデータの削除
	ModelLoader::GetInstance()->Delete(m_modelFileName);

	//アニメーションの削除
	for (auto anime : m_animeList)
	{
		delete anime;
		anime = nullptr;
	}
}

//更新
void Model::Update()
{
	if (m_nowAnime) m_nowAnime->Update();
	if (m_prevAnime)
	{
		m_prevAnime->Update();
		if (!m_prevAnime->IsFade()) m_prevAnime = nullptr;
	}
}

//描画
void Model::Draw(const Transform& transform)
{
	//モデルの姿勢情報を設定	
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetScale(transform.scale));
	mat = MMult(mat, transform.rotation);
	mat = MMult(mat, MGetTranslate(transform.position));
	MV1SetMatrix(m_modelHandle, mat);

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

//アニメーション登録
void Model::Register(const char* animeFileName)
{
	Animation* anime = new Animation(animeFileName, m_animeList.empty());

	//リストに登録
	m_animeList.push_back(anime);
	if (!m_nowAnime)
	{
		m_nowAnime = anime;
	}
}

//アニメーションの切り替え
void Model::PlayAnime(int animeIndex)
{
	//変更後のアニメと現在のアニメが同じなら何もしない
	if (m_animeList[animeIndex] == m_nowAnime) return;

	//現在のアニメを以前のアニメに変更
	m_prevAnime = m_nowAnime;

	//現在のアニメを引数のアニメに変更
	m_nowAnime = m_animeList[animeIndex];

	//アニメーションをフェードさせる
	m_nowAnime->FadeIn();
	m_prevAnime->FadeOut();
}
