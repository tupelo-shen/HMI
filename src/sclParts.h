// Copyright (c) 2016 Yokogawa Electric Corporation. All rights reserved.
/**
* @file sclParts.h
* SCPartsクラスヘッダ
*
* @author   A.Kubota
* @version  R1.01.01    2016/03/07  A.Kubota    New
*/

#ifndef _SCL_PARTS_H_
#define _SCL_PARTS_H_

#include "sclCore.h"
#include "sclBoard.h"
#include "sclCallback.h"
#include "sclDrawCom.h"
#include "sclColor.h"


/*
 * Partsクラス用定義
 */
enum {
    SC_PARTS_FONT_TYPE_LFONT,   // = SC_FONT_TYPE_L(0)
    SC_PARTS_FONT_TYPE_MFONT,   // = SC_FONT_TYPE_M(1)
    SC_PARTS_FONT_TYPE_SFONT,   // = SC_FONT_TYPE_S(2) sclDrawCom.h
    SC_PARTS_FONT_TYPE_XLPICT,  // BIGFONT_TYPEの順と合わせる sclDrawCom.h
    SC_PARTS_FONT_TYPE_LPICT,
    SC_PARTS_FONT_TYPE_MPICT,
    SC_PARTS_FONT_TYPE_SPICT,
    SC_PARTS_FONT_TYPE_XSPICT,
    SC_PARTS_FONT_TYPE_EMCLPICT,
    SC_PARTS_FONT_TYPE_EMCSPICT,
    SC_PARTS_FONT_TYPE_NUM
};


// 相互参照
class SCBoard;


/*
 * Partsクラス定義
 */
class SCParts : public SCCore {
    M_DECLARE_RUNTIME(SCParts)
public :
    SCParts(const short id, const SCRect& area,
            unsigned char fore_color = COLOR_BLACK,
            unsigned char back_color = COLOR_TRANSPARENT,
            unsigned int work_scr = SC_UPDATE_WORK);
    virtual ~SCParts() = 0;

    virtual int             Draw(unsigned short* com);      // エリアの指定は？
    int                     DrawNumString(SCDRAWPARAM& param,
                                unsigned short* buf);
    int                     DrawTagString(SCDRAWPARAM& param,
                                unsigned short* buf);
    int                     DrawUnitString(SCDRAWPARAM& param,
                                unsigned short* buf);
    virtual int             DrawBackground(unsigned short* para);
    virtual int             Update(const int updateType, unsigned short* para); // エリアの指定は？ トレンド更新は分けるか？
    virtual void            Initialize(){return;}
    virtual void            ReDrawDisplay();
    void                    AddCallback(SCCallback* cb);
    void                    DoCallback(int type);
    void                    RemoveAllCallbacks();

    virtual void            TDown(const SCPoint& point);
    virtual void            TMove(const SCPoint& point,
                                const unsigned int level, const int direction);
    virtual void            TUp(const SCPoint& point);
    virtual void            TFlick(const SCPoint& point,
                                const unsigned int level, const int direction);
    virtual void            TLongTap(const SCPoint& point);

    SCBoard*                Parent() const {return(m_parent);}
    void                    Parent(SCBoard* parent) {m_parent = parent;}
    SCParts*                Next() const {return(m_next);}
    void                    Next(SCParts* next) {m_next = next;}
    SCParts*                Prev() const {return(m_prev);}
    void                    Prev(SCParts* prev) {m_prev = prev;}

    unsigned int            GetWorkScr() const {return(m_work_scr);}
    void                    SetWorkScr(unsigned int work_scr)
                                {m_work_scr = work_scr;}
    short                   GetID() const {return(m_id);}
    bool                    GetVisible() const {return(m_fVisible);}
    void                    SetVisible(bool visible) {m_fVisible = visible;}
    bool                    GetEnable() const {return(m_fEnable);}
    void                    SetEnable(bool enable) {m_fEnable = enable;}
    bool                    GetPushed() const {return(m_fPushed);}
    void                    SetPushed(bool pushed) {m_fPushed = pushed;}
    unsigned char           GetForeColor() const {return(m_fore_color);}
    void                    SetForeColor(unsigned char fore_color)
                                {m_fore_color = fore_color;}
    unsigned char           GetBackColor() const {return(m_back_color);}
    void                    SetBackColor(unsigned char back_color)
                                {m_back_color = back_color;}

    virtual unsigned short  GetFontType(){return(SC_PARTS_FONT_TYPE_LFONT);}

protected :
    SCBoard*        m_parent;       // 自分を管理している親部品(画面)
    SCParts*        m_next;         // 登録順の次の部品
    SCParts*        m_prev;         // 登録順の前の部品
    SCParts*        m_oTrend;       // 重なっているトレンド部品
    SCCallback*     m_callback;     // コールバック(リスト)
    unsigned int    m_work_scr;     // 描画する裏画面
    short           m_id;           // 部品ID(画面内でユニーク)
    bool            m_fVisible;     // 可視
    bool            m_fEnable;      // 選択可
    bool            m_fPushed;      // 押下中
    unsigned char   m_fore_color;   // 前景色
    unsigned char   m_back_color;   // 背景色
};

#endif  // _SCL_PARTS_H_    end

