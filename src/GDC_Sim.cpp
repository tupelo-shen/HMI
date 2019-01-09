#include <cstdint>
#include <map>
// #include "GDC_Common.h"
#include "GDC_Sim.h"
#include "DrawCommand.h"
#include "FontInfo.h"
#include "GDC_Common.h"
// --------------------< 本文件全局变量声明 >---------------------------
unsigned char           vram[VRAM_SIZE];            // VRAM 64M
unsigned short          menu_execute;
unsigned short          scr_flip;
unsigned short          menu_source_form;
unsigned short          menu_transf_mode1;
unsigned short          menu_color[2];              // Menu绘图颜色
menu_bitblt_loc         menu_dest_loc;
menu_bitblt_loc         menu_source_loc;
pf_point_t              menu_transf_area;
unsigned short          gdc_int;

unsigned short          font_type;
unsigned short          font_character_code;

unsigned short          color_pal[size_color_pal_idx][size_color_palette];
unsigned char           menu_color_pal_idx;
unsigned char           menu_color_pal_addr;
unsigned short          menu_color_pal_data;

bool                    continue_draw_str = false;
bool                    continue_set_plane_id = false;
uint16_t                char_margin;
static Simulator        *_sim;
static unsigned short   plane_mode;
unsigned char           transparent_color;          // 透明色
static DrawCommand      cmd;                        // 保存描画命令

std::map<unsigned short, FontInfo*> font_map;

/*****************************************************************************
                            * 函数实现 *                      
******************************************************************************/
void setSim(Simulator *sim)
{
    _sim = sim;
}
/******************************************************************************/
/*!
\brief    获取simulater
\details  
\return   なし
*******************************************************************************/
Simulator *getSim() 
{
    return _sim;
}

unsigned int VramAddr(unsigned int x, unsigned y) 
{
    return (x | (y << 9));
}

pf_point_t VramPoint(unsigned int addr) 
{
    pf_point_t point = {0};
    point.x = addr & 0x000001FF;
    point.y = ((addr >> 9) & 0x0001FFFF);
    
    return point;
}

/****************************************************************************/
/**
 * @brief           GetVram()
 *                  从VRAM中取得对应偏移量的位置的地址
 *
 * @param[in]       offset: VRAM中的偏移量
 *
 * @return          指向unsigned char类型的指针
 */
/****************************************************************************/
unsigned char* GetVram(unsigned int offset)
{
    return &vram[offset];
}

pf_point_t TransfPoint(menu_bitblt_loc loc) 
{
    pf_point_t point;
    point.x = loc.offset_addr.x + loc.point_addr.x;
    point.y = loc.offset_addr.y + loc.point_addr.y;
    return point;
}

static void execBitBlt(unsigned int to_addr, unsigned char dat)
{
    if(dat != transparent_color) 
    {
        vram[to_addr] = dat;
    }
}

static int perform_it(void* user_data) 
{
    Simulator& sim = *(static_cast<Simulator *>(user_data));
    GtkWidget * widget = GTK_WIDGET(sim.getDw());
    gtk_widget_queue_draw(widget);
    return FALSE;
}

static void destroy_it(void* user_data)
{
    return;
}

/******************************************************************************/
/*!
\brief    LCD周期刷新
\details  将VRAM的显示plate区域传输到LCD。
\return   なし
*******************************************************************************/
void UpdateLcd(void) 
{
    if(menu_execute > 0) 
    {
        ExecuteMenuDraw();
        getSim()->getBmp()->convertColor8to16(&vram[VramAddr(0, getLcdFixedY())], 
                                            color_pal[menu_color_pal_idx]);
        g_idle_add_full(G_PRIORITY_DEFAULT, perform_it, getSim(), destroy_it);
    }
}

/******************************************************************************/
/*!
\brief    
\details  
\return   なし
*******************************************************************************/
unsigned short ExecuteMenuDraw(void) 
{
    unsigned short menu_execute_tmp = menu_execute;
    switch(menu_execute)
    {
        case 0x0001:
            FillRect();
        break;
        case 0x0002:
            Transf8bitRect();
        break;
        case 0x0004:
            Transf1bitRect();
        break;
        case 0x0008:
            Transf1bitFontRect();
        break;
        case 0x0010:
            RectString(cmd.raw());
        break;
        default:
        break;
    }
    menu_execute = 0;
    return menu_execute_tmp;
}
/******************************************************************************/
/*!
\brief    填充矩形
\details  将Menu绘图设定的color寄存器的指定颜色（fore）拷贝到目标地址
\return   None
*******************************************************************************/
void FillRect(void) 
{
    pf_point_t point_dest = TransfPoint(menu_dest_loc);
    for(int i=0; i<menu_transf_area.x; i++) 
    {
        for(int j=0; j<menu_transf_area.y; j++) 
        {
            execBitBlt(VramAddr(point_dest.x+i, getBitBltFixedY()+point_dest.y+j), menu_color[1]);
        }
    }
}

