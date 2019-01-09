#ifndef __HMI_RESOURCE_H__
#define __HMI_RESOURCE_H__

#include "rc_font.h"

/* CPU之外RAM的使用方法
0x80700000+-------------------------------------------------+
          | Heap                             3072KB         |
          |                                                 |
          | Trend (Log + Disp) Data      MAX 2048KB         |
          |                                                 |
          | 其它                          MAX 4096KB        |
0x80FFFFFF| (Font信息，图像信息，文字信息)                     |
0x81000000+-------------------------------------------------+
          | Stack                                           |
0x810FFFFF| 1024KB                                          |
          +-------------------------------------------------+
*/

#define INIT_MEM_SIZE           (3*1024*1024)   // heap size
#define SC_BIN_HEAD_SIZE        (7*4)
#define SC_PALETTE_DATA_SIZE    (2048)          // 256色*4(makeRegistColor command数)*2Palette
#define SC_MAX_COLOR_NO         (256)           // 注册颜色的数量
#define SC_PALETTE_PRIMARY_COLOR_NUM  (4)       // 每种颜色要读取的数据数(R,G,B,A)
#define SC_MAX_PICTURE_NUM      (300)
#define SC_MAX_CHAR_NUM         (65536)
#define SC_MAX_CHAR_NUM_IN_FONT (13000)
#define SC_MAX_STR_NUM          (3000)          // provisional definition
#define SC_MAX_STR_LENGTH       (60)            // provisional definition
// #define SC_MAX_FONT_NUM         (6)

enum {
    OFFSET_PICTURE,
    OFFSET_PICTURE_CMD,
    OFFSET_STRING_CMD,
    OFFSET_STRING,
    OFFSET_IMAGE,
    OFFSET_FONT,
    OFFSET_PALETTE,
};

typedef union _head
{
    struct _font
    {
        unsigned char WIDTH;
        unsigned char HEIGHT;
        unsigned short COUNT;
        struct _char 
        {
            unsigned short CODE;
            unsigned short WIDTH;
        } CHAR[SC_MAX_CHAR_NUM_IN_FONT];
    } FONT;
    struct _image
    {
        unsigned int COUNT;
        struct _char 
        {
            unsigned int OFFSET;
            unsigned int WIDTH;
            unsigned int HEIGHT;
        } PICT[SC_MAX_PICTURE_NUM];
        unsigned char REV[SC_MAX_CHAR_NUM_IN_FONT*4 + 4 - (SC_MAX_PICTURE_NUM*12 + 4)];
    } IMG;
    struct _string
    {
        unsigned int OFFSET[SC_MAX_STR_NUM + 1];
        unsigned char REV[SC_MAX_CHAR_NUM_IN_FONT*4 + 4 - (SC_MAX_STR_NUM*4 + 4)];
    } STR;
} SCHEAD;

typedef struct _sc_pict_info_str {
    unsigned short  width;
    unsigned short  height;
} SCPICTINFO;

typedef struct _sc_font_info_str {
    unsigned int  f_width;
    unsigned int  f_height;
    unsigned char width[SC_MAX_CHAR_NUM];
} SCFONTINFO;

typedef int(*SCFONTTAB)[SC_MAX_FONT_NUM];

unsigned long*          sc_getHmiHeap(unsigned long* size);
unsigned long*          sc_getHmiHeapTemp(unsigned long* size);
unsigned int*           sc_getBinOffset(void);
SCHEAD*                 sc_getHead(void);
int                     sc_getFontNum(void);
void                    sc_setFontNum(int font_num);
SCFONTINFO*             sc_getFontInfo(void);
SCFONTTAB               sc_getFontTab(void);
unsigned short          sc_getPictWidth(int pict_id);
void                    sc_getFixedString(int target_ID, unsigned short* str);
#endif /* __HMI_RESOURCE_H__ */