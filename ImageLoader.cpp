#include "ImageLoader.h"
#include "DxLib.h"

int ImageLoader::LoadResource(const char* path)
{
	return LoadGraph(path);
}

void ImageLoader::DeleteResource(int id)
{
	DeleteGraph(id);
}