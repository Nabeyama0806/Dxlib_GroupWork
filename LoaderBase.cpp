#include "LoaderBase.h"
#include "DxLib.h"
#include <string>

LoaderBase::LoaderBase()
{
#ifdef _DEBUG
	m_fontHandle = CreateFontToHandle(nullptr, FontSize, FontThick);
#endif
}

LoaderBase::~LoaderBase()
{
#ifdef _DEBUG
	DeleteFontToHandle(m_fontHandle);
#endif
}

//リソースの読み込み
int LoaderBase::Load(const char* path)
{
	LoadData* loadData = nullptr;

	//既に読み込んでいるかをチェック
	auto data = m_dataList.find(path);
	if (data != m_dataList.end())
	{
		//読み込み済みのデータで処理をする
		loadData = &data->second;
	}
	else
	{
		//新規にロードして登録する

		//ロード
		int id = LoadResource(path);

		//登録
		auto result = m_dataList.emplace(path, LoadData(id));

		//登録したデータで処理する
		loadData = &result.first->second;
	}

	//読み込みカウントを増やす
	loadData->count++;

	return loadData->id;
}

//リソースの破棄
void LoaderBase::Delete(const char* path)
{
	LoadData* loadData = nullptr;

	//読み込んでいるかチェック
	auto data = m_dataList.find(path);

	if (data == m_dataList.end())
	{
		//読み込んでいなかったので何もしない
		return;
	}

	loadData = &data->second;

	//読み込みカウントを減らす
	loadData->count--;

	//読み込みカウントが0になったら破棄する
	if (loadData->count <= 0)
	{
		//破棄
		DeleteResource(loadData->id);

		//一覧からも削除
		m_dataList.erase(path);
	}
}

#ifdef _DEBUG
#include "Screen.h"

void LoaderBase::Draw()
{
	int row = 0;

	for (auto data : m_dataList)
	{
		std::string str;
		str = data.first;
		str += " : ";
		str += std::to_string(data.second.count);

		int width = GetDrawStringWidthToHandle(
			str.c_str(),
			static_cast<int>(str.length()),
			m_fontHandle
		);

		width += FontMargin;

		DrawStringToHandle(
			Screen::Right - width, row * FontSize + FontMargin,
			str.c_str(),
			GetColor(55, 0, 255),
			m_fontHandle
		);

		row++;
	}
}
#endif