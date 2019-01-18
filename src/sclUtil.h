#ifndef _SCL_UTIL_H_
#define _SCL_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned short  SCGetRealFontType(unsigned short font_type);
unsigned short* SCUTF16_StrCpy(unsigned short* s1, const unsigned short* s2);

#ifdef __cplusplus
}   /* extern C */
#endif

#endif