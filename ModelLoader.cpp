#include "ModelLoader.h"
#include "DxLib.h"

int ModelLoader::LoadResource(const char* path)
{
	return MV1LoadModel(path);
}

void ModelLoader::DeleteResource(int id)
{
	MV1DeleteModel(id);
}