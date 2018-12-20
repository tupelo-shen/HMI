#ifndef __TEST_SIM_h__
#define __TEST_SIM_h__

void TestSetMenuExecute();
void TestCommand();
void TestSetScreenFlip();
void TestSetMenuCommonSetup();
void TestSetMenuForeColor();
void TestSetMenuSourceOffset();
void TestSetMenuSourcePoint();
void TestSetMenuDestOffset();
void TestSetMenuDestPoint();
void TestSetMenuTransfArea();
void TestSetFontType();
void TestSetFontCharacterCode();
void TestSetColorPaletteAddr();
void TestSetColorPaletteData();
void TestRegistFont();
void TestSetTransparentColor();
void TestSetPlaneID();
void ThestVramAddr();
void TestTransf8bitRect();
void test_main();
void test_draw();
void test_draw_8bit(unsigned char color);
void test_draw_font(unsigned char back_color, unsigned char fore_color);
void test_draw_string();
void test_draw_fill_rect(unsigned char color);
void test_flip();
void test_transparent();
#endif 
