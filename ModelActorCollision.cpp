#include "ModelActorCollision.h"
#include "ModelActor.h"
#include "Collider.h"

//更新
void ModelActorCollision::Update()
{
	std::list<ModelActor*> actorList(m_modelActorList);

	//総当たりで衝突判定
	for (auto it1 = actorList.begin(); it1 != actorList.end(); ++it1)
	{
		for (auto it2 = it1; it2 != actorList.end(); ++it2)
		{
			//イテレータから取り出す
			ModelActor* actor1 = *it1;
			ModelActor* actor2 = *it2;

			//衝突判定の形状がなければスキップ
			if (!actor1->GetCollider()) continue;
			if (!actor2->GetCollider()) continue;

			//自身はスキップ
			if (actor1 == actor2) continue;

			//衝突判定
			if (actor1->GetCollider()->CheckCollision(
				actor1->GetTransform(),
				actor2->GetTransform(), actor2->GetCollider()))
			{
				actor1->OnCollision(actor2);
				actor2->OnCollision(actor1);
			}
		}
	}
}