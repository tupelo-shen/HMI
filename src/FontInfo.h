#ifndef __FONT_INFO_H__
#define __FONT_INFO_H__

#include "GDC_Driver.h"

class FontInfo 
{
private:
    unsigned int        font_id;
    unsigned int        vram_offset;
    pf_font_size_t      font_size;
    unsigned int        width_info;
    unsigned int        width_address;
public:
    FontInfo(unsigned int   _font_id, 
            unsigned int    _vram_offset, 
            pf_font_size_t  _font_size, 
            unsigned int    _width_info, 
            unsigned int    _width_address);

    unsigned int        getFontId();
    unsigned int        getVramOffset();
    unsigned short      getWidth();
    unsigned short      getHeight();
    unsigned int        getWidthInfo();
    unsigned int        getWidthAddr();
};

#endif