#include "rc_font.h"

/* Font Table Language/FontFile */
int sc_font_tab[SC_MAX_LANG_NUM][SC_MAX_FONT_NUM] =  // 288B
{
    {    //CH
        IDF_FILE_GB24_KANEKO,
        IDF_FILE_GB16_KANEKO,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //CS
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //US
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //FR
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //DE
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //IT
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //JP
        IDF_FILE_K20GM_KANEKO,
        IDF_FILE_SHNMK_KANEKO16,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //KO
        IDF_FILE_B24_WANGYU_MICRO_OHM,
        IDF_FILE_B16_WANGYU_MICRO_OHM,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //PO
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //PT
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //RU
        IDF_FILE_CONV_CROX5H_R,
        IDF_FILE_CONV_CROX5H_R,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    },
    {    //ES
        IDF_FILE_HELVR24_R1,
        IDF_FILE_HELVR18_R3,
        IDF_FILE_DXFONT8X16,
        IDF_FILE_NONE,
        IDF_FILE_NONE,
        IDF_FILE_NONE
    }
};

/* Font Map FontID/FontFile */
S_FONT_FILE_INFO sc_font_map[LANG_MAX_NUMBER][SC_FONT_TYPE_NUM] =
{
    {
        {IDF_LARGE, IDF_FILE_GB24_KANEKO},
        {IDF_MEDIUM, IDF_FILE_GB16_KANEKO},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//CH
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//CS
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//US
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//FR
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//DE
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//IT
    {
        {IDF_LARGE, IDF_FILE_K20GM_KANEKO},
        {IDF_MEDIUM, IDF_FILE_SHNMK_KANEKO16},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//JP
    {
        {IDF_LARGE, IDF_FILE_B24_WANGYU_MICRO_OHM},
        {IDF_MEDIUM, IDF_FILE_B16_WANGYU_MICRO_OHM},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//KO
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//PO
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//PT
    {
        {IDF_LARGE, IDF_FILE_CONV_CROX5H_R},
        {IDF_MEDIUM, IDF_FILE_CONV_CROX5H_R},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    },//RU
    {
        {IDF_LARGE, IDF_FILE_HELVR18_R3},
        {IDF_MEDIUM, IDF_FILE_HELVR18_R3},
        {IDF_SMALL, IDF_FILE_DXFONT8X16}
    }//ES
};