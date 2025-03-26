#pragma once

struct LoadData
{
public:
	const int id;	//ロードしたリソースハンドル
	int count;		//idが利用されている個数

	LoadData(int id) : 
		id(id),
		count(0) {}
};