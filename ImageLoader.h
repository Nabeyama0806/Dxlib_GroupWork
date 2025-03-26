#pragma once
#include "LoaderBase.h"

//画像ローダークラス
class ImageLoader : public LoaderBase
{
private:
	ImageLoader() {}

protected:
	virtual int LoadResource(const char* path) override;
	virtual void DeleteResource(int id) override;

public:
	//シングルトン
	static ImageLoader* GetInstance()
	{
		static ImageLoader instance;
		return &instance;
	}
};