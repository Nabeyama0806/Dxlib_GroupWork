#pragma once
#include "SceneBase.h"

class Node;

class SceneResult : public SceneBase
{
private:
	static constexpr float WaitTransitionTime = 1.5f;

	Node* m_rootNode;
	int m_score;

	float m_waitTransitionTime;

public:
	//コンストラクタ
	SceneResult(int score) :
		m_rootNode(nullptr),
		m_score(score),
		m_waitTransitionTime(WaitTransitionTime) {}

	virtual void Initialize() override;
	virtual void Finalize() override;
	virtual SceneBase* Update() override;
	virtual void Draw() override;
};