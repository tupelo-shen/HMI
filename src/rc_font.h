/**
* @file     rc_font.h
*           字体相关头文件
*
* @author   Tupelo
* @version  V1.01.01    2018/12/29
*/
#ifndef __RC_FONT_H__
#define __RC_FONT_H__

/* FontID */
#define    IDF_LARGE                            0
#define    IDF_MEDIUM                           1
#define    IDF_SMALL                            2

/* FontFile */
#define    IDF_FILE_HELVR24_R1                  0
#define    IDF_FILE_HELVR18_R3                  1
#define    IDF_FILE_K20GM_KANEKO                2
#define    IDF_FILE_SHNMK_KANEKO16              3
#define    IDF_FILE_GB24_KANEKO                 4
#define    IDF_FILE_GB16_KANEKO                 5
#define    IDF_FILE_B24_WANGYU_MICRO_OHM        6
#define    IDF_FILE_B16_WANGYU_MICRO_OHM        7
#define    IDF_FILE_CONV_CROX6H_R               8
#define    IDF_FILE_CONV_CROX5H_R               9
#define    IDF_FILE_DXFONT8X16                  10

#define    SC_FONT_TYPE_NUM                     3   /* large, medium, small */
#define    SC_MAX_FONT_NUM                      6
#define    IDF_FILE_NONE                        999
#define    SC_MAX_LANG_NUM                      LANG_MAX_NUMBER

enum
{
    LANG_CH = 0,        /* Chinese       */
    LANG_CS,            /* Czech         */
    LANG_US,            /* English       */
    LANG_FR,            /* French        */
    LANG_DE,            /* German        */
    LANG_IT,            /* Italian       */
    LANG_JP,            /* Japanese      */
    LANG_KO,            /* Korea         */
    LANG_PO,            /* Polish        */
    LANG_PT,            /* Portuguese    */
    LANG_RU,            /* Russian       */
    LANG_ES,            /* Spanish       */
    LANG_MAX_NUMBER
};

typedef struct t_font_file_info
{
    unsigned short  uhFontID;
    unsigned char   ubFileID;
} S_FONT_FILE_INFO;

extern S_FONT_FILE_INFO sc_font_map[LANG_MAX_NUMBER][SC_FONT_TYPE_NUM];
extern int sc_font_tab[SC_MAX_LANG_NUM][SC_MAX_FONT_NUM];

#endif  /* __RC_FONT_H__ */
