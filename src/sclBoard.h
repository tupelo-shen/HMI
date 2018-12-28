/**
* @file sclBoard.h
*           SCBoard
*
* @author   
* @version
*/
#ifndef __SCL_BOARD_H__
#define __SCL_BOARD_H__

#include "sclCore.h"

#define SCBoardScreenIDNONE (-1)

// 与SCBoard互相引用
class SCParts;

/*
 * Boardクラス定義
 */
class SCBoard : public SCCore 
{
    M_DECLARE_RUNTIME(SCBoard)
public :
    SCBoard(const SCRect& area, const short sID = SCBoardScreenIDNONE);
    virtual ~SCBoard();

    virtual int     Draw(unsigned short* com);
    virtual int     Update(const int updateType, unsigned short* com);
    void            Display(int comNum, unsigned short* cmd) const;
    void            FlushDrawCom(int comNum, unsigned short* cmd) const;
    virtual int     DrawBackground(const SCRect& area, unsigned short* com);
    virtual void    Initialize(void);
    virtual void    ReDrawDisplay(void);
    virtual void    UpdateDisplay(const int updateType);
    virtual void    TDown(const unsigned short x, const unsigned short y);
    virtual void    TMove(const unsigned short x, const unsigned short y,
                        const unsigned int level, const int direction);
    virtual void    TUp(const unsigned short x, const unsigned short y);
    virtual void    TFlick(const unsigned short x, const unsigned short y,
                        const unsigned int level, const int direction);
    virtual void    TLongTap(const unsigned short x, const unsigned short y);
    virtual bool    IsOKtoMove(void){return(true);}
    SCParts*        GetChild(void) const {return(m_child);} // 返回管理的子部品
    SCParts*        GetChild(const unsigned short x,
                        const unsigned short y) const;
    SCParts*        GetChild(const SCPoint& point) const;   // 返回指定位置的子部品
    SCParts*        GetChild(const short id) const;         // 返回指定ID的子部品
    SCParts*        GetLastChild(void) const;               // 返回最后一个注册的子部品
    SCParts*        GetOperableChild(const unsigned short x,
                        const unsigned short y) const;
    SCParts*        GetOperableChild(const SCPoint& point) const;
    void            RegistParts(SCParts*);                  // 注册子部品
    void            UnregistParts(SCParts*);                // 解除子部品的注册
    void            DestroyParts(SCParts*);                 // 销毁子部品
    void            DestroyAllParts(void);

    virtual unsigned short  GetBackColor(void);
    short           GetScreenID(void) {return(m_screenID);}     // 取得画面ID
    SCBoard*        GetDialog(void) {return(m_dialog);}
    void            SetDialog(SCBoard* dialog) {m_dialog = dialog;}
    bool            IsDialog(void);
    SCBoard*        GetParent(void) {return(m_parent);}
    void            SetParent(SCBoard* parent) {m_parent = parent;}
    bool            GetModal(void) {return(m_fModal);}
    void            SetModal(bool modal) {m_fModal = modal;}
    SCBoard*        GetActiveBoard(void);
    void            RegistDialog(SCBoard* dialog);
    void            DestroyDialog(void);
    void            DestroyAllDialog(void);
    SCPoint         GetTouchPoint(void){return(m_touch_point);}
    unsigned int    GetTouchLevel(void){return(m_touch_level);}
    int             GetTouchDirection(void){return(m_touch_direction);}
    void            CreatePassReleaseDlg(int partsID, int passMode, void (*funcPassRelease)(int));
    void            CreateRunningDlg(void);
protected :
    SCParts*        m_child;            // 管理的子部品
    SCParts*        m_captured;         // 通知需要touch处理的子部品
    SCBoard*        m_dialog;           // 对话框
    SCBoard*        m_parent;           // 对话框的父画面
    bool            m_fModal;           // 模式标志(对话框)
    short           m_screenID;         // 画面ID

    SCPoint         m_touch_point;      // touch事件 位置(Callback处理使用)
    unsigned int    m_touch_level;      // touch事件 level(Callback处理使用)
    int             m_touch_direction;  // touch事件 方向(Callback处理使用)
};
#endif /* __SCL_BOARD_H__ */
