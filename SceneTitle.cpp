#include "SceneTitle.h"
#include"SceneGame.h"
#include "DxLib.h"
#include "Input.h"
#include "Screen.h"
#include "Actor.h"

//‰Šú‰»
void SceneTitle::Initialize()
{
	m_rootNode = new Node();

	//”wŒi
}

//I—¹
void SceneTitle::Finalize()
{
	m_rootNode->TreeRelease();
	delete m_rootNode;
	m_rootNode = nullptr;
}

//XV
SceneBase* SceneTitle::Update()
{
	//‚¢‚¸‚ê‚©‚ÌƒL[‚ª‰Ÿ‚³‚ê‚½‚çƒQ[ƒ€ƒV[ƒ“‚ÖˆÚ“®
	if (Input::GetInstance()->IsAnyKeyDown())
	{
		return new SceneGame();
	}

	//ƒm[ƒh‚ÌXV
	m_rootNode->TreeUpdate();

	return this;
}

//•`‰æ
void SceneTitle::Draw()
{
	//ƒm[ƒh‚Ì•`‰æ
	m_rootNode->TreeDraw();

}