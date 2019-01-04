/**
* @file     sclLabel.cpp
*           label部品 方法实现文件
*
* @author   Tupelo
* @version  V1.01.01    2018/12/29
*/
#include <string.h>
#include "sclLabel.h"
#include "sclColor.h"
#include "sclDrawCom.h"
#include "sclUtil.h"

// 类信息记录
M_IMPLEMENT_RUNTIME(SCLabel, SCUpdateTypeNONE, SCParts)

SCLabel::SCLabel(
const short id,
const SCRect& area,
unsigned short font_type,
unsigned char fore_color,
unsigned char back_color,
unsigned int work_scr,
DRAW_MODE draw_mode
) : SCParts(id, area, fore_color, back_color, work_scr), 
    m_font_type(font_type), 
    m_targetID(-1), 
    m_arrangement(SC_ARRANGE_CENTER),
    m_drawMode(draw_mode)
{
    memset(m_label, 0x0000, sizeof(m_label));
}

SCLabel::~SCLabel()
{
    ;
}

/****************************************************************************/
/**
 * @brief           drawCapture()
 *                  draw Label capture
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   unsigned short**    com_pos     描画命令保存位置
 *
 * @return  int     填充的描画命令数
 */
/****************************************************************************/
int SCLabel::drawCapture(unsigned short** com_pos)
{
    int com_num = 0;
    SCDRAWPARAM     param;

    /* 描画背景 */
    if(m_back_color != COLOR_TRANSPARENT)
    {
        param.rect       = m_area;                // 塗りつぶし領域
        param.work_scr   = m_work_scr;            // 表示プレーン(更新用)指定
        param.fill       = true;
        param.back_color = m_back_color;
        param.work_scr   = m_work_scr;
        param.frame      = false;
        com_num          = SCDrawCom::makeRect(param, *com_pos);
        *com_pos        += com_num;
    }

    param.rect = m_area;                    // 塗りつぶし領域
    param.work_scr = m_work_scr;            // 表示プレーン(更新用)指定
    param.arrangement = this->m_arrangement;
    param.distance = 0;                     // 字間
    param.font_type = m_font_type;          // フォントタイプ
    param.fore_color = m_fore_color;        // 文字の色
    param.back_color = COLOR_TRANSPARENT;   // 背景色 <- Mitomo Modified 透明色固定

    if(m_targetID < 0)                      // Mitomo modified 固定文字列未使用時は、m_targetIDが負の値
    {
        param.str = m_label;                    // 表示文字列

        // 指定フォントサイズで描画
        if(m_drawMode == DRAWMODE_DIRECT)
        {
            com_num += SCDrawCom::makeFreeString(param, *com_pos);
        }
        // 言語によってフォントサイズ自動変更
        else
        {
            com_num += SCDrawCom::makeFreeStringReal(param, *com_pos);
        }
    }
    else
    {
        param.target_ID = m_targetID;           // 表示文字列

        // 指定フォントサイズで描画
        if(m_drawMode == DRAWMODE_DIRECT)
        {
            com_num += SCDrawCom::makeFixedString(param, *com_pos);
        }
        // 言語によってフォントサイズ自動変更
        else
        {
            com_num += SCDrawCom::makeFixedStringReal(param, *com_pos);
        }
    }

    return com_num;
}
/****************************************************************************/
/**
 * @brief           setStr()
 *                  设置Label上显示的字符串
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in]       target_id 根据字符串ID设置
 *
 * @return          void
 */
/****************************************************************************/
void SCLabel::setStr(int target_id)
{
    m_targetID = target_id;
}

/****************************************************************************/
/**
 * @brief           setStr()
 *                  设置Label上显示的字符串
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in]       mes 常量字符串
 *
 * @return          void
 */
/****************************************************************************/
void SCLabel::setStr(const char* mes)
{
    int i = 0;

    while (mes[i] != '\0' && i < SC_LABEL_STRING_MAX)
    {
        this->m_label[i] = static_cast<unsigned short>(mes[i]);
        ++i;
    }
    this->m_label[i] = 0x0000;

    m_targetID = -1;    // Mitomo add: 任意文字列がセットされたときは-1とする
}

/****************************************************************************/
/**
 * @brief           setStr()
 *                  设置Label上显示的字符串
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in]       mes 常量字符串
 *
 * @return          void
 */
/****************************************************************************/
void SCLabel::setStr(const unsigned short* mes)
{
    int i = 0;

    while (mes[i] != 0x0000 && i < SC_LABEL_STRING_MAX)
    {
        this->m_label[i] = mes[i];
        ++i;
    }
    this->m_label[i] = 0x0000;

    m_targetID = -1;    // Mitomo add: 任意文字列がセットされたときは-1とする
}

/****************************************************************************/
/**
 * @brief           setDrawMode()
 *                  设置描画模式
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in]       setDrawMode 描画模式
 *
 * @return          void
 */
/****************************************************************************/
void SCLabel::setDrawMode(DRAW_MODE drawMode)
{
    m_drawMode = drawMode;
}

/****************************************************************************/
/**
 * @brief           Draw()
 *                  初始描画
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   unsigned short**    com_pos     描画命令保存位置
 *
 * @return  int     填充的描画命令数
 */
/****************************************************************************/
int SCLabel::Draw(unsigned short* com_pos)
{
    int com_num = 0;

    // 判断是否可见
    if(GetVisible() == false) {
        return(0);
    }

    com_num += drawCapture(&com_pos);

    return com_num;
}
