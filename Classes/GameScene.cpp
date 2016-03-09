#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "BlockSprite.h"

using namespace CocosDenshion;

Scene* Game::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Game::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// タップイベントを取得する 引数は有効性
void Game::touchEnabled(bool flg)
{
    // setTouchEnabled(true); 非推奨
    // setTouchMode(kCCTouchesOneByOne); 非推奨
    // _touchEnabledはcclayerで用意されてる変数 bool
    if (_touchEnabled != flg) {
        // 有効性をセット
        _touchEnabled = flg;
        if (flg) {
            // すでにイベントが設定されていた場合はreturn
            // nullptrはポインタとして扱ってくれる
            if (_touchListener != nullptr) {
                return;
            }
            auto listener = EventListenerTouchOneByOne::create();
            listener->onTouchBegan = CC_CALLBACK_2(Game::TouchBegan, this);
            listener->onTouchEnded = CC_CALLBACK_2(Game::TouchEnded, this);
            this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
            // cocos2dxのEventListener
            _touchListener = listener;
        }
        else
        {
            // イベント削除
            this->getEventDispatcher()->removeEventListener(_touchListener);
            _touchListener = nullptr;
        }
    }
    
}

// 初期処理
bool Game::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    // タップイベントを取得する
    touchEnabled(true);
    
    // バックキー・メニューキーを取得する(androidのみ)
    setKeypadEnabled(true);
    
    // 変数初期化
    initForVariables();
    
    // 背景設定
    showBackground();

    // コマ表示
    showBlock();

    // ラベル作成
    showLabel();
    showHighScoreLabel();

    // リセットボタン作成
    showResetButton();
    
    // 効果音の事前読み込み
    SimpleAudioEngine::getInstance()->preloadEffect(MP3_REMOVE_BLOCK);
    
    return true;
}


// 背景表示
void Game::showBackground()
{
    // windowサイズ取得
    Size winSize = Director::getInstance()->getWinSize();
    
    m_background = Sprite::create(PNG_BACKGROUND);
    m_background->setPosition(Point(winSize.width / 2, winSize.height / 2));

    this->addChild(m_background, kZOrderBackground, kTagBackground);
}

// 変数初期化
void Game::initForVariables()
{
    // 乱数初期化
    srand((unsigned)time(NULL));

    // コマの一辺の長さを取得
    BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlockRed);
    m_blockSize = pBlock->getContentSize().height;
    
    // コマ種類の配列生成
    blockTypes.push_back(kBlockRed);
    blockTypes.push_back(kBlockBlue);
    blockTypes.push_back(kBlockYellow);
    blockTypes.push_back(kBlockGreen);
    blockTypes.push_back(kBlockGray);
    
    // 変数初期化(アニメーション中かの変数)
    m_animating = false;
    m_score = 0;
}

// コマの位置を取得する
Point Game::getPosition(int posIndexX, int posIndexY)
{
    float offsetX = m_background->getContentSize().width * 0.168;
    float offsetY = m_background->getContentSize().height * 0.029;
    return Point((posIndexX + 0.5) * m_blockSize + offsetX, (posIndexY + 0.5) * m_blockSize + offsetY);
}

// コマのタグを取得
int Game::getTag(int posIndexX, int posIndexY)
{
    return kTagBaseBlock + posIndexX * 100 + posIndexY;
}

// コマを表示させる
void Game::showBlock()
{
    // 8 × 8 のコマを作成する
    for (int x = 0; x < MAX_BLOCK_X; x++)
    {
        for (int y = 0; y < MAX_BLOCK_Y; y++)
        {
            // ランダムでコマを作成
            kBlock blockType = (kBlock)(rand() % kBlockCount);
            // 対応するタグを取得
            int tag = getTag(x, y);
            // コマ配列にタグを追加
            m_blockTags[blockType].push_back(tag);
            // コマを作成
            BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
            // コマの位置を設定
            pBlock->setPosition(getPosition(x, y));
            // コマを背景のスプライトに組み込み
            m_background->addChild(pBlock,kZOrderBlock, tag);
        }
    }
}

