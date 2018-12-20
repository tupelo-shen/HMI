#ifndef __GDC_SIM_H__
#define __GDC_SIM_H__

#include "Simulator.h"
#include "GDC_Driver.h"
#include "DrawCommand.h"

typedef struct
{
    unsigned short  x;
    unsigned int    y;
} pf_offset_t;

typedef struct 
{
    pf_point_t      point_addr;
    pf_offset_t     offset_addr;
} menu_bitblt_loc;

typedef struct
{
    uint32_t        address25to20 : 6;
    uint32_t        address19to10 : 10;
    uint32_t        address09to00 : 10;
} SplitAddress;

const int size_color_palette = 256;
const int size_color_pal_idx = 2;

unsigned int    VramAddr(unsigned int x, unsigned y);
pf_point_t      VramPoint(unsigned int addr);
pf_point_t      TransfPoint(menu_bitblt_loc loc);
unsigned long   getBitBltFixedY(void);
unsigned long   getLcdFixedY();
void            setSim(Simulator *sim);
Simulator*      getSim(void);
unsigned short  ExecuteMenuDraw(void);
/******************************************************************************/
/*!
\brief    LCD周期刷新
\details  将VRAM的显示plate区域传输到LCD。
\return   なし
*******************************************************************************/
void            UpdateLcd(void);

/******************************************************************************/
/*!
\brief    填充矩形
\details  将Menu绘图设定的color寄存器的指定颜色（fore）拷贝到目标地址
\return   None
*******************************************************************************/
void            FillRect(void);

/******************************************************************************/
/*!
\brief    传输矩形(1 Bit)
\details  传输1 Bit矩形数据。数据源只能是线性数据。数据源是透明色时，不用填写目标地址
\return   None
*******************************************************************************/
void            Transf1bitRect(void);

/******************************************************************************/
/*!
\brief    传输矩形(文字CODE)
\details  1bit矩形传输文字CODE。数据源只能是线性数据。数据源是透明色时，不用填写目标地址。
\return   なし
*******************************************************************************/
void            Transf1bitFontRect(void);

/******************************************************************************/
/*!
\brief    传输矩形(8 Bits)
\details  传输8bits矩形数据。选择数据源是坐标数据还是线性数据
\return   None
*******************************************************************************/
void            Transf8bitRect(void);

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
void            Transf1bitCommonRect(unsigned short width, 
                                    unsigned short height, 
                                    pf_point_t source, 
                                    pf_point_t dest, 
                                    unsigned short offset_addr=0);

/******************************************************************************/
/*!
\brief     
\details  
\param[in] dat ，        数据
\return   None
*******************************************************************************/
void            RectString(unsigned short dat);


void            SetDrawCommand(DrawCommand _cmd);
bool            ContinueDrawString(void);
bool            ContinueSetPlaneID(void);
void            splitAddress(uint32_t address, SplitAddress *split_address);
void            SetPlaneID(unsigned short dat);

