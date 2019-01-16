// Copyright (c) 2016 Yokogawa Electric Corporation. All rights reserved.
/**
* @file sclButton.cpp
* SCButtonクラス
*
*/


#include <string.h>
#include "sclColor.h"
#include "sclButton.h"
#include "sclDrawCom.h"
#include "HMIControl.h"

// class info
M_IMPLEMENT_RUNTIME(SCButton, SCUpdateTypeNONE, SCLabel)
// M_IMPLEMENT_RUNTIME(SCButton, SCUpdateTypeCYCLIC, SCLabel)

/**
 * @brief   constructor
 *
 * @param[in]   id              button id
 * @param[in]   area            button rect
 * @param[in]   font_type       font type
 * @param[in]   transparent     transparent switch(normal.transparent)
 */
SCButton::SCButton(
    const short id,
    const SCRect& area,
    unsigned char fore_color,
    unsigned char back_color,
    unsigned int work_scr,
    unsigned short font_type,
    bool transparent
) : SCLabel(id, area, font_type, fore_color, back_color, work_scr), 
    m_transparent(transparent)
{
    m_fEnable = true;
}

/**
 * @brief   destructor
 */
SCButton::~SCButton()
{
    ;
}

/**
 * @brief   get button color
 *
 * @return button color
 */
unsigned char SCButton::getButtonColor()
{
    unsigned char color;

    if (m_fEnable == false)
    {
        color = COLOR_GRAY_EE;   // 薄い灰色
        return color;
    }

    if(!Parent()->IsDialog())
    {
        // 親画面：通常画面の場合
        if(GetPushed())
        {
            color = COLOR_GRAY_CC;   // 押下時色：濃い灰色
        }else
        {
            color = COLOR_WHITE;     // 通常色：白
        }
    }else
    {
        // 親画面：DIALOG画面の場合：
        if(GetPushed())
        {
            color = COLOR_GRAY_CC;   // 押下時色：濃い灰色
        }else
        {
            color = COLOR_GRAY_EE;   // 通常色：薄い灰色
        }
    }

    return color;
}

/**
 * @brief   draw button rect
 *
 * @param[out]  com_pos     描画コマンド格納アドレス
 * @return      作成した描画コマンド数
 */
int SCButton::drawRect(unsigned short** com_pos)
{
    int             com_num;
    SCDRAWPARAM     param;

    // 背景塗りつぶし
    param.fill = true;                 // 塗りつぶしあり
    param.back_color = getButtonColor();
    param.work_scr = m_work_scr;    // 表示プレーン(更新用)指定
    param.rect = m_area;                // 塗りつぶし領域
    param.frame = true;                 // 枠線あり
    param.fore_color = COLOR_BLACK;
//  com_num = SCDrawCom::makeFillRect(SC_UPDATE_WORK, 0, m_area, *com_pos);
    com_num = SCDrawCom::makeRect(param, *com_pos);
    *com_pos += com_num;
    return com_num;
}

/**
 * @brief   draw button underline
 *
 * @param[out]  com_pos     // 描画コマンド格納アドレス
 * @return      作成した描画コマンド数
 */
int SCButton::drawUnderLine(unsigned short** com_pos)
{
    SCDRAWPARAM     param;

    param.work_scr = m_work_scr;
    param.fore_color = COLOR_BLACK;
    SCRect r;
    r.Set(m_area.x, m_area.y+m_area.height-2, m_area.width, 1/*height*/);
    param.rect = r;

    int com_num = SCDrawCom::makeLine(param, *com_pos);
    *com_pos += com_num;

    return com_num;
}

/**
 * @brief   draw button sideline
 *
 * @param[out]  com_pos     // 描画コマンド格納アドレス
 * @return      作成した描画コマンド数
 */
int SCButton::drawSideLine(unsigned short** com_pos)
{
    SCDRAWPARAM     param;

    param.work_scr = m_work_scr;
    param.fore_color = COLOR_BLACK;
    SCRect r;
    r.Set(m_area.x + m_area.width, m_area.y, 2, m_area.height);
    param.rect = r;

    int com_num = SCDrawCom::makeLine(param, *com_pos);
    *com_pos += com_num;

    return com_num;
}

/**
 * @brief   draq button (virtual)
 *
 * @param[out]  unsigned short*     com     // 描画コマンド格納アドレス
 *
 * @return      int     作成した描画コマンド数
 */
int SCButton::Draw(unsigned short* com_pos)
{
    int             com_num = 0;// return value

    // 可視属性チェック
    if(GetVisible())
    {
        if(!m_transparent)
        {
            com_num += drawRect(&com_pos);      // 外枠
            com_num += drawUnderLine(&com_pos); // 下線
            //com_num += drawSideLine(&com_pos);  // 横線
            if (m_fEnable)
            {
                SetForeColor(COLOR_BLACK);
            }
            else
            {
                SetForeColor(COLOR_GRAY_CC);
            }
            com_num += drawCapture(&com_pos);   // 文字列
        }
        // 透明モード：ONの場合、ボタンを描画しない
    }else
    {
        // visible:offの場合、背景色で塗りつぶす
    }

    return com_num;
}

/**
 * @brief   button is pushed down (virtual)
 *
 * @param[in]  point pointer position
 *
 * @return      none
 */
void SCButton::TDown(const SCPoint& point)
{
    SetPushed(true);

    if(!m_transparent)
    {
        // 再描画
        ReDrawDisplay();
    }
}

void SCButton::redrawButton(const SCPoint& point)
{

}
/**
 * @brief   button is pushed (moving) (virtual)
 *
 * @param[in]  point pointer position
 * @param[in]  level
 * @param[in]  direction
 *
 * @return      none
 */
void SCButton::TMove(const SCPoint& point, const unsigned int level, const int direction)
{
    bool next = m_area.Contains(point);
    if(GetPushed() != next)
    {
        SetPushed( next );

        if(!m_transparent)
        {
            // 再描画
            ReDrawDisplay();
        }
    }
}

/**
 * @brief   button is pushed up (virtual)
 *
 * @param[in]  point pointer position
 *
 * @return      none
 */
void SCButton::TUp(const SCPoint& point)
{
    if(GetPushed())
    {
        SetPushed(false);

        if(!m_transparent)
        {
            // 再描画
            ReDrawDisplay();
        }

        DoCallback(SCCallbackTypeTAP);
    }
}

/**
 * @brief   button is flicked (virtual)
 *
 * @param[in]  point pointer position
 * @param[in]  level
 * @param[in]  direction
 *
 * @return      none
 */
void SCButton::TFlick(const SCPoint& point, const unsigned int level, const int direction)
{
    if(GetPushed())
    {
        SetPushed(false);

        if(!m_transparent)
        {
            // 再描画
            ReDrawDisplay();
        }

        DoCallback(SCCallbackTypeFLICK);
    }
}

/**
 * @brief   button is pushed long (virtual)
 *
 * @param[in]  point pointer position
 *
 * @return      none
 */
void SCButton::TLongTap(const SCPoint& point)
{
    if(GetPushed())
    {
        SetPushed(false);

        if(!m_transparent)
        {
            // 再描画
            ReDrawDisplay();
        }

        DoCallback(SCCallbackTypeLONGTAP);
    }
}

