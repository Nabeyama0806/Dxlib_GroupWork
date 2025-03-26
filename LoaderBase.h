#pragma once
#include <map>
#include "LoadData.h"

//メモ : 抽象クラスはオブジェクトを作れない(ポインタは作れる)

//リソースの基底ローダークラス(抽象クラス)
class LoaderBase
{
private:
	const char* RootPath = "Resource/";
	std::map<const char*, LoadData> m_dataList;	//リソース一覧

#ifdef _DEBUG
	static const int FontSize = 10;
	static const int FontThick = 3;
	static const int FontMargin = 2;
	int m_fontHandle;
#endif

protected:
	LoaderBase();
	virtual ~LoaderBase();

	virtual int LoadResource(const char* path) = 0;	//リソースの読み込み
	virtual void DeleteResource(int id) = 0;		//リソースの破棄

public:
	int Load(const char* path);		//リソースの読み込み
	void Delete(const char* path);	//リソースの破棄

#ifdef _DEBUG
	void Draw();
#endif
};