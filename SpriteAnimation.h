#pragma once

struct SpriteAnimation
{
public:
	const char* textureName; //画像パス
	int textureId;			 //描画する画像
	int gridAmount;			 //アニメーションのコマ数
	int sampleRate;			 //1秒完にアニメーションのコマを切り替える回数
	bool loopFlag;			 //アニメーションのがループするかどうか

	//コンストラクタ
	SpriteAnimation(
		const char* textureName,
		int gridAmount = 1,
		int sampleRate = 10,
		bool loopFlag = true
	) :
		textureName(textureName),
		textureId(0),
		gridAmount(gridAmount),
		sampleRate(sampleRate),
		loopFlag(loopFlag) {
	}
};
