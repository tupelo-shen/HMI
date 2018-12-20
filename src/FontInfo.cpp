#include "FontInfo.h"

FontInfo::FontInfo(unsigned int _font_id, 
                unsigned int    _vram_offset, 
                pf_font_size_t  _font_size, 
                unsigned int    _width_info, 
                unsigned int    _width_address) 
{
    font_id         = _font_id;
    vram_offset     = _vram_offset;
    font_size       = _font_size;
    width_info      = _width_info;
    width_address   = _width_address;
}

unsigned int FontInfo::getFontId() {
    return font_id;
}

unsigned int FontInfo::getVramOffset() {
    return vram_offset;
}

unsigned short FontInfo::getWidth() {
    return font_size.width;
}

unsigned short FontInfo::getHeight() {
    return font_size.height;
}

unsigned int FontInfo::getWidthInfo()
{
    return width_info;
}

unsigned int FontInfo::getWidthAddr()
{
    return width_address;
}