/******************************************************************************/
/*!
\brief    设置执行寄存器
\details  设置多位或0位时，设置无效。
\param[in] 绘图执行命令
\return   成功/失败
*******************************************************************************/
void            SetMenuExecute(unsigned short code);
void            SetScreenFlip(unsigned short code);
void            SetMenuCommonSetup(unsigned short code);
void            SetMenuBackColor(unsigned short color);
void            SetMenuForeColor(unsigned short color);
/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送元オフセットアドレス0設定
\details  メニュー描画BitBlt転送元オフセットアドレス0を設定する。
\param[in] BitBlt転送元オフセットアドレス0
\return   なし
*******************************************************************************/
void            SetMenuSourceOffset0(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送元オフセットアドレス1設定
\details  メニュー描画BitBlt転送元オフセットアドレス1を設定する。
\param[in] BitBlt転送元オフセットアドレス1
\return   なし
*******************************************************************************/
void            SetMenuSourceOffset1(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送元オフセットアドレス2設定
\details  メニュー描画BitBlt転送元オフセットアドレス2を設定する。
\param[in] BitBlt転送元オフセットアドレス2
\return   なし
*******************************************************************************/
void            SetMenuSourceOffset2(unsigned short loc);
/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送先オフセットアドレス0設定
\details  メニュー描画BitBlt転送先オフセットアドレス0を設定する。
\param[in] BitBlt転送先オフセットアドレス0
\return   なし
*******************************************************************************/
void            SetMenuDestOffset0(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送先オフセットアドレス1設定
\details  メニュー描画BitBlt転送先オフセットアドレス1を設定する。
\param[in] BitBlt転送先オフセットアドレス1
\return   なし
*******************************************************************************/
void            SetMenuDestOffset1(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送先オフセットアドレス2設定
\details  メニュー描画BitBlt転送先オフセットアドレス2を設定する。
\param[in] BitBlt転送先オフセットアドレス2
\return   なし
*******************************************************************************/
void            SetMenuDestOffset2(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送元X座標設定
\details  メニュー描画BitBlt転送元X座標を設定する。
\param[in] BitBlt転送元X座標
\return   なし
*******************************************************************************/
void            SetMenuSourcePointX(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送元Y座標設定
\details  メニュー描画BitBlt転送元Y座標を設定する。
\param[in] BitBlt転送元Y座標
\return   なし
*******************************************************************************/
void            SetMenuSourcePointY(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送先X座標設定
\details  メニュー描画BitBlt転送先X座標を設定する。
\param[in] BitBlt転送先X座標
\return   なし
*******************************************************************************/
void            SetMenuDestPointX(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送先Y座標設定
\details  メニュー描画BitBlt転送先Y座標を設定する。
\param[in] BitBlt転送先Y座標
\return   なし
*******************************************************************************/
void            SetMenuDestPointY(unsigned short loc);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送データ幅設定
\details  メニュー描画BitBlt転送データ幅を設定する。
\param[in] BitBlt転送データ幅
\return   なし
*******************************************************************************/
void            SetMenuTransfAreaWidth(unsigned short width);

/******************************************************************************/
/*!
\brief    メニュー描画BitBlt転送データ高設定
\details  メニュー描画BitBlt転送データ高を設定する。
\param[in] BitBlt転送データ高
\return   なし
*******************************************************************************/
void            SetMenuTransfAreaHeight(unsigned short height);

/******************************************************************************/
/*!
\brief    メニュー描画割込みフラグ設定
\details  メニュー描画割込みフラグを設定する。
\param[in] メニュー描画割込みフラグ
\return   なし
*******************************************************************************/
void            SetMenuIntFlag(unsigned short _gdc_int);

/******************************************************************************/
/*!
\brief    フォント描画
\details  文字列種類指定
\param[in] 文字列種類
\return   なし
*******************************************************************************/
void            SetFontType(unsigned short _font_type);

/******************************************************************************/
/*!
\brief    フォント描画
\details  文字コード指定(上位ビット)
\param[in] 文字コード(上位ビット)
\return   なし
*******************************************************************************/
void            SetFontCharacterHigherCode(unsigned short char_code);

/******************************************************************************/
/*!
\brief    フォント描画
\details  文字コード指定(下位ビット)
\param[in] 文字コード(下位ビット)
\return   なし
*******************************************************************************/
void            SetFontCharacterLowerCode(unsigned short char_code);

/******************************************************************************/
/*!
\brief    8ビットカラーパレット書込み
\details  8ビットカラーパレット設定
\param[in] カラーパレットアドレス
\return   なし
*******************************************************************************/
void            SetColorPaletteAddr(unsigned short addr);

/******************************************************************************/
/*!
\brief    8ビットカラーパレット書込み
\details  8ビットカラーパレットデータ[15:11]設定
\param[in] カラーパレットデータ
\return   なし
*******************************************************************************/
void            SetColorPaletteDataR(unsigned short dat);

/******************************************************************************/
/*!
\brief    8ビットカラーパレット書込み
\details  8ビットカラーパレットデータ[10:5]設定＆書込み実行
\param[in] カラーパレットデータ
\return   なし
*******************************************************************************/
void            SetColorPaletteDataG(unsigned short dat);

/******************************************************************************/
/*!
\brief    8ビットカラーパレット書込み
\details  8ビットカラーパレットデータ[4:0]設定＆書込み実行
\param[in] カラーパレットデータ
\return   なし
*******************************************************************************/
void            SetColorPaletteDataB(unsigned short dat);

/******************************************************************************/
/*!
\brief    文字列描画
\details  文字列描画を開始する。次のコマンドからコマンドデータ0x0000がセットされるまで
          文字描画を続ける
\return   なし
*******************************************************************************/
void            StartDrawString(unsigned short dat);

void            StartSetPlaneID(unsigned short dat);

/******************************************************************************/
/*!
\brief    カラーパレットをRAMに書込む
\details  8ビットカラーパレットデータ[9:0]書込み後にこの関数をコールする。
\return   なし
*******************************************************************************/
void            WriteColorPalette(void);

void            FlipPalette(unsigned int);
void            FlipScreen(unsigned int screen);
void            GetPaletteAndScreen(unsigned int *palette, unsigned int *screen);
unsigned int    GetPalette();
unsigned int    GetScreen();

void            RegistFont(unsigned int     font_id, 
                            unsigned int    vram_offset, 
                            pf_font_size_t  font_size, 
                            unsigned int    width_info, 
                            unsigned int    width_address);
void            setTransparentColor(unsigned char color);
#endif /* __GDC_SIM_H__ */
