#include <cstdlib>
#include <cstring>
#include <map>
#include "Queue.h"
#include "GDC_Driver.h"
#include "GDC_Sim.h"
#include "GDC_Common.h"
#include "DrawCommand.h"

#define RTE_FPGA_GDC_TPIF_LONGTAP_DETECTION_TIME 1000

// 描画命令的buffer
ana_stl::Queue<DrawCommand, 10 * 1024 * 1024> cmd_buf;

int PF_GDC_DRV_FlipPaletteAndScreen(unsigned int palette, unsigned int screen)
{
    FlipPalette(palette);
    FlipScreen(screen);
}
int PF_GDC_DRV_FlipPalette(unsigned int palette)
{
    FlipPalette(palette);
}
int PF_GDC_DRV_FlipScreen(unsigned int screen)
{
    FlipScreen(screen);
}
void PF_GDC_DRV_GetPaletteAndScreen(unsigned int *palette, unsigned int *screen)
{
    GetPaletteAndScreen(palette, screen);
}

unsigned int PF_GDC_DRV_GetPalette(void)
{
    return GetPalette();
}

unsigned int PF_GDC_DRV_GetScreen(void)
{
    return GetScreen();
}
pf_touch_event_func_t touchpanel_callback;
pf_touch_config_t touchpanel_config;

/*
 *      设置触摸板配置数据
 *
 * @params
 *          func，   event发生时，调用的回调函数
 *          config， 指向event配置数据结构体的指针
 * @return
 *          如果回调函数没有被指定，则返回-1；
 *          成功，返回0；
 */
int PF_GDC_DRV_SetTouchPanelConfigData(pf_touch_event_func_t func, const pf_touch_config_t* config)
{
    pf_touch_config_t temp_config = *config;

    if (temp_config.longtap_detection_time == 0) 
    {
        temp_config.longtap_detection_time = RTE_FPGA_GDC_TPIF_LONGTAP_DETECTION_TIME;
    }

    if (temp_config.width_resolution == 0 || temp_config.height_resolution == 0) 
    {
        temp_config.width_resolution = 320;
        temp_config.height_resolution = 240;
    }

    if (func == NULL) 
    {
        return -1;
    }

    touchpanel_callback = func;
    touchpanel_config = temp_config;

    return 0;
}

/*
 * @desc
 *          拷贝命令到存储buffer中。No[15:10], command数据[9:0]
 *
 * @params
 *          cmds ，  描画命令
 *          count ， 描画命令个数
 * @return
 *          成功，返回0；
 */

int PF_GDC_DRV_SetMenuCommand(const unsigned short * cmds, unsigned int count)
{
    for(unsigned int i=0; i<count; i++) 
    {
        DrawCommand cmd(cmds[i]);
        if(!cmd_buf.add(cmd)) 
        {
            PF_GDC_DRV_FlushMenuCommand();
            cmd_buf.add(cmd);
        }
    }
    return 0;
}

std::map<unsigned char, void (*)(unsigned short)> menu_execs = 
{
    {0x01, SetMenuExecute},
    {0x02, SetScreenFlip},
    {0x03, SetMenuCommonSetup},
    {0x04, SetMenuBackColor},
    {0x05, SetMenuForeColor},
    {0x06, SetMenuSourceOffset2},
    {0x07, SetMenuSourceOffset1},
    {0x08, SetMenuSourceOffset0},
    {0x09, SetMenuSourcePointY},
    {0x0A, SetMenuSourcePointX},
    {0x0B, SetMenuDestOffset2},
    {0x0C, SetMenuDestOffset1},
    {0x0D, SetMenuDestOffset0},
    {0x0E, SetMenuDestPointY},
    {0x0F, SetMenuDestPointX},
    {0x10, SetMenuTransfAreaWidth},
    {0x11, SetMenuTransfAreaHeight},
    {0x12, SetMenuIntFlag},
    {0x13, SetFontType},
    {0x14, SetFontCharacterHigherCode},
    {0x15, SetFontCharacterLowerCode},
    {0x16, SetColorPaletteAddr},
    {0x17, SetColorPaletteDataR},
    {0x18, SetColorPaletteDataG},
    {0x19, SetColorPaletteDataB},
    {0x3E, StartDrawString},
    {0x3F, StartSetPlaneID},    
};

/*
 * @desc
 *          拷贝command到存储buffer中。No[15:10], command数据[9:0]
 *
 * @params
 *          void
 * @return
 *          成功，返回0；
 */
int PF_GDC_DRV_FlushMenuCommand(void)
{
    while (!cmd_buf.empty())
    {
        DrawCommand cmd = cmd_buf.remove();
        if(ContinueDrawString())
        {
            SetMenuExecute(0x0010);
            SetDrawCommand(cmd);
        }
        else if (ContinueSetPlaneID())
        {
            SetPlaneID(cmd.raw());
        }
        else
        {
            if(menu_execs.find(cmd.no()) == menu_execs.end())
            {
                // nothing;
            }
            else
            {
                menu_execs[cmd.no()](cmd.conf());
            }
        }

        UpdateLcd();
    }
    return 0;
}

int PF_GDC_DRV_ClearMenuCommand(void)
{
    cmd_buf.clear();
    return 0;
}

/*
 * @desc
 *          注册plane表
 *
 * @params
 *          plane_table，    指向plane表的指针；
 *          count，          plane表的个数；
 * @return
 *          成功，返回0；
 */
int PF_GDC_DRV_RegistPlaneTable(const pf_plane_table_t *plane_table, unsigned int count)
{
    int i;

    if (count > MAX_PLANE_ID_COUNT) 
    {
        return PF_GDC_PARAMETER_ERROR;
    }

    for (i = 0; i < count; i++)
    {
        if (plane_table[i].start >= VRAM_SIZE) 
        {
            return -1;
        }
    }

    PF_GDC_SetPlaneTable(plane_table, (uint16_t)count);

    return 0;
}

void PF_GDC_DRV_SetTransparentColor(unsigned char _transparent_color)
{
    setTransparentColor(_transparent_color);
}

/****************************************************************************/
/**
 * @brief           PF_GDC_DRV_RegistFont()
 *                  将字体相关信息写入驱动中
 *
 * @param[in]       font_id:        字体类型，现在只有大、中、小三种字体
 *                  vram_offset:    在VRAM中的偏移地址
 *                  font_size:      字体实际占据像素的个数（weight * height）
 *                  width_info:     字体宽度，等宽还是等比例
 *                  width_address:  字体宽度值
 * @return          0: 成功
 */
/****************************************************************************/
int PF_GDC_DRV_RegistFont(
    unsigned int font_id, 
    unsigned int vram_offset, 
    pf_font_size_t font_size, 
    unsigned int width_info, 
    unsigned int width_address)
{
    RegistFont(font_id, vram_offset, font_size, width_info, width_address);
    return 0;
}

/****************************************************************************/
/**
 * @brief           PF_GDC_DRV_VramAccess()
 *                  将字体相关数据信息写入VRAM中
 *
 * @param[in]       offset: VRAM中的偏移量
 *                  dat:    要写入的数据
 *                  size：   要写入的数据大小
 *
 * @return          0: 成功
 */
/****************************************************************************/
int PF_GDC_DRV_VramAccess(unsigned int offset, const void *dat, unsigned int size)
{
    memcpy(GetVram(offset), dat, size);
    return 0;
}