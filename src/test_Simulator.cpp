#include <cassert>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include "Queue.h"
#include "DrawCommand.h"
#include "GDC_Sim.h"
#include "test_Simulator.h"

extern unsigned char        vram[64*1024*1024];      // VRAM
extern unsigned short       menu_execute;
extern ana_stl::Queue<DrawCommand, 1024> cmd_buf;
extern unsigned short       scr_flip;
extern unsigned short       menu_transf_mode1;
extern unsigned short       menu_source_form;
extern unsigned short       menu_color[2]; // メニュー描画カラー
extern menu_bitblt_loc      menu_source_loc;
extern menu_bitblt_loc      menu_dest_loc;
extern pf_point_t           menu_transf_area;
extern unsigned short       gdc_int;
extern unsigned short       font_type;
extern unsigned short       font_character_code;
extern unsigned short       color_pal[size_color_pal_idx][size_color_palette];
extern unsigned char        menu_color_pal_idx;
extern unsigned char        menu_color_pal_addr;
extern unsigned short       menu_color_pal_data;
extern unsigned char        transparent_color;       // 透明色
extern bool                 continue_draw_str;
extern unsigned short       char_margin;

void TestSetMenuExecute() 
{
    SetMenuExecute(0x0001);
    assert(menu_execute == 0x0001);
    SetMenuExecute(0x0002);
    assert(menu_execute == 0x0002);
    SetMenuExecute(0x0003);
    assert(menu_execute == 0x0002);
    SetMenuExecute(0x0004);
    assert(menu_execute == 0x0004);
    SetMenuExecute(0x0005);
    assert(menu_execute == 0x0004);
    SetMenuExecute(0x0006);
    assert(menu_execute == 0x0004);
    SetMenuExecute(0x0007);
    assert(menu_execute == 0x0004);
    SetMenuExecute(0x0008);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x0009);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x000A);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x000B);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x000C);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x000D);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x000E);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x000F);
    assert(menu_execute == 0x0008);
    SetMenuExecute(0x0010);
    assert(menu_execute == 0x0010);

    printf("TestSetMenuExecute() is tested sucessfully!\n");
}

void TestSetScreenFlip() 
{
    unsigned short cmds_1[1] = {(0x0002 << 10) | (0x0000)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_idx == 0);
    assert(scr_flip == 0);
    // assert(PF_GDC_DRV_GetScreen() == 0);
    // assert(PF_GDC_DRV_GetPalette() == 0);

    cmds_1[0] = ((0x0002 << 10) | (0x0001));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_idx == 0);
    assert(scr_flip == 1);
    // assert(PF_GDC_DRV_GetPalette() == 0);
    // assert(PF_GDC_DRV_GetScreen() == 1);

    cmds_1[0] = ((0x02 << 10) | (0x02));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_idx == 1);
    assert(scr_flip == 0);
    // assert(PF_GDC_DRV_GetPalette() == 1);
    // assert(PF_GDC_DRV_GetScreen() == 0);

    cmds_1[0] = ((0x02 << 10) | (0x03));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_idx == 1);
    assert(scr_flip == 1);

    cmds_1[0] = ((0x02 << 10) | (0x00));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_idx == 0);
    assert(scr_flip == 0);
    
    PF_GDC_DRV_FlipPalette(1);
    PF_GDC_DRV_FlipScreen(2);
    assert(PF_GDC_DRV_GetPalette() == 1);
    assert(PF_GDC_DRV_GetScreen() == 2);

    PF_GDC_DRV_FlipPaletteAndScreen(0, 0);
    assert(PF_GDC_DRV_GetScreen() == 0);
    assert(PF_GDC_DRV_GetPalette() == 0);

    unsigned int tmp_palette = 2;
    unsigned int tmp_screen = 3;
    PF_GDC_DRV_GetPaletteAndScreen(&tmp_palette, &tmp_screen);
    assert(tmp_palette == 0);
    assert(tmp_screen == 0);

    printf("TestSetScreenFlip() is tested sucessfully!\n");
}

