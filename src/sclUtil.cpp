/**
* @file     sclUtil.cpp
*           工具函数
*
* @author   Tupelo
* @version  V1.01.01    2018/12/29
*/
#include "sclUtil.h"
#include "rc_font.h"

/****************************************************************************/
/**
 * @brief           SCUTF16_StrCpy()
 *                  UTF-16型字符串的strcpy()函数
 *
 * @param[out]      unsigned short*     s1
 * @param[in]       unsigned short*     s2
 *
 * @return  int     unsigned short*     被拷贝的字符串(= s1)
 */
/****************************************************************************/
extern "C" unsigned short*  SCUTF16_StrCpy(
    unsigned short* s1,
    const unsigned short* s2
)
{
    unsigned short* rtn = s1;


    while(*s2) {
        *s1++ = *s2++;
    }
    *s1 = *s2;


    return(rtn);
}

/****************************************************************************/
/**
 * @brief           SCGetRealFontType()
 *                  取得由designer指定的真实的FontType
 *
 * @param[in]       unsigned short  font_type       指定的FontType
 *                                              SC_PARTS_FONT_TYPE_LFONT(IDF_LARGE)
 *                                              SC_PARTS_FONT_TYPE_MFONT(IDF_MEDIUM)
 *                                              SC_PARTS_FONT_TYPE_SFONT(IDF_SMALL)
 *
 * @return  int     unsigned short  Designer上で指定されたフォントタイプ
 *                                              SC_PARTS_FONT_TYPE_LFONT(IDF_LARGE)
 *                                              SC_PARTS_FONT_TYPE_MFONT(IDF_MEDIUM)
 *                                              SC_PARTS_FONT_TYPE_SFONT(IDF_SMALL)
 */
/****************************************************************************/
extern "C" unsigned short SCGetRealFontType(
    unsigned short  font_type
)
{
    unsigned char   file_id;
    unsigned short  lang;
    unsigned short  rtn = IDF_MEDIUM;


    if(font_type < SC_FONT_TYPE_NUM) 
    {
        // unsigned int  i;

        // lang = LIQHMIDATA->sGetLang();
        lang = 2; // US

        // fontTypeからデザイナーで定義されたFontFileIDを取得
        file_id = sc_font_map[lang][font_type].ubFileID;

        // driverに登録されたFontFileIDから実際のFontTypeを取得
        for(unsigned int i = 0 ; i < SC_FONT_TYPE_NUM ; i++) {
            if(sc_font_tab[lang][i] == file_id) {
                rtn = i;
                break;
            }
        }
    }


    return(rtn);
}