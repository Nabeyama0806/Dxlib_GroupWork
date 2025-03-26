#pragma once
#include "Vector2.h"

//画面に関する定数
namespace Screen
{
	static constexpr int Width = 1280; //幅
	static constexpr int Heigth = 720;//高さ
	static constexpr Vector2 Size = Vector2(Width, Heigth);//幅・高さ

	static constexpr int Top = 0;				//上
	static constexpr int Bottom = Heigth;		//下
	static constexpr int Left = 0;				//左
	static constexpr int Right = Width;			//右
	static constexpr Vector2 Center = Size / 2;	//中央

	static constexpr Vector2 TopLeft = Vector2(Left, Top);			//左上
	static constexpr Vector2 TopCenter = Vector2(Center.x, Top);	//中央上
	static constexpr Vector2 TopRight = Vector2(Right, Top);		//右上

	static constexpr Vector2 CenterLeft = Vector2(Left, Center.y);			//中央左
	static constexpr Vector2 CenterRight = Vector2(Right, Center.y);		//中央上

	static constexpr Vector2 BottomLeft = Vector2(Left, Bottom);		//左下
	static constexpr Vector2 BottomCenter = Vector2(Center.x, Bottom);	//中央下
	static constexpr Vector2 BottomRight = Vector2(Right, Bottom);		//右下
};