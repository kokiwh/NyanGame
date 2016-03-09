#ifndef __PLAY_SE_H__
#define __PLAY_SE_H__

USING_NS_CC;
using namespace std;

class PlaySE : public ActionInstant
{
public:
    PlaySE(string sound);
    virtual ~PlaySE(){}

    // update関数に処理を記述
    virtual void update(float time);
    virtual FiniteTimeAction* reverse(void);
    virtual Object* copyWithZone(Zone* pZone);
    
public:
    // create関数
    static PlaySE* create(string sound);
    // ファイル名を保持するメンバ変数
    string m_sound;
};

#endif // __PLAY_SE_H__