void TestCommand()
{
    PF_GDC_DRV_ClearMenuCommand();
    unsigned short cmds_1[1] = {(0x01 << 10) | (0x02)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_execute == 0x0000);

    unsigned short cmds_2[2] = {((0x01 << 10) | (0x02)), ((0x01 << 10) | (0x04))};
    PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_execute == 0x0000);

    printf("TestCommand() is tested sucessfully!\n");
}

void TestSetMenuCommonSetup() 
{
    unsigned short cmds_1[1] = {(0x03 << 10) | (0x00)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_transf_mode1 == 0x0000);
    assert(menu_source_form == 0x0000);
    
    cmds_1[0] = ((0x03 << 10) | (0x01));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_transf_mode1 == 0x0000);
    assert(menu_source_form == 0x0001);

    cmds_1[0] = ((0x03 << 10) | (0x02));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_transf_mode1 == 0x0001);
    assert(menu_source_form == 0x0000);

    cmds_1[0] = ((0x03 << 10) | (0x03));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_transf_mode1 == 0x0001);
    assert(menu_source_form == 0x0001);

    cmds_1[0] = ((0x03 << 10) | (0x00));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_transf_mode1 == 0x0000);
    assert(menu_source_form == 0x0000);

    printf("TestSetMenuCommonSetup() is tested sucessfully!\n");
}

void TestSetMenuForeColor() 
{
    unsigned short cmds_1[1] = {(0x04 << 10) | (0x00)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color[1] == 0x0000);

    cmds_1[0] = ((0x05 << 10) | (0x01));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color[1] == 0x0001);

    cmds_1[0] = ((0x05 << 10) | (0x00));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color[1] == 0x0000);

    printf("TestSetMenuForeColor() is tested sucessfully!\n");
}

void TestSetMenuBackColor() 
{
    unsigned short cmds_1[1] = {(0x05 << 10) | (0x00)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color[0] == 0x0000);

    cmds_1[0] = ((0x04 << 10) | (0x01));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color[0] == 0x0001);

    cmds_1[0] = ((0x04 << 10) | (0x00));
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color[0] == 0x0000);

    printf("TestSetMenuBackColor() is tested sucessfully!\n");
}