// タップ開始時に処理される
bool Game::TouchBegan(Touch* pTouch, Event* pEvent)
{
    // アニメーション中は開始できない(m_animatingがtrueはアニメーション中)
    return !m_animating;
}

// タップ終了時に処理される
void Game::TouchEnded(Touch* pTouch, Event* pEvent)
{
    // タップポイント取得
    Point touchPoint = m_background->convertTouchToNodeSpace(pTouch);
    // タップしたコマのTagとコマの種類を取得
    int tag = 0;
    kBlock blockType;
    // タップした位置のコマを取得
    getTouchBlockTag(touchPoint, tag, blockType);
    
    if (tag != 0) {
        //隣接するコマを検索する
        list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);
        
        if (sameColorBlockTags.size() > 1) {
            // 得点加算(消したコマ -2)の2乗
            // powは何乗
            m_score += pow(sameColorBlockTags.size()-2, 2);
            
            // アニメーション開始
            m_animating = true;
            // 隣接するコマを削除する
            removeBlock(sameColorBlockTags, blockType);
            // コマ削除後のアニメーション
            movingBlocksAnimation1(sameColorBlockTags);
        }
    }
}

// タップされたコマのタグを取得
void Game::getTouchBlockTag(Point touchPoint, int &tag, kBlock &blockType)
{
    for (int x = 0; x < MAX_BLOCK_X; x++) {
        for (int y = 0; y < MAX_BLOCK_Y; y++) {
            // タグを取得
            int currentTag = getTag(x, y);
            // コマを取得
            Node* node = m_background->getChildByTag(currentTag);
            // タッチ判定
            // コマが存在する && タップしたポイントにコマがある
            if (node && node->boundingBox().containsPoint(touchPoint)) {
                // コマのタグを設定
                tag = currentTag;
                // コマの種類を設定
                blockType = ((BlockSprite*)node)->getBlockType();
                // 結果を返す
                return;
            }
        }
    }
}

// コマ配列にあるか検索 ある場合=>true
bool Game::hasSameColorBlock(list<int> blockTagList, int searchBlockTag)
{
    list<int>::iterator it;
    for (it = blockTagList.begin(); it != blockTagList.end(); ++it) {
        if (*it == searchBlockTag) {
            return true;
        }
    }
    return false;
}

// タップされたコマと同色で且つ接しているコマの配列を返す
list<int> Game::getSameColorBlockTags(int baseTag, kBlock blockType)
{
    // 同色のコマを格納する配列の初期化
    list<int> sameColorBlockTags;
    // 配列に追加
    sameColorBlockTags.push_back(baseTag);
    
    list<int>::iterator it = sameColorBlockTags.begin();
    while (it != sameColorBlockTags.end()) {
        int tags[] = {
            *it + 100, // right block tag
            *it - 100, // left block tag
            *it + 1, // up block tag
            *it -1, // down block tag
        };
        
        for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++) {
            // すでにリストにあるか検索
            if (!hasSameColorBlock(sameColorBlockTags, tags[i])) {
                // コマ配列にあるか検索
                if (hasSameColorBlock(m_blockTags[blockType], tags[i])) {
                    sameColorBlockTags.push_back(tags[i]);
                }
            }
        }
        it ++;
    }
    return sameColorBlockTags;
}

// 配列のコマを削除
void Game::removeBlock(list<int> blockTags, kBlock blockType)
{

    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end()) {
        // 既存配列から該当コマを削除
        m_blockTags[blockType].remove(*it);
        // 対象となるコマを取得
        Node* block = m_background->getChildByTag(*it);
        if (block) {
            //コマが消えるアニメーションを生成(コマを小さくする)
            ScaleTo* scale = ScaleTo::create(REMOVING_TIME, 0);
            
            //コマを削除するアクションを生成
            CallFuncN* func = CallFuncN::create(CC_CALLBACK_1(Game::removingBlock, this));

            // アクションをつなげる
            FiniteTimeAction* sequence = Sequence::create(scale, func, NULL);
            
            FiniteTimeAction* action;
            
            action = sequence;
            
            // アクションをセットする
            block->runAction(action);
        }
        it++;
    }
    // 効果音を再生
    SimpleAudioEngine::getInstance()->playEffect(MP3_REMOVE_BLOCK);
}