/******************************************************************************/
/*!
\brief    填充矩形(1 Bit)
\details  传输1 Bit矩形数据。数据源只能是线性数据。数据源是透明色时，不用填写目标地址
\return   None
*******************************************************************************/
void Transf1bitRect(void) 
{
    pf_point_t point_source = TransfPoint(menu_source_loc);
    pf_point_t point_dest = TransfPoint(menu_dest_loc);
    Transf1bitCommonRect(menu_transf_area.x, menu_transf_area.y, point_source, point_dest);
}

/******************************************************************************/
/*!
\brief    传输1 Bit矩形的共同处理
\details  1bit矩形传输文字CODE，BMP数据的共同处理
\param[in] width ，      矩形宽度
\param[in] height，      矩形高度
\param[in] source ，     数据源坐标
\param[in] dest ，       目标数据坐标
\param[in] offset_addr， 字体的文字编码地址
\return   None
*******************************************************************************/
void Transf1bitCommonRect(unsigned short    width, 
                        unsigned short      height, 
                        pf_point_t          source, 
                        pf_point_t          dest, 
                        unsigned short      offset_addr) 
{
    int                 k = 0;
    const unsigned char bit = 0x80;
    const unsigned int  char_code_offset = offset_addr*height*width/8;

    for(int j=0; j<height; j++) 
    {
        for(int i=0; i<width; i++) 
        {
            if((vram[VramAddr(source.x, source.y)+char_code_offset + (k/8)] & (bit >> (k%8))) > 0)  // 如果设置了位，请加入前色
            {
                execBitBlt(VramAddr(dest.x + i, getBitBltFixedY() + dest.y + j), menu_color[1]);
            } else 
            {
                execBitBlt(VramAddr(dest.x + i, getBitBltFixedY() + dest.y + j), menu_color[0]);
            }
            k++;
        }
    }
}

/******************************************************************************/
/*!
\brief    传输矩形(文字CODE)
\details  1bit矩形传输文字CODE。数据源只能是线性数据。数据源是透明色时，不用填写目标地址。
\return   なし
*******************************************************************************/
void Transf1bitFontRect(void)
{
  pf_point_t point_source_offset = VramPoint(font_map[font_type]->getVramOffset());
  pf_point_t point_dest = TransfPoint(menu_dest_loc);
  Transf1bitCommonRect(
            font_map[font_type]->getWidth(), 
            font_map[font_type]->getHeight(), 
            point_source_offset, 
            point_dest, 
            font_character_code);
}

/******************************************************************************/
/*!
\brief     
\details  
\return   None
*******************************************************************************/
static uint16_t getFontWidth(uint16_t code, uint32_t width_address)
{
    const uint8_t *data = (const uint8_t *)width_address;
    uint16_t width = data[code];

    return width;
}
/******************************************************************************/
/*!
\brief     
\details  
\return   None
*******************************************************************************/
static void addWidthOffset(unsigned int &width_offset, uint16_t char_code)
{
    if(font_map[font_type]->getWidthInfo() == 1) { // Propotional
        width_offset += getFontWidth(char_code, font_map[font_type]->getWidthAddr()) + char_margin;
    } else { // Same width
        width_offset += font_map[font_type]->getWidth() + char_margin;
    }
}

/******************************************************************************/
/*!
\brief     
\details  
\return   None
*******************************************************************************/
void RectString(unsigned short dat) 
{
    static unsigned int width_offset = 0;
    if(dat == 0) 
    {
        continue_draw_str = false;
        width_offset = 0;
    } 
    else 
    {
        if(font_map[font_type] != NULL) 
        {
            pf_point_t point_source_offset = VramPoint(font_map[font_type]->getVramOffset());
            pf_point_t point_dest = TransfPoint(menu_dest_loc);
            point_dest.x += width_offset;
            Transf1bitCommonRect(font_map[font_type]->getWidth(), 
                                font_map[font_type]->getHeight(), 
                                point_source_offset, 
                                point_dest, 
                                dat);
            addWidthOffset(width_offset, dat);
        }
    } 
}