void TestSetMenuSourceOffset() 
{
    menu_source_loc.offset_addr.x = 0;
    menu_source_loc.offset_addr.y = 0;
    unsigned short cmds_0[3] = {(0x06 << 10) | (0x0000), (0x07 << 10) | (0x0000), (0x08 << 10) | (0x0000)};
    PF_GDC_DRV_SetMenuCommand(cmds_0, sizeof(cmds_0) / sizeof(cmds_0[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_source_loc.offset_addr.x == 0x0000);
    assert(menu_source_loc.offset_addr.y == 0x00000000);

    unsigned short cmds_1[1] = {(0x08 << 10) | (0x0387)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_source_loc.offset_addr.x == 0x0187);
    assert(menu_source_loc.offset_addr.y == 0x00000001);

    cmds_1[0] = {(0x07 << 10) | (0x00FF)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_source_loc.offset_addr.x == 0x0187);
    assert(menu_source_loc.offset_addr.y == 0x01FF);

    cmds_1[0] = {(0x06 << 10) | (0x003F)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_source_loc.offset_addr.x == 0x0187);
    assert(menu_source_loc.offset_addr.y == 0x0001F9FF);

    printf("TestSetMenuSourceOffset() is tested sucessfully!\n");
}

void TestSetMenuSourcePoint() {
    unsigned short cmds_1[1] = {(0x09 << 10) | (0x01)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_source_loc.point_addr.y == 0x0001);

    cmds_1[0] = {(0x0A << 10) | (0x0002)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_source_loc.point_addr.x == 0x0002);
    

    printf("TestSetMenuSourcePoint() is tested sucessfully!\n");
}

void TestSetMenuDestOffset() {
    menu_dest_loc.offset_addr.x = 0;
    menu_dest_loc.offset_addr.y = 0;
    unsigned short cmds_0[3] = {(0x0B << 10) | (0x0000), (0x0C << 10) | (0x0000), (0x0D << 10) | (0x0000)};
    PF_GDC_DRV_SetMenuCommand(cmds_0, sizeof(cmds_0) / sizeof(cmds_0[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_dest_loc.offset_addr.x == 0x0000);
    assert(menu_dest_loc.offset_addr.y == 0x00000000);


    unsigned short cmds_1[1] = {(0x0D << 10) | (0x0287)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_dest_loc.offset_addr.x == 0x0087);
    assert(menu_dest_loc.offset_addr.y == 0x00000001);

    cmds_1[0] = {(0x0C << 10) | (0x00FF)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_dest_loc.offset_addr.x == 0x0087);
    assert(menu_dest_loc.offset_addr.y == 0x01FF);

    cmds_1[0] = {(0x0B << 10) | (0x003F)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_dest_loc.offset_addr.x == 0x0087);
    assert(menu_dest_loc.offset_addr.y == 0x0001F9FF);

    printf("TestSetMenuDestOffset() is tested sucessfully!\n");
}

void TestSetMenuDestPoint() {
    unsigned short cmds_1[1] = {(0x0E << 10) | (0x01)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_dest_loc.point_addr.y == 0x0001);

    cmds_1[0] = {(0x0F << 10) | (0x0002)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_dest_loc.point_addr.x == 0x0002);

    printf("TestSetMenuDestPoint() is tested sucessfully!\n");
}

void TestSetMenuTransfArea() {
    unsigned short cmds_1[1] = {(0x10 << 10) | (0x12)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_transf_area.x == 0x0012);
    cmds_1[0] = {(0x11 << 10) | (0x14)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_transf_area.y == 0x0014);

    printf("TestSetMenuTransfArea() is tested sucessfully!\n");
}

void TestSetMenuIntFlag() {
    unsigned short cmds_1[1] = {(0x12 << 10) | (0x23)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(gdc_int == 0x0023);

    printf("TestSetMenuIntFlag() is tested sucessfully!\n");
}

void TestSetFontType() {
    unsigned short cmds_1[1] = {(0x13 << 10) | (0x33)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(font_type == 0x0033);

    printf("TestSetFontType() is tested sucessfully!\n");
}

void TestSetFontCharacterCode() 
{
    unsigned short cmds_1[1] = {(0x14 << 10) | (0x43)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(font_character_code == 0x4300);

    cmds_1[0] = {(0x15 << 10) | (0x12)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(font_character_code == 0x4312);

    cmds_1[0] = {(0x14 << 10) | (0xAB)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(font_character_code == 0xAB12);

    printf("TestSetFontCharacterCode() is tested sucessfully!\n");
}

void TestSetColorPaletteAddr() {
    unsigned short cmds_1[1] = {(0x16 << 10) | (0x0121)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_idx == 0x01);
    assert(menu_color_pal_addr == 0x21);

    cmds_1[0] = {(0x16 << 10) | (0x0012)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_idx == 0x00);
    assert(menu_color_pal_addr == 0x12);

    printf("TestSetColorPaletteAddr() is tested sucessfully!\n");
}

void TestSetColorPaletteData() {
    unsigned short cmds_1[1] = {(0x17 << 10) | (0x1F)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_data == 0x2000);

    cmds_1[0] = {(0x18 << 10) | (0x21)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(menu_color_pal_data == 0x2100);

    cmds_1[0] = {(0x19 << 10) | (0x11)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();

    assert(color_pal[menu_color_pal_idx][menu_color_pal_addr] == 0x2102);

    printf("TestSetColorPaletteData() is tested sucessfully!\n");
}

void TestStartDrawString() {
    unsigned short cmds_1[1] = {(0x3E << 10) | (0x1B)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(continue_draw_str == true);
    assert(char_margin == 0x001B);

    cmds_1[0] = {(0x3E << 10) | (0x03)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(continue_draw_str == true);
    assert(char_margin == 0x001B);

    cmds_1[0] = {0x00};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(continue_draw_str == false);
    assert(char_margin == 0x001B);
    printf("TestStartDrawString() is tested sucessfully!\n");
}

void TestSetPlaneID()
{
    static pf_plane_table_t p_table[2];
    p_table[0].start = 0x00101010;
    p_table[1].start = 0x00010101;
    PF_GDC_DRV_RegistPlaneTable(p_table, 2);
    assert(ContinueSetPlaneID() == false);
    unsigned short cmds_1[1] = {(0x3F << 10) | (0x01)};
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(ContinueSetPlaneID() == false);
    unsigned short cmds_2[1] = {0x0000};
    PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(ContinueSetPlaneID() == false);

    // assert(menu_source_loc.offset_addr.x == 0x0010);
    // assert(menu_source_loc.offset_addr.y == 0x0808);

    // PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    // PF_GDC_DRV_FlushMenuCommand();
    // cmds_2[0] = 0x0002;
    // PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
    // PF_GDC_DRV_FlushMenuCommand();

    // assert(menu_source_loc.offset_addr.x == 0x0010);
    // assert(menu_source_loc.offset_addr.y == 0x0808);
    // assert(menu_dest_loc.offset_addr.x != 0x0010);
    // assert(menu_dest_loc.offset_addr.y != 0x0808);

    // unsigned short cmds_3[2] = {
    //     (0x3F << 10) | (0x02),
    //     0x0000
    // };
    // PF_GDC_DRV_SetMenuCommand(cmds_3, sizeof(cmds_3) / sizeof(cmds_3[0]));
    // PF_GDC_DRV_FlushMenuCommand();

    // assert(menu_dest_loc.offset_addr.x == 0x0010);
    // assert(menu_dest_loc.offset_addr.y == 0x0808);
    
    printf("TestSetPlaneID() is tested sucessfully!\n");
}

void TestVramAddr() 
{
    vram[VramAddr(0, 0)] = 1;
    assert(vram[0] == 1);
    vram[VramAddr(100, 0)] = 2;
    assert(vram[100] == 2);
    vram[VramAddr(319, 0)] = 3;
    assert(vram[319] == 3);
    vram[VramAddr(0, 1)] = 4;
    assert(vram[512] == 4);
    vram[VramAddr(1, 1)] = 5;
    assert(vram[513] == 5);
    vram[VramAddr(319, 1)] = 5;
    assert(vram[512 + 319] == 5);
    
    printf("TestVramAddr() is tested sucessfully!\n");
}

void TestTransf8bitRect() 
{
    ///////////////////////////////////////////////////////////////////////////////
    // Linear transfer test
    ///////////////////////////////////////////////////////////////////////////////
    unsigned char bmp_8[768] = {0};
    for(int i=0; i<768; i++) 
    {
        bmp_8[i] = 0x01;
    }

    color_pal[menu_color_pal_idx][0x01] = 0xF000;

    memcpy(&vram[VramAddr(0x0007+0x0002, 0x0101+0x0001)], bmp_8, sizeof(bmp_8));

    for(int j=0; j<24; j++) {
        for(int i=0; i<32; i++) {
            assert(vram[VramAddr(0x0007+0x0002, 0x0101+0x0001)+i+32*j] == 0x01);
        }
    }
    unsigned short cmds_1[] = {
        (0x03 << 10) | (0x01), // Linear
        (0x06 << 10) | (0x00),
        (0x07 << 10) | (0x080),
        (0x08 << 10) | (0x207),
        (0x09 << 10) | (0x01),
        (0x0A << 10) | (0x02),
        (0x0B << 10) | (0x00),
        (0x0C << 10) | (0x00),
        (0x0D << 10) | (0x00),
        (0x0E << 10) | (0x02),
        (0x0F << 10) | (0x01),
        (0x10 << 10) | (0x1F),
        (0x11 << 10) | (0x17),
        (0x01 << 10) | (0x02)
    };

    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();

    assert(menu_transf_mode1 == 0x0000);
    assert(menu_source_form == 0x0001);
    assert(menu_source_loc.offset_addr.x == 0x0007);
    assert(menu_source_loc.offset_addr.y == 0x0101);
    assert(menu_source_loc.point_addr.y == 0x0001);
    assert(menu_source_loc.point_addr.x == 0x0002);
    assert(menu_dest_loc.offset_addr.x == 0x0000);
    assert(menu_dest_loc.offset_addr.y == 0x0000);
    assert(menu_dest_loc.point_addr.y == 0x0002);
    assert(menu_dest_loc.point_addr.x == 0x0001);
    assert(menu_transf_area.y == 0x0017);
    assert(menu_transf_area.x == 0x001F);

    assert(ExecuteMenuDraw() == 0x0000); // Reset flag after executing menu command.

    for(int j=0; j<menu_transf_area.y; j++) {
        for(int i=0; i<menu_transf_area.x; i++) {
            assert(vram[VramAddr(TransfPoint(menu_source_loc).x, TransfPoint(menu_source_loc).y)+i+menu_transf_area.x*j] == 0x01);
        }
    }
  
    for(int j=0; j<menu_transf_area.y; j++) {
        for(int i=0; i<menu_transf_area.x; i++) {
            assert(vram[VramAddr(TransfPoint(menu_dest_loc).x+i, TransfPoint(menu_dest_loc).y+j)] == 0x01);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Linear transfer test
    ///////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<768; i++) {
        bmp_8[i] = 0x02;
    }

    for(int j=0; j<menu_transf_area.y; j++) {
        for(int i=0; i<menu_transf_area.x; i++) {
            vram[VramAddr(TransfPoint(menu_source_loc).x+i, TransfPoint(menu_source_loc).y+j)] = bmp_8[i + menu_transf_area.x*j];
        }
    }

    unsigned short cmds_2[] = {
        (0x03 << 10) | (0x00), // Point
        (0x01 << 10) | (0x02)
    };

    PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
    PF_GDC_DRV_FlushMenuCommand();

    assert(menu_transf_mode1 == 0x0000);
    assert(menu_source_form == 0x0000);
    assert(ExecuteMenuDraw() == 0x0000); // Reset flag after executing menu command.

    for(int j=0; j<menu_transf_area.y; j++) {
        for(int i=0; i<menu_transf_area.x; i++) {
            assert(vram[VramAddr(TransfPoint(menu_source_loc).x+i, TransfPoint(menu_source_loc).y+j)] == 0x02);
        }
    }
  
    for(int j=0; j<menu_transf_area.y; j++) {
        for(int i=0; i<menu_transf_area.x; i++) {
            assert(vram[VramAddr(TransfPoint(menu_dest_loc).x+i, TransfPoint(menu_dest_loc).y+j)] == 0x02);
        }
    }
    
    printf("TestTransf8bitRect() is tested sucessfully!\n");

}

void test_main() 
{
    TestCommand();
    TestSetMenuExecute();
    TestSetScreenFlip();
    TestSetMenuCommonSetup();
    TestSetMenuForeColor();
    TestSetMenuBackColor();
    TestSetMenuSourceOffset();
    TestSetMenuSourcePoint();
    TestSetMenuDestOffset();
    TestSetMenuDestPoint();
    TestSetMenuTransfArea();
    TestSetMenuIntFlag();
    TestSetFontType();
    TestSetFontCharacterCode();
    TestSetColorPaletteAddr();
    TestSetColorPaletteData();
    TestStartDrawString();
    TestSetPlaneID();
    TestVramAddr();
    TestTransf8bitRect();
    
    printf("test_main() is tested sucessfully!\n");
}

void test_draw_8bit(unsigned char color)
{
    std::cout << "Start 8bit" << std::endl;
    unsigned char bmp_8[768] = {0};
    for(int i=0; i<768; i++) {
        bmp_8[i] = color;
    }

    memcpy(&vram[VramAddr(0x0008+0x0002, 0x0101+0x0001)], bmp_8, sizeof(bmp_8));

    unsigned short cmds_1[] = {
        (0x16 << 10) | (0x01), 
        (0x17 << 10) | (0xFF), 
        (0x18 << 10) | (0x00), 
        (0x19 << 10) | (0x00), 
        (0x03 << 10) | (0x01), // Linear
        (0x06 << 10) | (0x00),
        (0x07 << 10) | (0x80),
        (0x08 << 10) | (0x208),
        (0x09 << 10) | (0x01),
        (0x0A << 10) | (0x02),
        (0x0B << 10) | (0x00),
        (0x0C << 10) | (0x00),
        (0x0D << 10) | (0x00),
        (0x0E << 10) | (0x00),
        (0x0F << 10) | (0x00),
        (0x10 << 10) | (0x20),
        (0x11 << 10) | (0x18),
        (0x01 << 10) | (0x02),
    };


    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();

    for(unsigned short i=0; i<200; i++) {
        unsigned short cmds_2[] = {
            (0x0E << 10) | (0x02+i),
            (0x0F << 10) | (0x01+i),
            (0x01 << 10) | (0x02)
        };
        PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
        PF_GDC_DRV_FlushMenuCommand();
    }
    
    printf("test_draw_8bit() is tested sucessfully!\n");
}

void test_draw_font(unsigned char back_color, unsigned char fore_color) 
{
    std::cout << "Start font" << std::endl;
    unsigned char ch[] = { 
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x0F, 0x80,
        0x1F, 0xC0,
        0x30, 0xE0,
        0x30, 0x60,
        0x00, 0x60,
        0x00, 0xC0,
        0x01, 0x80,
        0x03, 0x00,
        0x06, 0x00,
        0x0C, 0x00,
        0x18, 0x00,
        0x30, 0x00,
        0x3F, 0xE0,
        0x3F, 0xE0,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00
    };
    //  000000000000
    //  000000000000
    //  000000000000
    //  000000000000
    //  000000000000
    //  000011111000
    //  000111111100
    //  001100001110
    //  001100000110
    //  000000000110
    //  000000001100
    //  000000011000
    //  000000110000
    //  000001100000
    //  000011000000
    //  000110000000
    //  001100000000
    //  001111111110
    //  001111111110
    //  000000000000
    //  000000000000
    //  000000000000
    //  000000000000
    //  000000000000
    
    memcpy(&vram[VramAddr(0, 0x0101+0x0001)], ch, sizeof(ch));
    memcpy(&vram[VramAddr(0, 0x0101+0x0001)+sizeof(ch)], ch, sizeof(ch));
    for(int i=0; i<36; i++) {
        assert(vram[VramAddr(0, 0x0101+0x0001)+i] == vram[VramAddr(0, 0x0101+0x0001)+sizeof(ch)+i]);
    }
    pf_font_size_t size = {16, 24};
    static unsigned short char_code_width[32] = {0};
    char_code_width[0] = 12;
    char_code_width[1] = 12;

    RegistFont(0, VramAddr(0, 0x0101+0x0001), size, 1, (unsigned int)char_code_width);
    RegistFont(1, VramAddr(0, 0x0101+0x0001), size, 0, (unsigned int)char_code_width);

    unsigned short cmds_3[] = {
        (0x04 << 10) | (back_color),
        (0x05 << 10) | (fore_color),
        (0x13 << 10) | (0x00),
        (0x14 << 10) | (0x00),
        (0x15 << 10) | (0x00),
        (0x16 << 10) | (0x02), 
        (0x17 << 10) | (0x00), 
        (0x18 << 10) | (0x00), // おまけ
        (0x19 << 10) | (0xFF), // おまけ
        (0x16 << 10) | (0x03), 
        (0x17 << 10) | (0x00), 
        (0x18 << 10) | (0xFF), 
        (0x19 << 10) | (0x00), 
        (0x01 << 10) | (0x08)
    };

    PF_GDC_DRV_SetMenuCommand(cmds_3, sizeof(cmds_3) / sizeof(cmds_3[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(font_type == 0x0000);
    assert(font_character_code == 0x0000);
    
    printf("test_draw_font() is tested sucessfully!\n");
}
    
void test_draw_string()
{
    // Propotional
    // Basic string
    unsigned short cmds_1[] = {
        (0x04 << 10) | (0x02),
        (0x05 << 10) | (0x03),
        (0x13 << 10) | (0x00),
        (0x14 << 10) | (0x00),
        (0x15 << 10) | (0x00),
        (0x16 << 10) | (0x02), 
        (0x17 << 10) | (0x00), 
        (0x18 << 10) | (0x00), // おまけ
        (0x19 << 10) | (0xFF), // おまけ
        (0x16 << 10) | (0x03), 
        (0x17 << 10) | (0x00), 
        (0x18 << 10) | (0xFF), 
        (0x19 << 10) | (0x00), 
        (0x3E << 10) | (0x00),
        (0x01),
        (0x01),
        (0x00),
    };
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();

    sleep(1);
    // String margin
    unsigned short cmds_2[] = {
        (0x3E << 10) | (0x10),
        (0x01),
        (0x01),
        (0x00),
    };
    PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
    PF_GDC_DRV_FlushMenuCommand();

    sleep(1);
    // Fixed width
    unsigned short cmds_3[] = {
        (0x13 << 10) | (0x01),
        (0x3E << 10) | (0x00),
        (0x01),
        (0x01),
        (0x00),
    };
    PF_GDC_DRV_SetMenuCommand(cmds_3, sizeof(cmds_3) / sizeof(cmds_3[0]));
    PF_GDC_DRV_FlushMenuCommand();
    
    
    printf("test_draw_string() is tested sucessfully!\n");
}

void test_draw_fill_rect(unsigned char color)
{
    unsigned short cmds_1[] = {
        (0x04 << 10) | color,
        (0x0B << 10) | (0x00),
        (0x0C << 10) | (0x00),
        (0x0D << 10) | (0x00),
        (0x0E << 10) | (0x00),
        (0x0F << 10) | (0x00),
        (0x10 << 10) | (0x3F),
        (0x11 << 10) | (0x0B),
        (0x01 << 10) | (0x01),
    };
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();

    
    printf("test_draw_fill_rect() is tested sucessfully!\n");
}

void test_flip()
{
    sleep(1);
    unsigned short cmds_1[] = {
        (0x02 << 10) | (0x01),
    };
    assert(getLcdFixedY() == 0x0000);
    assert(scr_flip == 0x0000);
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(getLcdFixedY() == 0x200);
    assert(scr_flip == 0x0001);

    sleep(1);
    cmds_1[0] = (0x02 << 10) | (0x00);
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();
    assert(getLcdFixedY() == 0x0000);
    assert(scr_flip == 0x0000);

    // 転送先自動指定テスト
    unsigned short cmds_2[] = {
        (0x03 << 10) | (0x02),
        (0x04 << 10) | (0x03),
        (0x0B << 10) | (0x00),
        (0x0C << 10) | (0x00),
        (0x0D << 10) | (0x00),
        (0x0E << 10) | (0x00),
        (0x0F << 10) | (0x00),
        (0x10 << 10) | (0x3F),
        (0x11 << 10) | (0x0B),
        (0x01 << 10) | (0x01),
    };
    PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
    PF_GDC_DRV_FlushMenuCommand();

    sleep(1);
    cmds_1[0] = (0x02 << 10) | (0x01);
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();

    cmds_2[5] = {
        (0x0E << 10) | (0x50),
    };
    PF_GDC_DRV_SetMenuCommand(cmds_2, sizeof(cmds_2) / sizeof(cmds_2[0]));
    PF_GDC_DRV_FlushMenuCommand();

    sleep(1);
    cmds_1[0] = (0x02 << 10) | (0x00);
    PF_GDC_DRV_SetMenuCommand(cmds_1, sizeof(cmds_1) / sizeof(cmds_1[0]));
    PF_GDC_DRV_FlushMenuCommand();

    
    printf("test_flip() is tested sucessfully!\n");

}

void test_transparent()
{
    PF_GDC_DRV_SetTransparentColor(0x10);
    assert(transparent_color == 0x10);
    test_draw_8bit(0x10);
    test_draw_font(0x02, 0x10);
    test_draw_fill_rect(0x10);
    
    printf("test_transparent() is tested sucessfully!\n");
}

void test_draw() {
    sleep(3);
    test_draw_8bit(0x01);
    test_draw_font(0x02, 0x03);
    sleep(1);
    test_draw_string();
    test_draw_fill_rect(0x03);
    test_flip();
    sleep(1);
    test_draw_8bit(0x02);
    sleep(1);
    test_draw_font(0x01, 0x03);
    sleep(1);
    sleep(1);
    test_transparent();
    
    printf("test_draw() is tested sucessfully!\n");
}