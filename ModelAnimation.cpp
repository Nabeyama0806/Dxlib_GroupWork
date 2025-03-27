#include "ModelAnimation.h"
#include "ModelLoader.h"
#include "Transform.h"
#include "Lerp.h"
#include "Time.h"

//コンストラクタ
ModelAnimation::ModelAnimation(const char* animeFileName, const bool isFarst) :
    m_animeFileName(animeFileName),
    m_modelHandle(0),
    m_attachIndex(0),
    m_animeHandle(0),
    m_animeTime(0),
    m_animeTotalTime(0),
    m_blendStart(0),
    m_blendEnd(0),
    m_blendTime(-1),
    m_isFarst(isFarst)
{ 
}

ModelAnimation::~ModelAnimation()
{
    Release();
}

//リソースの読み込み
void ModelAnimation::Load(const int modelHandle)
{
    //モデルの識別番号を取得
    m_modelHandle = modelHandle;

    //アニメーションの識別番号を取得
     m_animeHandle = ModelLoader::GetInstance()->Load(m_animeFileName);

    //アタッチするアニメーションの要素番号を指定
    //ただし、アニメーションは個別で管理しているため、常に0番目をアタッチする
    m_attachIndex = MV1AttachAnim(m_modelHandle, 0, m_animeHandle);

    //アニメーションの総再生時間の取得
    m_animeTotalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_attachIndex);

    //現在のアニメーション影響度を0にする
    MV1SetAttachAnimBlendRate(m_modelHandle, m_attachIndex, m_isFarst ? 1.0f : 0.0f);
}

//リソースの解放
void ModelAnimation::Release()
{
    //アニメーションの削除
    ModelLoader::GetInstance()->Delete(m_animeFileName);
}

//更新
void ModelAnimation::Update()
{
    //再生時間が総アニメ時間を超えると再生時間を0にしてアニメーションをループさせる
    m_animeTime += Time::GetInstance()->GetDeltaTime();
    if (m_animeTime >= m_animeTotalTime / FPS) m_animeTime = 0;
    MV1SetAttachAnimTime(m_modelHandle, m_attachIndex, m_animeTime * FPS);

    //アニメーションの切り替え
    if (m_blendTime >= 0)
    {
        //切り替えに必要な時間を加算し、割合を算出
        m_blendTime += Time::GetInstance()->GetDeltaTime();
        float t = m_blendTime / BlendDuration;

        //割合が1を超えるとアニメの切り替えが完了
        if (t >= 1.0f)
        {
            //値をリセットする
            t = 1.0f;
            m_blendTime = -1;
        }

        //アニメーションの切り替え処理
        float blendRate = Lerp::Exec(m_blendStart, m_blendEnd, t);
        MV1SetAttachAnimBlendRate(m_modelHandle, m_attachIndex, blendRate);
    }
}
