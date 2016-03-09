#include "BlockSprite.h"

BlockSprite::BlockSprite()
{
    initNextPos();
}

BlockSprite::~BlockSprite()
{
}

BlockSprite* BlockSprite::createWithBlockType(kBlock blockType)
{
    BlockSprite *pRet = new BlockSprite();
    if (pRet && pRet->initWithBlockType(blockType))
    {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }

}

bool BlockSprite::initWithBlockType(kBlock blockType)
{
    //　ファイル名を取得できたかの判定 取得できた場合はスルー
    // Sprite::initWithFile => ファイル名を連想配列にある場合は何もしなく、ない場合はロードする
    // getBlockImageFileName => ファイル名を取得する(下にあるメソッド)
    if (!Sprite::initWithFile(getBlockImageFileName(blockType))) {
        return false;
    }
    
    m_blockType = blockType;
    
    return true;
}

// 引数に応じてファイル名を返す, 引数に合うものがない場合はメッセージを表示し空文字を返す
const char* BlockSprite::getBlockImageFileName(kBlock blockType)
{
    switch (blockType) {
        case kBlockRed:
            return "nyangame/red.png";
        case kBlockBlue:
            return "nyangame/blue.png";
        case kBlockYellow:
            return "nyangame/yellow.png";
        case kBlockGreen:
            return "nyangame/green.png";
        case kBlockGray:
            return "nyangame/gray.png";
        default:
            CCAssert(false, "invalid blockType");
            return "";
    }
}

// 移動先の初期化
void BlockSprite::initNextPos()
{
    // インデックスは0以上のため-1は移動がない
    m_nextPosX = -1;
    m_nextPosY = -1;
}

// 移動先のインデックスをセット
void BlockSprite::setNextPos(int nextPosX, int nextPosY)
{
    m_nextPosX = nextPosX;
    m_nextPosY = nextPosY;
}

