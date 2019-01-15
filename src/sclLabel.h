/**
* @file     sclLabel.h
*           label部品头文件
*
* @author   Tupelo
* @version  V1.01.01    2018/12/29
*/
#ifndef __SCL_LABEL_H__
#define __SCL_LABEL_H__

#include "sclParts.h"

/*
 * Label class 使用，用于限定Label所有的字符串的字符个数
 */
#define SC_LABEL_STRING_MAX     (60)

/*
 * Label class declaration
 */
class SCLabel : public SCParts
{
    M_DECLARE_RUNTIME(SCLabel)

    typedef enum
    {
        DRAWMODE_DIRECT = 0,    // 按照指定字体大小描画字符串
        DRAWMODE_REAL,          // 根据语言的字体大小自动变更
    }DRAW_MODE;

public:
    SCLabel( const short id, const SCRect& area,
            unsigned short  font_type   = SC_PARTS_FONT_TYPE_SFONT/* SC_PARTS_FONT_TYPE_MFONT */,
            unsigned char   fore_color  = COLOR_SKY_BLUE/* COLOR_BLACK */,
            unsigned char   back_color  = COLOR_TRANSPARENT,
            unsigned int    work_scr    = SC_UPDATE_WORK,
            DRAW_MODE       draw_mode   = DRAWMODE_DIRECT);
    virtual ~SCLabel();

    virtual int             Draw(unsigned short* com);
    
    void                    setStr(const char* mes);
    void                    setStr(const unsigned short* mes);
    void                    setStr(int target_id);
    void                    setDrawMode(DRAW_MODE draw_mode);
    void                    arrangement(int arr){this->m_arrangement = arr;};
    int                     drawCapture(unsigned short** com_pos);

private:
    unsigned short          m_font_type;
    unsigned short          m_label[SC_LABEL_STRING_MAX + 1];
    int                     m_targetID;
    int                     m_arrangement;
    DRAW_MODE               m_drawMode;     // 字符串描画模式
};

#endif /* __SCL_LABEL_H__ */