// コマの削除
void Game::removingBlock(Node* block)
{
    // 呼び出し元のNodeクラスを削除
    block->removeFromParentAndCleanup(true);
}


// コマのインデックスを取得
Game::PositionIndex Game::getPositionIndex(int tag)
{
    int pos1_x = (tag - kTagBaseBlock) / 100;
    int pos1_y = (tag - kTagBaseBlock) % 100;
    return PositionIndex(pos1_x, pos1_y);
}

// コマの新しい位置をセット
void Game::setNewPosition1(int tag, PositionIndex posIndex)
{
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosY = blockSprite->getNextPosY();
    if (nextPosY == -1)
    {
        nextPosY = posIndex.y;
    }
    
    // 移動先の位置をセット
    blockSprite->setNextPos(posIndex.x, --nextPosY);
}

// 消えたコマを埋めるように新しい位置をセット(引数は削除されるコマのタグ一覧)
void Game::searchNewPosition1(list<int> blocks)
{
    // 削除されるコマのイテレーター
    list<int>::iterator it1 = blocks.begin();
    // 削除されるコマのイテレーターのループ
    while (it1 != blocks.end()) {
        // 位置の情報のインデックスを取得
        PositionIndex posIndex1 = getPositionIndex(*it1);
        
        // コマ種類のイテレーター
        vector<kBlock>::iterator it2 = blockTypes.begin();
        // コマ種類のイテレーターのループ
        while (it2 != blockTypes.end()) {
            // 各コマの種類のコマ一覧のイテレーター
            list<int>::iterator it3 = m_blockTags[*it2].begin();
            // 各コマ種の全コマに対してループ
            while (it3 != m_blockTags[*it2].end()) {
                // 残っているコマのインデックスを取得
                PositionIndex posIndex2 = getPositionIndex(*it3);
                // 削除されるコマの上方向に位置するかの判断
                if (posIndex1.x == posIndex2.x && posIndex1.y < posIndex2.y) {
                    // 消えるコマの上に位置するコマに対して、移動先の位置をセットする
                    setNewPosition1(*it3, posIndex2);
                }
                it3++;
            }
            it2++;
        }
        it1++;
    }
}

// コマを移動する (移動するコマに対して移動先の情報をセットし、アクションをセット)
void Game::moveBlock()
{
    // コマ種類のイテレーター
    vector<kBlock>::iterator it1 = blockTypes.begin();
    // コマ種類のループ
    while (it1 != blockTypes.end()) {
        //各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            // コマの取得
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(*it2);
            // 移動先のインデックス取得
            int nextPosX = blockSprite->getNextPosX();
            int nextPosY = blockSprite->getNextPosY();
            // 移動先がいるかの判定
            if (nextPosX != -1 || nextPosY != -1) {
                // 移動先のインデックスより新しいタグを生成
                int newTag = getTag(nextPosX, nextPosY);
                // 移動先の情報の初期化
                blockSprite->initNextPos();
                // コマに新しいタグをセット
                blockSprite->setTag(newTag);
                
                // タグ一覧の値も新しいタグに変更する
                *it2 = newTag;
                // アニメーションを生成
                MoveTo* move = MoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
                // アニメーションをセット
                blockSprite->runAction(move);
            }
            it2++;
        }
        it1++;
    }
}

// コマ削除後のアニメーション
void Game::movingBlocksAnimation1(list<int> blocks)
{
    // コマの新しい位置をセットする
    searchNewPosition1(blocks);
    //新しい位置がセットされたコマのアニメーション
    moveBlock();
    // アニメーション終了時に次のアニメーション処理を開始する
    scheduleOnce(schedule_selector(Game::movingBlocksAnimation2), MOVING_TIME);
}

// コマの移動完了
void Game::movedBlocks()
{
    // ラベル再表示
    showLabel();
    // アニメーション終了
    m_animating = false;
    
    // ゲームオーバー判定
    if (!existsSameBlock()) {
        // ハイスコアの更新・表示
        saveHighScore();
        
        Size bgSize = m_background->getContentSize();
        // ゲーム終了表示
        Sprite* gameOver = Sprite::create(PNG_GAMEOVER);
        gameOver->setPosition(Point(bgSize.width / 2, bgSize.height * 0.8));
        m_background->addChild(gameOver,kZOrderGameOver,kTagGameOver);
 
        // タップイベント削除
        touchEnabled(false);
    }
}