/******************************************************************************/
/*!
\brief      返回short型值的位数
\details    <http://www.nminoru.jp/~nminoru/programming/bitcount.html>
\param[in]  short型值
\return     short型值的位数
*******************************************************************************/
static int num_bits_short(unsigned short bits)
{
    bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
    bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
    bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
    return (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
}
static void SetShortHigher(unsigned short &dat, unsigned short set) 
{
    dat = ( ( dat & ~(0xFF00) ) | (set << 8) );
}
static void SetShortLower(unsigned short &dat, unsigned short set) 
{
  dat = ( dat & ~(0x00FF) ) | set;
}

/******************************************************************************/
/*!
\brief      设置执行寄存器
\details    设置多位或0位时，设置无效。
\param[in]  绘图执行命令
\return     成功/失败
*******************************************************************************/
void SetMenuExecute(unsigned short code) 
{
    int num = num_bits_short(code);
    if(num > 1 || num == 0) 
    {
        return;
    } 
    else 
    {
        menu_execute = code;
    }
}
void SetScreenFlip(unsigned short code) 
{
    scr_flip = (code & 0x0001);
    menu_color_pal_idx = ( (code >> 1) & 0x0001);
}
void SetMenuCommonSetup(unsigned short code) 
{
    menu_source_form = (code & 0x0001);
    menu_transf_mode1 = ( (code >> 1) & 0x0001);
}
void SetMenuBackColor(unsigned short color) 
{
    menu_color[0] = color;
}
void SetMenuForeColor(unsigned short color) 
{
    menu_color[1] = color;
}
// void SetMenuSourceOffset2(unsigned short loc) 
// {
//     SetOffset2(loc, menu_source_loc);
// }

/******************************************************************************/
/*!
\brief    填充矩形(8 Bits)
\details  传输8bits矩形数据。选择数据源是坐标数据还是线性数据
\return   None
*******************************************************************************/
void Transf8bitRect() 
{
    pf_point_t point_source = TransfPoint(menu_source_loc);
    pf_point_t point_dest = TransfPoint(menu_dest_loc);
    for(int j=0; j<menu_transf_area.y; j++) 
    {
        for(int i=0; i<menu_transf_area.x; i++) 
        {
            if(menu_source_form == 0)       // 坐标
            {     
                execBitBlt(VramAddr(point_dest.x + i, getBitBltFixedY() + point_dest.y + j), 
                            vram[VramAddr(point_source.x + i, point_source.y + j)]);
            } 
            else if(menu_source_form == 1)  // linear
            {
                execBitBlt(VramAddr(point_dest.x + i, getBitBltFixedY() + point_dest.y + j), 
                            vram[VramAddr(point_source.x, point_source.y)+i+menu_transf_area.x*j]);
            } 
            else 
            {
                // nothing.
            }
        }
    }
}



// -------------------<>-------------------------------------
static const unsigned short     x_mask      = 0x01FF;
static const unsigned int       y_mask_0    = 0x00000001;
static const unsigned int       y_mask_1    = 0x000007FE;
static const unsigned int       y_mask_2    = 0x0001F800;
static const unsigned int       y_mask_01   = (y_mask_0 | y_mask_1);
static const unsigned int       y_mask_12   = (y_mask_1 | y_mask_2);
static const unsigned int       y_mask_02   = (y_mask_0 | y_mask_2);

static void SetOffset0(unsigned short loc, menu_bitblt_loc &menu_loc) 
{
    menu_loc.offset_addr.x = (loc & x_mask);
    menu_loc.offset_addr.y = (((((loc >> 9) & y_mask_0))) | (menu_loc.offset_addr.y & y_mask_12));
}

static void SetOffset1(unsigned short loc, menu_bitblt_loc &menu_loc) {
    menu_loc.offset_addr.y = ((((loc << 1) & y_mask_1)) | (menu_loc.offset_addr.y & y_mask_02));
}

static void SetOffset2(unsigned short loc, menu_bitblt_loc &menu_loc) 
{
    unsigned int loc_int = (unsigned int) loc;
    menu_loc.offset_addr.y = (((loc_int << 11) & y_mask_2) | (menu_loc.offset_addr.y & y_mask_01));
}

void SetMenuSourceOffset0(unsigned short loc) 
{
    SetOffset0(loc, menu_source_loc);
}

void SetMenuSourceOffset1(unsigned short loc) 
{
    SetOffset1(loc, menu_source_loc);
}

void SetMenuSourceOffset2(unsigned short loc) 
{
    SetOffset2(loc, menu_source_loc);
}

void SetMenuDestOffset0(unsigned short loc) {
    SetOffset0(loc, menu_dest_loc);
}

void SetMenuDestOffset1(unsigned short loc) {
    SetOffset1(loc, menu_dest_loc);
}

void SetMenuDestOffset2(unsigned short loc) {
    SetOffset2(loc, menu_dest_loc);
}
void SetMenuSourcePointX(unsigned short loc) 
{
    menu_source_loc.point_addr.x = loc;
}
void SetMenuSourcePointY(unsigned short loc) 
{
    menu_source_loc.point_addr.y = loc;
}
void SetMenuDestPointY(unsigned short loc) 
{
    menu_dest_loc.point_addr.y = loc;
}
void SetMenuDestPointX(unsigned short loc) 
{
    menu_dest_loc.point_addr.x = loc;
}
void SetMenuTransfAreaWidth(unsigned short width) 
{
    menu_transf_area.x = width;
}

void SetMenuTransfAreaHeight(unsigned short height) 
{
    menu_transf_area.y = height;
}
void SetMenuIntFlag(unsigned short _gdc_int) 
{
    gdc_int = _gdc_int;
}

void SetFontType(unsigned short _font_type) 
{
    font_type = _font_type;
}

void SetFontCharacterHigherCode(unsigned short char_code) 
{
    SetShortHigher(font_character_code, char_code);
}

void SetFontCharacterLowerCode(unsigned short char_code) 
{
    SetShortLower(font_character_code, char_code);
}

void SetColorPaletteAddr(unsigned short addr) 
{
    menu_color_pal_idx = (char)(addr >> 8);
    menu_color_pal_addr = (char)(addr & 0x00FF);
}

void SetColorPaletteDataR(unsigned short dat) 
{
    unsigned short r = (dat >> 3) & 0x1f;
    if( (r != 0x1f) && ((dat & 0x04) != 0) ) {
        r++;
    }
    menu_color_pal_data = ((menu_color_pal_data & ~(0xF800)) | (r << 11));
}

void SetColorPaletteDataG(unsigned short dat) 
{
    unsigned short g = (dat >> 2) & 0x3f;
    if( (g != 0x3f) && ((dat & 0x02) != 0) ) {
        g++;
    }
    menu_color_pal_data = ( ( menu_color_pal_data & ~(0x07E0) ) | (g << 5) );
}

void SetColorPaletteDataB(unsigned short dat) 
{
    unsigned short b = (dat >> 3) & 0x1f;
    if( (b != 0x1f) && ((dat & 0x04) != 0) ) 
    {
        b++;
    }
    menu_color_pal_data = ( menu_color_pal_data & ~(0x001F) ) | b;
    WriteColorPalette();
}
// 开始画字符串
void StartDrawString(unsigned short dat) 
{
    continue_draw_str = true;
    char_margin = dat;
}

void StartSetPlaneID(unsigned short dat)
{
    continue_set_plane_id = true;
    plane_mode = dat; // 1: source, 2: dest
}

void WriteColorPalette(void)
{
    color_pal[menu_color_pal_idx][menu_color_pal_addr] = menu_color_pal_data;
}

// ----------------------<>---------------------------------
void SetDrawCommand(DrawCommand _cmd)
{
    cmd = _cmd;
}
bool ContinueDrawString(void)
{
    return continue_draw_str;
}
bool ContinueSetPlaneID(void)
{
    return continue_set_plane_id;
}
void splitAddress(uint32_t address, SplitAddress *split_address)
{
    split_address->address25to20 = (address & 0x03F00000) >> 20;
    split_address->address19to10 = (address & 0x000FFC00) >> 10;
    split_address->address09to00 = (address & 0x000003FF);
}
void SetPlaneID(unsigned short dat)
{

    if(dat < PF_GDC_GetPlaneTableCount()) 
    {
        SplitAddress split_address = {0};
        const pf_plane_table_t* plane_table = PF_GDC_GetPlaneTable();
        splitAddress(plane_table[dat].start, &split_address);
        if(plane_mode == 1) 
        {
            SetMenuSourceOffset2(split_address.address25to20);
            SetMenuSourceOffset1(split_address.address19to10);
            SetMenuSourceOffset0(split_address.address09to00);
        } 
        else if(plane_mode == 2){
            SetMenuDestOffset2(split_address.address25to20);
            SetMenuDestOffset1(split_address.address19to10);
            SetMenuDestOffset0(split_address.address09to00);
        }
    }
    continue_set_plane_id = false;
}

unsigned long getBitBltFixedY()
{
    return (scr_flip == 0 && menu_transf_mode1) ? 0x200 : 0;
}
unsigned long getLcdFixedY()
{
    return (scr_flip == 0) ? 0 : 0x200;
}

void FlipPalette(unsigned int palette)
{
    menu_color_pal_idx = palette;
}
void FlipScreen(unsigned int screen)
{
    scr_flip = screen;
}
void GetPaletteAndScreen(unsigned int *palette, unsigned int *screen)
{
    *palette = menu_color_pal_idx;
    *screen = scr_flip;
}
unsigned int GetPalette()
{
    return menu_color_pal_idx;
}
unsigned int GetScreen()
{
    return scr_flip;
}

void RegistFont(unsigned int    font_id, 
                unsigned int    vram_offset, 
                pf_font_size_t  font_size, 
                unsigned int    width_info, 
                unsigned int    width_address) 
{
    font_map[font_id] = new FontInfo(font_id, vram_offset, font_size, width_info, width_address);
}

void setTransparentColor(unsigned char color)
{
    transparent_color = color; 
}
