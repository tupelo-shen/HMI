/**
* @file sclDrawCom.h
*           绘图类
*
* @author   
* @version  
*/

#ifndef __SCL_DRAW_COM_H__
#define __SCL_DRAW_COM_H__

#include "sclBase.h"

#define SC_DRAW_COM_MAX     (10240)     // 描画命令的最大数
#define SC_DRAW_COM_LM      (7680)      // Flash处理的决策值(最大值的3/4)

/* 指定内画面 */
#define SC_UPDATE_WORK      (0)         // 表示plane(更新用)
#define SC_TREND_WORK1      (1)         // trend内画面１
#define SC_TREND_WORK2      (2)         // trend内画面2
#define SC_LIST_WORK        (3)         // List内画面
#define SC_SCRAREA_WORK     (4)         // 滚动区域内画面

#define SC_ARRANGE_LEFT     (0)         // 靠左
#define SC_ARRANGE_CENTER   (1)         // 中央
#define SC_ARRANGE_RIGHT    (2)         // 靠右
#define SC_ARRANGE_TOP      (3)         // 靠上
#define SC_ARRANGE_BOTTOM   (4)         // 考下

/* 各种定义 */
#define SC_MAX_FREE_STRING  (128)       // 字符串的最大长度

/*
 * 访问VRAM使用的结构体
 */
typedef struct _sc_access_vram {
    union _ADRS {
        unsigned int    DWORD;
        struct {
            unsigned int X      :9;
            unsigned int Y      :17;
            unsigned int DUMMY  :6;
        } BIT;
    } ADRS;
} SCVRAM;

/*
 * 描画参数结构体
 */
typedef struct _sc_draw_param {
    SCRect          rect;
    SCPoint         dest;
    unsigned int    work_scr;
    unsigned int    work2_scr;
    int             arrangement;
    int             target_ID;
    unsigned short* str;
    bool            frame;
    bool            fill;
    unsigned short  font_type;
    unsigned short  distance;
    unsigned char   fore_color;
    unsigned char   back_color;
} SCDRAWPARAM;

/* 处理绘图命令的类 */
class SCDrawCom {
private :
    SCDrawCom() {}

public :
    static int              makeLine(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeRect(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeFixedString(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeFreeString(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeFixedStringReal(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeFreeStringReal(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeBigFontString(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makePict(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeDisp2UpdateCopy(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeWork2UpdateCopy(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeWork2UpdateCopy2(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);
    static int              makeSlideCopy(SCDRAWPARAM& param,
                                unsigned short* draw_command_buf);

    // 内部函数
    static int              makeFillRect(unsigned int work_scr,
                                unsigned char color, SCRect& rect,
                                unsigned short* buf);
    static int              makeRepeatFillRect(SCRect& rect,
                                unsigned short* buf);
    static unsigned short   makeCom01(unsigned short exec);
    static unsigned short   makeCom02(unsigned int pal_no, unsigned int scr_no);
    static unsigned short   makeCom03(unsigned int work_scr, int data_type);
    static unsigned short   makeCom04(unsigned char color);
    static unsigned short   makeCom05(unsigned char color);
    static int              makeCom060708D2U(unsigned int display,
                                unsigned short* p_buf);
    static int              makeCom060708W2U(unsigned int display,
                                unsigned short* p_buf);
    static unsigned short   makeCom09(unsigned short y);
    static unsigned short   makeCom0a(unsigned short x);
    static int              makeCom0b0c0d(unsigned int work_scr,
                                unsigned short* p_buf);
    static unsigned short   makeCom0e(unsigned short y);
    static unsigned short   makeCom0f(unsigned short x);
    static unsigned short   makeCom10(unsigned short width);
    static unsigned short   makeCom11(unsigned short height);
    static unsigned short   makeCom13(unsigned short font_type);
    static int              makeCom16171819(unsigned char pType,
                                unsigned char pNo, unsigned char r,
                                unsigned char g, unsigned char b,
                                unsigned short* p_buf);
    static int              makeCom3e(unsigned short* str,
                                unsigned short distance,
                                unsigned short* p_buf);
    static int              makeCom3f(unsigned short set_plane,
                                unsigned short plane_id,
                                unsigned short* p_buf);


    static int              getFontHeight(unsigned short font_type);
    static int              getStringWidth(unsigned short font_type,
                                    unsigned short* str);
    static void             getTargetPosition(int arrange, SCRect& rect,
                                    int width, int height, SCPoint* pos);
    static int              getBigFontStringWidth(unsigned short font_type,
                                    unsigned short distance,
                                    unsigned short* str);
    static int              getBigFontID(unsigned short font_type,
                                    unsigned short char_code);
    static int              getBigFontHeight(unsigned short font_type);
};

#endif