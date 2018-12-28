#include "sclUtil.h"
/****************************************************************************/
/**
 * @brief   SCUTF16_StrCpy関数
 * UTF-16文字列のstrcpy関数
 *
 * @author  
 * @date    2016/10/04
 *
 * @param[out]  unsigned short*     s1
 * @param[in]   unsigned short*     s2
 *
 * @return      unsigned short*     コピーされた文字列のアドレス(= s1)
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