#ifndef __BLOCK_SPRITE_H__
#define __BLOCK_SPRITE_H__

#include "cocos2d.h"
#include "Config.h"

class BlockSprite : public cocos2d::Sprite
{
protected:
//    const char* getBlockImageFileName(kBlock blockType);

public:
    const char* getBlockImageFileName(kBlock blockType);
    // マクロ定義 アクセッサ
    // protectedメンバとして kBlock m_blockType;
    // publicメンバとして virtual kBlock getBlockType(void) const {return m_blockType;}
    CC_SYNTHESIZE_READONLY(kBlock, m_blockType, BlockType);
    // protectedメンバとして int m_nextPosX;
    // publicメンバとして virtual int getNextPosX(void) const {return m_nextPosX;}
    CC_SYNTHESIZE_READONLY(int, m_nextPosX, NextPosX);
    // protectedメンバとして int m_nextPosY;
    // publicメンバとして virtual int getNextPosY(void) const {return m_nextPosY;}
    CC_SYNTHESIZE_READONLY(int, m_nextPosY, NextPosY);

    // x,yインデックス初期化のため
    void initNextPos();
    // 移動先のx,yインデックスをセットするため
    void setNextPos(int nextPosX, int nextPosY);
    
    BlockSprite();
    virtual ~BlockSprite();
    virtual bool initWithBlockType(kBlock blockType);
    // create関数
    static BlockSprite* createWithBlockType(kBlock blockType);
};

#endif // __BLOCK_SPRITE_H__
