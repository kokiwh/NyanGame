#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Config.h"

USING_NS_CC;
using namespace std;

// 縦のコマ数
#define MAX_BLOCK_Y 8
// 横のコマ数
#define MAX_BLOCK_X 8

// 背景画像のファイル名
#define PNG_BACKGROUND "nyangame/background.png"
// ゲームオーバーファイル名
#define PNG_GAMEOVER "nyangame/gameover.png"
// リセットボタンファイル名
#define PNG_RESET "nyangame/reset.png"
// 効果音のファイル名
#define MP3_REMOVE_BLOCK "nyangame/decision4.mp3"
// コマを小さくするアニメーションの時間(削除された時)
#define REMOVING_TIME 0.1f
// コマの移動時間
#define MOVING_TIME 0.2f
// フォント5種類
#define FONT_RED "nyangame/redFont.fnt"
#define FONT_BLUE "nyangame/blueFont.fnt"
#define FONT_YELLOW "nyangame/yellowFont.fnt"
#define FONT_GREEN "nyangame/greenFont.fnt"
#define FONT_GRAY "nyangame/grayFont.fnt"
#define FONT_WHITE "nyangame/whiteFont.fnt"
// ハイスコアのキー
#define KEY_HIGHSCORE "HighScore"

class Game : public Layer
{
protected:
    enum kTag
    {
        // 画像のタグ
        kTagBackground = 1,
        // 2〜6のタグ値
        kTagRedLabel,
        kTagBlueLabel,
        kTagYellowLabel,
        kTagGreenLabel,
        kTagGrayLabel,
        kTagScoreLabel,
        kTagGameOver,
        kTagHighScoreLabel,
        // コマの基準タグ
        kTagBaseBlock = 10000,
    };

    enum kZOrder
    {
        // zオーダー 表示順
        kZOrderBackground,
        // ラベルのタグ
        kZOrderLabel,
        // コマの基準タグ
        kZOrderBlock,
        // ゲームオーバタグ
        kZOrderGameOver,
    };
    
    struct PositionIndex
    {
        PositionIndex(int x1, int y1)
        {
            x = x1;
            y = y1;
        }
        int x;
        int y;
    };

    // 背景の表示関連 //
    Sprite* m_background;
    // メソッド宣言
    void showBackground();
    
    // コマの表示関連 //
    // コマの一辺のサイズを記録する
    float m_blockSize;
    // コマの種類ごとのグループ
    map<kBlock, list<int>> m_blockTags;
    // メソッド宣言
    void touchEnabled(bool flg);
    void initForVariables();
    void showBlock();
    Point getPosition(int posIndexX, int posIndexY);
    int getTag(int posIndexX, int posIndexY);
    
    // コマの削除関連 //
    // メソッド宣言
    // タップされたコマのタグを取得するメソッドの宣言
    void getTouchBlockTag(Point touchPoint, int &tag, kBlock &blockType);
    list<int> getSameColorBlockTags(int baseTag, kBlock blockType);
    void removeBlock(list<int> blockTags, kBlock blockType);
    bool hasSameColorBlock(list<int> blockTagList, int searchBlockTag);
    
    // コマの削除
    void removingBlock(Node* block);
    
    // アニメーションに必要な変数の宣言(縦)
    vector<kBlock> blockTypes;
    PositionIndex getPositionIndex(int tag);
    void setNewPosition1(int tag, PositionIndex posIndex);
    void searchNewPosition1(list<int> blocks);
    void moveBlock();
    void movingBlocksAnimation1(list<int> blocks);
    
    // アニメーションに必要な変数の宣言(横)
    // アニメーション中かの変数
    bool m_animating;
    // アニメーション完了時の処理
    void movedBlocks();
    map<int, bool> getExistBlockColumn();
    void searchNewPosition2();
    void setNewPosition2(int tag, PositionIndex posIndex);
    void movingBlocksAnimation2(float frame);

    // ラベルの初期表示
    void showLabel();
    
    // スコア保持変数
    int m_score;
    
    // ゲームオーバ
    bool existsSameBlock();

    // ハイスコアの保存
    void saveHighScore();
    // ハイスコアの表示
    void showHighScoreLabel();

    // リセットボタン
    void menuResetCallback(Ref* pSender);
    void showResetButton();
    
public:
    static Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(Game);

    // タップ開始時に処理されるメソッドを宣言
    virtual bool TouchBegan(Touch* pTouch, Event* pEvent);
    // タップ終了時に処理されるメソッドを宣言
    virtual void TouchEnded(Touch* pTouch, Event* pEvent);
    
};

#endif // __GAME_SCENE_H__
