#include "tmpUi.h"
#include "Time.h"
#include "Screen.h"
#include "Vector3.h"

//コンストラクタ
tmpUi::tmpUi() :
	UiActor(
		"tmp",
		"Resource/title.png",
		Screen::Center
	)
{
	m_uiTransform.scale = 0.1f;
}