#include "PlaySE.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;
using namespace CocosDenshion;

// コンストラクタ
PlaySE::PlaySE(string sound)
{
    m_sound = sound;
}


PlaySE* PlaySE::create(string sound)
{
    PlaySE *pRet = new PlaySE(sound);
    if (pRet)
    {
        pRet->autorelease();
    }
    return pRet;
}

bool PlaySE::update(float time)
{
    CC_UNUSED_PARAM(time);
    SimpleAudioEngine::getInstance()->playEffect(m_sound.c_str());
}

FiniteTimeAction* PlaySE::reverse()
{
    return (FiniteTimeAction*)(PlaySE::create(m_sound));
}

Object* PlaySE::copyWithZone(Zone* pZone)
{
    CCZone* pNewZone = NULL;
    PlaySE* pRet = NULL;
    if (pZone && pZone->m_pCopyObject) {
        pRet = (PlaySE*) (pZone->m_pCopyObject);
    }
    else
    {
        pRet = new PlaySE(m_sound);
        pZone = pNewZone = new Zone(pRet);
    }
    ActionInstant::copyWithZone(pZone);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}