// 新しい位置をセット
void Game::setNewPosition2(int tag, PositionIndex posIndex)
{
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosX = blockSprite->getNextPosX();
    if (nextPosX == -1) {
        nextPosX = posIndex.x;
    }
    
    // 移動先の位置をセット
    blockSprite->setNextPos(--nextPosX, posIndex.y);
}

// 存在する列を取得する
map<int, bool> Game::getExistBlockColumn()
{

    // 戻り値になる配列(keyがint valueがbool)  xインデックスに対するコマの存在有無
    map<int, bool> xBlock;
    // 検索配列初期化
    for (int i = 0; i < MAX_BLOCK_X; i++) {
        xBlock[i] = false;
    }
    
    // コマの種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分ループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            // 存在するコマのx位置を記録
            xBlock[getPositionIndex(*it2).x] = true;
            it2++;
        }
        it1++;
    }
    return xBlock;
}

// 消えたコマ列を埋めるように新しい位置をセット
void Game::searchNewPosition2()
{
    // 存在する列を取得
    map<int, bool> xBlock = getExistBlockColumn();
    
    // コマが存在しないx位置を埋める
    bool first = true;
    for (int i = MAX_BLOCK_X-1; i >= 0; i--) {
        if (xBlock[i]) {
            // コマが存在する
            first = false;
        }
        else
        {
            // コマが存在しない
            if (first) {
                // 右側にコマがない
                continue;
            }
            else
            {
                // この位置より右側にあるコマを左に1つよせる
                
                // コマ種類のループ
                vector<kBlock>::iterator it1 = blockTypes.begin();
                while (it1 != blockTypes.end()) {
                    // 各種類のコマ数分のループ
                    list<int>::iterator it2 = m_blockTags[*it1].begin();
                    while (it2 != m_blockTags[*it1].end()) {
                        PositionIndex posIndex = getPositionIndex(*it2);
                        
                        if (i < posIndex.x) {
                            // 移動先の位置をセットする
                            setNewPosition2(*it2, posIndex);
                        }
                        it2++;
                    }
                    it1++;
                }
            }
        }
    }
}

// コマのアニメーション
void Game::movingBlocksAnimation2(float frame)
{
    // コマの新しい位置をセットする
    searchNewPosition2();
    //新しい位置がセットされたコマのアニメーション
    moveBlock();
    // アニメーション終了時に次の処理を開始する
    scheduleOnce([this](float dt){
        movedBlocks();
    }, MOVING_TIME, "kokohatekitou");
}

