#include "sclDrawCom.h"

/* いずれ移動予定の定义 */
#define SC_PLANE_WIDTH      (512)
#define SC_PLANE_HEIGHT     (512)

/* 描画実行内容定义(Com 0x01用) */
#define COM01_FILL_RECT     (0x0001)    // 矩形フィル実行(単色塗りつぶし)
#define COM01_MOVE_8BITRECT (0x0002)    // 8bitデータの矩形領域転送
#define COM01_MOVE_1BITRECT (0x0004)    // 1bitデータの矩形領域転送
#define COM01_MOVE_1BITSTR  (0x0008)    // 1bitデータの矩形領域転送(文字コード)

/* Command 0x03用定义 */
#define COM03_DEST_NONE     (0x0c00)    // 目标地址自动指定
#define COM03_DEST_DISP     (0x0c02)    // 目标地址自动指定表示plane
#define COM03_DATA_COORD    (0x0000)    // 源地址数据形式 坐标
#define COM03_DATA_LINEAR   (0x0001)    // 源地址数据形式 linear

/* Command 0x3f用定义 */
#define COM3f_SRC_PLANE     (0x0001)    // 転送元プレーン指定
#define COM3f_DEST_PLANE    (0x0002)    // 転送先プレーン指定

/*** 裏画面指定用command &VRAMアドレス定义 ***/
/* BitBlt転送元座標設定オフセット makeCom060708D2U用 */
// 表示plane (表示用))　X:0x000  Y:0x00000
#define DISPLAY_OFSET06 (0x1800)    // command 0x06
#define DISPLAY_OFSET07 (0x1c00)    // command 0x07
#define DISPLAY_OFSET08 (0x2000)    // command 0x08
// 表示plane (更新用))　X:0x000  Y:0x00200
#define UPDATE_OFSET06  (0x1800)    // command 0x06
#define UPDATE_OFSET07  (0x1d00)    // command 0x07
#define UPDATE_OFSET08  (0x2000)    // command 0x08
// 表示plane (trend 1用)　X:0x000  Y:0x00400
#define TREND1_OFSET06  (0x1800)    // command 0x06
#define TREND1_OFSET07  (0x1e00)    // command 0x07
#define TREND1_OFSET08  (0x2000)    // command 0x08
// 表示plane (trend 2用)　X:0x000  Y:0x00600
#define TREND2_OFSET06  (0x1800)    // command 0x06
#define TREND2_OFSET07  (0x1f00)    // command 0x07
#define TREND2_OFSET08  (0x2000)    // command 0x08
// 表示plane (list 用)　X:0x000  Y:0x00800
#define LIST_OFSET06    (0x1801)    // command 0x06
#define LIST_OFSET07    (0x1c00)    // command 0x07
#define LIST_OFSET08    (0x2000)    // command 0x08
// 表示plane (Scroll area用)　X:0x000  Y:0x00a00
#define SCRAREA_OFSET06 (0x1801)    // command 0x06
#define SCRAREA_OFSET07 (0x1d00)    // command 0x07
#define SCRAREA_OFSET08 (0x2000)    // command 0x08
/* BitBlt転送先座標設定オフセット */
// 自動設定(表示plane (更新用))　X:0x000  Y:0x00000
#define UPDATE_OFSET0B  (0x2c00)    // command 0x0b
#define UPDATE_OFSET0C  (0x3000)    // command 0x0c
#define UPDATE_OFSET0D  (0x3400)    // command 0x0d
// trend 裏画面1用　X:0x000  Y:0x00400
#define TREND1_OFSET0B  (0x2c00)    // command 0x0b
#define TREND1_OFSET0C  (0x3200)    // command 0x0c
#define TREND1_OFSET0D  (0x3400)    // command 0x0d
// trend 裏画面2用　X:0x000  Y:0x00600
#define TREND2_OFSET0B  (0x2c00)    // command 0x0b
#define TREND2_OFSET0C  (0x3300)    // command 0x0c
#define TREND2_OFSET0D  (0x3400)    // command 0x0d
// list 裏画面用　X:0x000  Y:0x00800
#define LIST_OFSET0B    (0x2c01)    // command 0x0b
#define LIST_OFSET0C    (0x3000)    // command 0x0c
#define LIST_OFSET0D    (0x3400)    // command 0x0d
// スクロールエリア裏画面用　X:0x000  Y:0x00a00
#define SCRAREA_OFSET0B (0x2c01)    // command 0x0b
#define SCRAREA_OFSET0C (0x3100)    // command 0x0c
#define SCRAREA_OFSET0D (0x3400)    // command 0x0d

/****************************************************************************/
/**
 * @brief   makeLine
 * 直線描画用のCommand を作成する
 *
 * @author  A.Kubota
 * @date    2016/04/22
 *
 * @param[in]   SCDRAWPARAM&    param               描画依赖的参数
 * @param[out]  unsigned short* draw_command_buf    描画命令的buffer
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           直線(width or heightを1にする)
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       内画面1
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    trend内画面１
 *                                    SC_TREND_WORK2    trend内画面2
 *                                    SC_LIST_WORK      List内画面
 *                                    SC_SCRAREA_WORK   滚动区域内画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    未使用
 *  int             target_ID;      未使用
 *  unsigned short* str;            未使用
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      未使用
 *  unsigned short  distance;       未使用
 *  unsigned char   fore_color;     线颜色
 *  unsigned char   back_color;     未使用
 */
/****************************************************************************/
int SCDrawCom::makeLine(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    return(makeFillRect(param.work_scr, param.fore_color, param.rect,
            draw_command_buf));
}

// ----------------------< !内部函数 >----------------------------------------
int SCDrawCom::makeFillRect(
    unsigned int    work_scr,
    unsigned char   color,
    SCRect&         rect,
    unsigned short* buf
)
{
    unsigned short* p_buf;


    // command 保存位置
    p_buf = buf;


    // 共同设置(0x03)
    *p_buf++ = makeCom03(work_scr, COM03_DATA_COORD);

    // 指定颜色（fore）(0x05)
    *p_buf++ = makeCom05(color);

    // BitBlt 目标地址坐标设置 offset 2→1→0
    p_buf += makeCom0b0c0d(work_scr, p_buf);

    // BitBlt 目标地址坐标设置 Ｙ坐标(0x0e)
    *p_buf++ = makeCom0e(rect.y);

    // BitBlt 目标地址坐标设置 X坐标(0x0f)
    *p_buf++ = makeCom0f(rect.x);

    // BitBlt 传输数据区域 width(0x10)
    *p_buf++ = makeCom10(rect.width);

    // BitBlt 传输数据区域 height(0x11)
    *p_buf++ = makeCom11(rect.height);

    // 描画执行寄存器 描画矩形
    *p_buf++ = makeCom01(COM01_FILL_RECT);


    return(p_buf - buf);
}