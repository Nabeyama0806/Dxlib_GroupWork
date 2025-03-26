#pragma once
#include "LoaderBase.h"

//3Dモデルローダークラス
class ModelLoader : public LoaderBase
{
private:
	ModelLoader() {}

protected:
	virtual int LoadResource(const char* path) override;
	virtual void DeleteResource(int id) override;

public:
	//シングルトン
	static ModelLoader* GetInstance()
	{
		static ModelLoader instance;
		return &instance;
	}
};