// ラベルの初期表示
void Game::showLabel()
{
    Size bgSize = m_background->getContentSize();
    
    // 残数表示
    // ラベルのタグ
    int tagsForLabel[] = {kTagRedLabel, kTagBlueLabel, kTagYellowLabel, kTagGreenLabel, kTagGrayLabel};
    // フォントファイル
    const char* fontNames[] = {FONT_RED, FONT_BLUE, FONT_YELLOW, FONT_GREEN, FONT_GRAY};
    // 表示位置(高さ)
    float heightRate[] = {0.61, 0.50, 0.39, 0.28, 0.17};

    // コマ種類のループ
    vector<kBlock>::iterator it = blockTypes.begin();
    while (it != blockTypes.end()) {
        // コマ残数表示
        // コマのタグを種類毎に保持している配列のサイズを取得
        int count = m_blockTags[*it].size();
        // コマ残数をstring型にする config.hで定義済み
        const char* countStr = ccsf("%02d", count);

        // ラベルのタグよりインスタンスを取得 (アプリ起動直後はない)
        Label* label = (Label*)m_background->getChildByTag(tagsForLabel[*it]);
        
        // タグが取得できない場合true (アプリ起動直後)
        if (!label)
        {
            // 起動直後に通る
            // コマ残数生成 ラベル作成
            label = Label::createWithBMFont(fontNames[*it], countStr);
            label->setPosition(Point(bgSize.width * 0.8, bgSize.height * heightRate[*it]));
            m_background->addChild(label, kZOrderLabel, tagsForLabel[*it]);
            
            // 画像のタグよりインスタンスを取得
            kBlock blockType = (kBlock)(*it);
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(blockType);
            // スプライト(図形や画像)を作成（画像などを指定）
            auto sprite = Sprite::create(blockSprite->getBlockImageFileName(blockType));
            sprite->setPosition(Point(bgSize.width * 0.74, bgSize.height * heightRate[*it]));
            // 画像を背景のスプライトに組み込み
            m_background->addChild(sprite,kZOrderLabel, blockType);
            
        }
        else
        {
            // 文字列の更新
            label->setString(countStr);
        }
        it++;
    }
    // スコアを表示
    // スコアのstringを取得
    const char* scoreStr = ccsf("%02d", m_score);
    // ラベルのタグよりインスタンスを取得
    Label* scoreLabel = (Label*)m_background->getChildByTag(kTagScoreLabel);
    if (!scoreLabel) {
        // スコアラベル生成
        scoreLabel = Label::createWithBMFont(FONT_WHITE, scoreStr);
        // 場所決め
        scoreLabel->setPosition(Point(bgSize.width * 0.78, bgSize.height * 0.75));
        // 配置
        m_background->addChild(scoreLabel,kZOrderLabel,kTagScoreLabel);
    }
    else
    {
        // string型のスコアを変更
        scoreLabel->setString(scoreStr);
    }
    
}


bool Game::existsSameBlock()
{
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            if (getSameColorBlockTags(*it2, *it1).size() > 1) {
                // 隣り合うコマが存在するため true
                return true;
            }
            it2++;
        }
        it1++;
    }
    // 隣り合うコマがなかったため false
    return false;
}


// ハイスコアの保存
void Game::saveHighScore()
{
    UserDefault* userDefault = UserDefault::getInstance();
    // ハイスコアを取得する
    int oldHighScore = userDefault->getIntegerForKey(KEY_HIGHSCORE, 0);
    // 過去のと比較
    if (oldHighScore < m_score) {
        // ハイスコアを保持する 更新
        userDefault->setIntegerForKey(KEY_HIGHSCORE, m_score);
        // ハイスコアのセーブ
        userDefault->flush();
        
        // ハイスコアを表示する
        showHighScoreLabel();
    }
}


// ハイスコアラベル表示
void Game::showHighScoreLabel()
{
    Size bgSize = m_background->getContentSize();
    
    // ハイスコア表示 getIntegerForkeyで値を取得 引数1はキー 引数2はキーに対する値がない時の返り値
    int highScore = UserDefault::getInstance()->getIntegerForKey(KEY_HIGHSCORE, 0);
    const char* highScoreStr  = ccsf("%d", highScore);
    Label* highScoreLabel = (Label*)m_background->getChildByTag(kTagHighScoreLabel);
    if (!highScoreLabel) {
        // ハイスコア生成
        highScoreLabel = Label::createWithBMFont(FONT_WHITE, highScoreStr);
        highScoreLabel->setPosition(Point(bgSize.width * 0.78, bgSize.height * 0.87));
        m_background->addChild(highScoreLabel,kZOrderLabel,kTagHighScoreLabel);
    }
    else
    {
        // ハイスコア更新
        highScoreLabel->setString(highScoreStr);
    }
}

// リセットボタンの処理
void Game::menuResetCallback(Ref *pSender)
{
    // シーンを置き換え
    Director::getInstance()->replaceScene(Game::createScene());
}

// リセットボタンの作成
void Game::showResetButton()
{
    Size bgSize = m_background->getContentSize();
    
    // リセットボタン作成
    MenuItemImage* resetButton = MenuItemImage::create(PNG_RESET, PNG_RESET, CC_CALLBACK_1(Game::menuResetCallback, this));
    resetButton->setPosition(Point(bgSize.width * 0.78, bgSize.height * 0.06));
    
    // メニュー作成
    Menu* menu = Menu::create(resetButton, NULL);
    menu->setPosition(Vec2::ZERO);
    m_background->addChild(menu);
}
