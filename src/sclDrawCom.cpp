#include "HMI_Resource.h"
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

enum {
    SC_BIGFONT_TYPE_XL = 0,
    SC_BIGFONT_TYPE_L,
    SC_BIGFONT_TYPE_M,
    SC_BIGFONT_TYPE_S,
    SC_BIGFONT_TYPE_XS,
    SC_BIGFONT_TYPE_EMC_L,
    SC_BIGFONT_TYPE_EMC_S,
};

typedef struct {
    int             fontID;
    unsigned short  char_code;
} SC_BIG_FONT_TBL;

// static SC_BIG_FONT_TBL const sc_big_font_xl_tbl[] = {
//     {IDB_BIGFONT_XL_MINUS,  0x002d},
//     {IDB_BIGFONT_XL_PERIOD, 0x002e},
//     {IDB_BIGFONT_XL_0,      0x0030},
//     {IDB_BIGFONT_XL_1,      0x0031},
//     {IDB_BIGFONT_XL_2,      0x0032},
//     {IDB_BIGFONT_XL_3,      0x0033},
//     {IDB_BIGFONT_XL_4,      0x0034},
//     {IDB_BIGFONT_XL_5,      0x0035},
//     {IDB_BIGFONT_XL_6,      0x0036},
//     {IDB_BIGFONT_XL_7,      0x0037},
//     {IDB_BIGFONT_XL_8,      0x0038},
//     {IDB_BIGFONT_XL_9,      0x0039},
//     {-1,                    0xffff}     // ターミネータ
// };

// static SC_BIG_FONT_TBL const sc_big_font_l_tbl[] = {
//     {IDB_BIGFONT_L_MINUS,   0x002d},
//     {IDB_BIGFONT_L_PERIOD,  0x002e},
//     {IDB_BIGFONT_L_0,       0x0030},
//     {IDB_BIGFONT_L_1,       0x0031},
//     {IDB_BIGFONT_L_2,       0x0032},
//     {IDB_BIGFONT_L_3,       0x0033},
//     {IDB_BIGFONT_L_4,       0x0034},
//     {IDB_BIGFONT_L_5,       0x0035},
//     {IDB_BIGFONT_L_6,       0x0036},
//     {IDB_BIGFONT_L_7,       0x0037},
//     {IDB_BIGFONT_L_8,       0x0038},
//     {IDB_BIGFONT_L_9,       0x0039},
//     {-1,                    0xffff}     // ターミネータ
// };

// static SC_BIG_FONT_TBL const sc_big_font_m_tbl[] = {
//     {IDB_BIGFONT_M_MINUS,   0x002d},
//     {IDB_BIGFONT_M_PERIOD,  0x002e},
//     {IDB_BIGFONT_M_0,       0x0030},
//     {IDB_BIGFONT_M_1,       0x0031},
//     {IDB_BIGFONT_M_2,       0x0032},
//     {IDB_BIGFONT_M_3,       0x0033},
//     {IDB_BIGFONT_M_4,       0x0034},
//     {IDB_BIGFONT_M_5,       0x0035},
//     {IDB_BIGFONT_M_6,       0x0036},
//     {IDB_BIGFONT_M_7,       0x0037},
//     {IDB_BIGFONT_M_8,       0x0038},
//     {IDB_BIGFONT_M_9,       0x0039},
//     {-1,                    0xffff}     // ターミネータ
// };

// static SC_BIG_FONT_TBL const sc_big_font_s_tbl[] = {
//     {IDB_BIGFONT_S_MINUS,   0x002d},
//     {IDB_BIGFONT_S_PERIOD,  0x002e},
//     {IDB_BIGFONT_S_0,       0x0030},
//     {IDB_BIGFONT_S_1,       0x0031},
//     {IDB_BIGFONT_S_2,       0x0032},
//     {IDB_BIGFONT_S_3,       0x0033},
//     {IDB_BIGFONT_S_4,       0x0034},
//     {IDB_BIGFONT_S_5,       0x0035},
//     {IDB_BIGFONT_S_6,       0x0036},
//     {IDB_BIGFONT_S_7,       0x0037},
//     {IDB_BIGFONT_S_8,       0x0038},
//     {IDB_BIGFONT_S_9,       0x0039},
//     {-1,                    0xffff}     // ターミネータ
// };

// static SC_BIG_FONT_TBL const sc_emc_big_font_l_tbl[] = {
//     {IDB_EMC_BF_L_MINUS,    0x002d},
//     {IDB_EMC_BF_L_PERIOD,   0x002e},
//     {IDB_EMC_BF_L_0,        0x0030},
//     {IDB_EMC_BF_L_1,        0x0031},
//     {IDB_EMC_BF_L_2,        0x0032},
//     {IDB_EMC_BF_L_3,        0x0033},
//     {IDB_EMC_BF_L_4,        0x0034},
//     {IDB_EMC_BF_L_5,        0x0035},
//     {IDB_EMC_BF_L_6,        0x0036},
//     {IDB_EMC_BF_L_7,        0x0037},
//     {IDB_EMC_BF_L_8,        0x0038},
//     {IDB_EMC_BF_L_9,        0x0039},
//     {-1,                    0xffff}     // ターミネータ
// };

// static SC_BIG_FONT_TBL const sc_emc_big_font_s_tbl[] = {
//     {IDB_EMC_BF_S_PERCENT,  0x0025},
//     {IDB_EMC_BF_S_MINUS,    0x002d},
//     {IDB_EMC_BF_S_PERIOD,   0x002e},
//     {IDB_EMC_BF_S_SLASH,    0x002f},
//     {IDB_EMC_BF_S_0,        0x0030},
//     {IDB_EMC_BF_S_1,        0x0031},
//     {IDB_EMC_BF_S_2,        0x0032},
//     {IDB_EMC_BF_S_3,        0x0033},
//     {IDB_EMC_BF_S_4,        0x0034},
//     {IDB_EMC_BF_S_5,        0x0035},
//     {IDB_EMC_BF_S_6,        0x0036},
//     {IDB_EMC_BF_S_7,        0x0037},
//     {IDB_EMC_BF_S_8,        0x0038},
//     {IDB_EMC_BF_S_9,        0x0039},
//     {IDB_EMC_BF_S_A,        0x0041},
//     {IDB_EMC_BF_S_C,        0x0043},
//     {IDB_EMC_BF_S_F,        0x0046},
//     {IDB_EMC_BF_S_G,        0x0047},
//     {IDB_EMC_BF_S_H,        0x0048},
//     {IDB_EMC_BF_S_L,        0x004c},
//     {IDB_EMC_BF_S_M,        0x004d},
//     {IDB_EMC_BF_S_S,        0x0053},
//     {IDB_EMC_BF_S_V,        0x0056},
//     {IDB_EMC_BF_S_b_s,      0x0062},
//     {IDB_EMC_BF_S_c_s,      0x0063},
//     {IDB_EMC_BF_S_g_s,      0x0067},
//     {IDB_EMC_BF_S_k_s,      0x006b},
//     {IDB_EMC_BF_S_m_s,      0x006d},
//     {IDB_EMC_BF_S_n_s,      0x006e},
//     {IDB_EMC_BF_S_p_s,      0x0070},
//     {IDB_EMC_BF_S_r_s,      0x0072},
//     {IDB_EMC_BF_S_t_s,      0x0074},
//     {IDB_EMC_BF_S_DEGREE,   0x00b0},
//     {IDB_EMC_BF_S_MICRO,    0x00b5},
//     {IDB_EMC_BF_S_OHM,      0x03a9},
//     {-1,                    0xffff}     // ターミネータ
// };
/****************************************************************************/
/**
 * @brief   makeLine
 * 直線描画用のCommand を作成する
 *
 * @author  ： 
 * @date    ： 
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
/****************************************************************************/
/**
 * @brief   makeRect
 * 矩形描画用の描画コマンドを作成する
 *
 * @author  A.Kubota
 * @date    2016/04/22
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           矩形領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    未使用
 *  int             target_ID;      未使用
 *  unsigned short* str;            未使用
 *  bool            frame;          枠線あり(true)/なし(false)
 *  bool            fill;           塗りつぶしあり(true)/なし(false)
 *  unsigned short  font_type;      未使用
 *  unsigned short  distance;       未使用
 *  unsigned char   fore_color;     枠線の色
 *  unsigned char   back_color;     塗りつぶしの色
 */
/****************************************************************************/
int SCDrawCom::makeRect(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    unsigned short* p_buf;


    // command 保存位置
    p_buf = draw_command_buf;


    if(param.fill) {    // 填充
        p_buf += makeFillRect(param.work_scr, param.back_color, param.rect,
                p_buf);
    }


    if(param.frame) {   // 带边框
        SCRect          lineRect;

        // (left, top)-(right, top)
        lineRect.Set(param.rect.x, param.rect.y, param.rect.width, 1);
        p_buf += makeFillRect(param.work_scr, param.fore_color, lineRect,
                p_buf);

        // (left, top)-(left, bottom)
        lineRect.Set(param.rect.x, param.rect.y, 1, param.rect.height);
        p_buf += makeRepeatFillRect(lineRect, p_buf);

        // (right, top)-(right, bottom)
        lineRect.Set(param.rect.Right(), param.rect.y, 1, param.rect.height);
        p_buf += makeRepeatFillRect(lineRect, p_buf);

        // (left, bottom)-(right, bottom)
        lineRect.Set(param.rect.x, param.rect.Bottom(), param.rect.width, 1);
        p_buf += makeRepeatFillRect(lineRect, p_buf);
    }


    return(p_buf - draw_command_buf);
}

/****************************************************************************/
/**
 * @brief   makeFixedString
 * 固定(登録済み)文字列の描画
 *
 * @author  A.Kubota
 * @date    2016/04/22
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    文字列のX方向の配置
 *                                    SC_ARRANGE_LEFT   左寄せ
 *                                    SC_ARRANGE_CENTER 中央
 *                                    SC_ARRANGE_RIGHT  右寄せ
 *  int             target_ID;      文字列ID
 *  unsigned short* str;            未使用
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      フォントの種類
 *                                    SC_FONT_TYPE_L    ラージフォント
 *                                    SC_FONT_TYPE_M    ミディアムフォント
 *                                    SC_FONT_TYPE_S    スモールフォント
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     文字の色
 *  unsigned char   back_color;     背景の色
 */
/****************************************************************************/
int SCDrawCom::makeFixedString(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    // unsigned short  work_str[SC_MAX_STR_LENGTH];
    // SCDRAWPARAM     work_param;


    // // 文字列データ取得
    // sc_getFixedString(param.target_ID, work_str);

    // // 依頼用パラメータをフリー文字列用に変更
    // work_param = param;
    // work_param.str = work_str;


    // return(makeFreeString(work_param, draw_command_buf));
}

/****************************************************************************/
/**
 * @brief   makeFreeString
 * ユーザ指定文字列の描画
 *
 * @author  A.Kubota
 * @date    2016/04/22
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    文字列のX方向の配置
 *                                    SC_ARRANGE_LEFT   左寄せ
 *                                    SC_ARRANGE_CENTER 中央
 *                                    SC_ARRANGE_RIGHT  右寄せ
 *  int             target_ID;      未使用
 *  unsigned short* str;            文字列(UTF-16)
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      フォントの種類
 *                                    SC_FONT_TYPE_L    ラージフォント
 *                                    SC_FONT_TYPE_M    ミディアムフォント
 *                                    SC_FONT_TYPE_S    スモールフォント
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     文字の色
 *  unsigned char   back_color;     背景の色
 */
/****************************************************************************/
int SCDrawCom::makeFreeString(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    int             font_height, str_width;
    SCPoint         pos;
    unsigned short* p_buf;


    // 字符串表示位置计算
    font_height = getFontHeight(param.font_type);
    str_width = getStringWidth(param.font_type, param.str);
    getTargetPosition(param.arrangement, param.rect, str_width, font_height,
            &pos);


    // command 保存位置
    p_buf = draw_command_buf;


    // 共同设定(0x03)
    *p_buf++ = makeCom03(param.work_scr, COM03_DATA_LINEAR);

    // 指定背景色(0x04)
    *p_buf++ = makeCom04(param.back_color);

    // 指定前景色(0x05)
    *p_buf++ = makeCom05(param.fore_color);

    // 指定文字种类(0x13)
    *p_buf++ = makeCom13(param.font_type);

    // メニュー描画BitBlt転送先座標設定オフセット2→1→0
    p_buf += makeCom0b0c0d(param.work_scr, p_buf);

    // メニュー描画BitBlt転送先座標設定Ｙ座標(0x0e)
    *p_buf++ = makeCom0e(pos.y);

    // メニュー描画BitBlt転送先座標設定Ｘ座標(0x0f)
    *p_buf++ = makeCom0f(pos.x);

    // 文字列データ作成ドライバ専用コマンド(0x3e)
    // 描画実行コマンドは0x3eコマンドを展開時にドライバが付加
    p_buf += makeCom3e(param.str, param.distance, p_buf);


    return(p_buf - draw_command_buf);
}
/****************************************************************************/
/**
 * @brief   makeFixedStringReal
 * 固定(登録済み)文字列を実際のフォントサイズで描画
 *
 * @author  A.Kubota
 * @date    2017/04/12
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    文字列のX方向の配置
 *                                    SC_ARRANGE_LEFT   左寄せ
 *                                    SC_ARRANGE_CENTER 中央
 *                                    SC_ARRANGE_RIGHT  右寄せ
 *  int             target_ID;      文字列ID
 *  unsigned short* str;            未使用
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      フォントの種類
 *                                    SC_FONT_TYPE_L    ラージフォント
 *                                    SC_FONT_TYPE_M    ミディアムフォント
 *                                    SC_FONT_TYPE_S    スモールフォント
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     文字の色
 *  unsigned char   back_color;     背景の色
 */
/****************************************************************************/
int SCDrawCom::makeFixedStringReal(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    unsigned short  work_str[SC_MAX_STR_LENGTH];
    SCDRAWPARAM     work_param;


    // 文字列データ取得
    sc_getFixedString(param.target_ID, work_str);

    // 依頼用パラメータをフリー文字列用に変更＆実際のフォントサイズ取得
    work_param = param;
    work_param.str = work_str;
    // work_param.font_type = SCGetRealFontType(work_param.font_type);


    return(makeFreeString(work_param, draw_command_buf));
}


/****************************************************************************/
/**
 * @brief   makeFreeStringReal
 * ユーザ指定文字列を実際のフォントサイズで描画
 *
 * @author  A.Kubota
 * @date    2017/04/12
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    文字列のX方向の配置
 *                                    SC_ARRANGE_LEFT   左寄せ
 *                                    SC_ARRANGE_CENTER 中央
 *                                    SC_ARRANGE_RIGHT  右寄せ
 *  int             target_ID;      未使用
 *  unsigned short* str;            文字列(UTF-16)
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      フォントの種類
 *                                    SC_FONT_TYPE_L    ラージフォント
 *                                    SC_FONT_TYPE_M    ミディアムフォント
 *                                    SC_FONT_TYPE_S    スモールフォント
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     文字の色
 *  unsigned char   back_color;     背景の色
 */
/****************************************************************************/
int SCDrawCom::makeFreeStringReal(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    SCDRAWPARAM     work_param;


    // 依頼用パラメータのFontTypeを実際のFontTypeに変更
    work_param = param;
    // work_param.font_type = SCGetRealFontType(work_param.font_type);


    return(makeFreeString(work_param, draw_command_buf));
}


/****************************************************************************/
/**
 * @brief   makeBigFontString
 * ユーザ指定文字列の描画
 *
 * @author  A.Kubota
 * @date    2016/04/22
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    文字列のX方向の配置
 *                                    SC_ARRANGE_LEFT   左寄せ
 *                                    SC_ARRANGE_CENTER 中央
 *                                    SC_ARRANGE_RIGHT  右寄せ
 *  int             target_ID;      未使用
 *  unsigned short* str;            文字列(UTF-16)
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      フォントの種類
 *                                    SC_BIGFONT_TYPE_XL     BigFont XL
 *                                    SC_BIGFONT_TYPE_L      BigFont L
 *                                    SC_BIGFONT_TYPE_M      BigFont M
 *                                    SC_BIGFONT_TYPE_S      BigFont S
 *                                    SC_BIGFONT_TYPE_XS     BigFont XS
 *                                    SC_BIGFONT_TYPE_EMC_L  BigFont L
 *                                    SC_BIGFONT_TYPE_EMC_S  BigFont S
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     未使用
 *  unsigned char   back_color;     未使用
 */
/****************************************************************************/
int SCDrawCom::makeBigFontString(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    int             font_height, str_width, char_width;
    int             big_font_ID;
    SCPoint         pos;
    unsigned short* p_buf;


    // 文字列表示位置計算
    // font_height = getBigFontHeight(param.font_type);
    str_width = getBigFontStringWidth(param.font_type, param.distance,
            param.str);
    getTargetPosition(param.arrangement, param.rect, str_width, font_height,
            &pos);


    // コマンド格納位置
    p_buf = draw_command_buf;


    // メニュー描画共通設定(0x03)
    *p_buf++ = makeCom03(param.work_scr, COM03_DATA_LINEAR);

    // メニュー描画BitBlt転送元座標設定Ｙ座標(0x09)
    *p_buf++ = makeCom09((unsigned short)0);

    // メニュー描画BitBlt転送元座標設定Ｘ座標(0x0a)
    *p_buf++ = makeCom0a((unsigned short)0);

    // メニュー描画BitBlt転送先座標設定オフセット2→1→0
    p_buf += makeCom0b0c0d(param.work_scr, p_buf);

    // メニュー描画BitBlt転送先座標設定Ｙ座標(0x0e)
    *p_buf++ = makeCom0e(pos.y);

    // メニュー描画BitBlt転送データ領域設定 height(0x11)
    *p_buf++ = makeCom11(font_height);


    while(*(param.str)) {
        // big_font_ID = getBigFontID(param.font_type, *(param.str));
        char_width = sc_getPictWidth(big_font_ID);

        // プレーンID設定(0x3f)
        p_buf += makeCom3f((unsigned short)COM3f_SRC_PLANE,
                (unsigned short)big_font_ID, p_buf);

        // メニュー描画BitBlt転送先座標設定Ｘ座標(0x0f)
        *p_buf++ = makeCom0f(pos.x);

        // メニュー描画BitBlt転送データ領域設定 width(0x10)
        *p_buf++ = makeCom10(char_width);

        // 描画実行レジスタ 8bitデータの矩形領域転送
        *p_buf++ = makeCom01(COM01_MOVE_8BITRECT);

        (param.str)++;
        pos.x += char_width + param.distance;
    }


    return(p_buf - draw_command_buf);
}


/****************************************************************************/
/**
 * @brief   makePict
 * 画像描画用の描画コマンドを作成する
 *
 * @author  A.Kubota
 * @date    2016/08/03
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           矩形領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    画像のX方向の配置
 *                                    SC_ARRANGE_LEFT   左寄せ
 *                                    SC_ARRANGE_CENTER 中央
 *                                    SC_ARRANGE_RIGHT  右寄せ
 *  int             target_ID;      画像ID
 *  unsigned short* str;            未使用
 *  bool            frame;          枠線あり(true)/なし(false)
 *  bool            fill;           塗りつぶしあり(true)/なし(false)
 *  unsigned short  font_type;      未使用
 *  unsigned short  distance;       未使用
 *  unsigned char   fore_color;     枠線の色
 *  unsigned char   back_color;     塗りつぶしの色
 */
/****************************************************************************/
int SCDrawCom::makePict(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    int             pict_width, pict_height;
    SCPoint         pos;
    unsigned short* p_buf;


    // コマンド格納位置
    p_buf = draw_command_buf;


    if(param.fill) {    // 塗りつぶしあり
        p_buf += makeFillRect(param.work_scr, param.back_color, param.rect,
                p_buf);
    }


    if(param.frame) {   // 枠線あり
        SCRect          lineRect;

        // (left, top)-(right, top)
        lineRect.Set(param.rect.x, param.rect.y, param.rect.width, 1);
        p_buf += makeFillRect(param.work_scr, param.fore_color, lineRect,
                p_buf);

        // (left, top)-(left, bottom)
        lineRect.Set(param.rect.x, param.rect.y, 1, param.rect.height);
        p_buf += makeRepeatFillRect(lineRect, p_buf);

        // (right, top)-(right, bottom)
        lineRect.Set(param.rect.Right(), param.rect.y, 1, param.rect.height);
        p_buf += makeRepeatFillRect(lineRect, p_buf);

        // (left, bottom)-(right, bottom)
        lineRect.Set(param.rect.x, param.rect.Bottom(), param.rect.width, 1);
        p_buf += makeRepeatFillRect(lineRect, p_buf);
    }


    // 画像表示位置計算
    pict_width = sc_getPictWidth(param.target_ID);
    // pict_height = sc_getPictHeight(param.target_ID);
    getTargetPosition(param.arrangement, param.rect, pict_width, pict_height,
        &pos);


    // メニュー描画共通設定(0x03)
    *p_buf++ = makeCom03(param.work_scr, COM03_DATA_LINEAR);

    // プレーンID設定(0x3f) 転送元
    p_buf += makeCom3f((unsigned short)COM3f_SRC_PLANE,
            (unsigned short)param.target_ID, p_buf);

    // メニュー描画BitBlt転送元座標設定Ｙ座標(0x09)
    *p_buf++ = makeCom09((unsigned short)0);

    // メニュー描画BitBlt転送元座標設定Ｘ座標(0x0a)
    *p_buf++ = makeCom0a((unsigned short)0);

    // メニュー描画BitBlt転送先座標設定オフセット2→1→0
    p_buf += makeCom0b0c0d(param.work_scr, p_buf);

    // メニュー描画BitBlt転送先座標設定Ｙ座標(0x0e)
    *p_buf++ = makeCom0e(pos.y);

    // メニュー描画BitBlt転送先座標設定Ｘ座標(0x0f)
    *p_buf++ = makeCom0f(pos.x);

    // メニュー描画BitBlt転送データ領域設定 width(0x10)
    *p_buf++ = makeCom10(pict_width);

    // メニュー描画BitBlt転送データ領域設定 height(0x11)
    *p_buf++ = makeCom11(pict_height);

    // 描画実行レジスタ 8bitデータの矩形領域転送
    *p_buf++ = makeCom01(COM01_MOVE_8BITRECT);


    return(p_buf - draw_command_buf);
}


/****************************************************************************/
/**
 * @brief   makeDisp2UpdateCopy
 * 表示面から更新面へのコピー
 *
 * @author  A.Kubota
 * @date    2016/04/22
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           コピー領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       現在の表示面を指定する(0 or 1)
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    未使用
 *  int             target_ID;      未使用
 *  unsigned short* str;            未使用
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      未使用
 *  unsigned short  distance;       未使用
 *  unsigned char   fore_color;     未使用
 *  unsigned char   back_color;     未使用
 */
/****************************************************************************/
int SCDrawCom::makeDisp2UpdateCopy(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    unsigned short* p_buf;


    // コマンド格納位置
    p_buf = draw_command_buf;


    // メニュー描画共通設定(0x03)
    *p_buf++ = makeCom03(SC_UPDATE_WORK, COM03_DATA_COORD);

    // メニュー描画BitBlt転送元座標設定オフセット2→1→0
    p_buf += makeCom060708D2U(param.work_scr, p_buf);

    // メニュー描画BitBlt転送元座標設定Ｙ座標(0x09)
    *p_buf++ = makeCom09(param.rect.y);

    // メニュー描画BitBlt転送元座標設定Ｘ座標(0x0a)
    *p_buf++ = makeCom0a(param.rect.x);

    // メニュー描画BitBlt転送先座標設定オフセット2→1→0
    p_buf += makeCom0b0c0d(SC_UPDATE_WORK, p_buf);

    // メニュー描画BitBlt転送先座標設定Ｙ座標(0x0e)
    *p_buf++ = makeCom0e(param.rect.y);

    // メニュー描画BitBlt転送先座標設定Ｘ座標(0x0f)
    *p_buf++ = makeCom0f(param.rect.x);

    // メニュー描画BitBlt転送データ領域設定 width(0x10)
    *p_buf++ = makeCom10(param.rect.width);

    // メニュー描画BitBlt転送データ領域設定 height(0x11)
    *p_buf++ = makeCom11(param.rect.height);

    // 描画実行レジスタ 8bitデータの矩形領域転送
    *p_buf++ = makeCom01(COM01_MOVE_8BITRECT);


    return(p_buf - draw_command_buf);
}


/****************************************************************************/
/**
 * @brief   makeWork2UpdateCopy
 * ワーク用画面（トレンド、リスト、スクロールエリア）から更新面へのコピー
 *
 * @author  Nobuhiro Mitomo
 * @date    2016/09/06
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           コピー領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       描画元の画面指定
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    未使用
 *  int             target_ID;      未使用
 *  unsigned short* str;            未使用
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      未使用
 *  unsigned short  distance;       未使用
 *  unsigned char   fore_color;     未使用
 *  unsigned char   back_color;     未使用
 */
/****************************************************************************/
int SCDrawCom::makeWork2UpdateCopy(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    unsigned short* p_buf;


    // コマンド格納位置
    p_buf = draw_command_buf;


    // メニュー描画共通設定(0x03)
    *p_buf++ = makeCom03(SC_UPDATE_WORK, COM03_DATA_COORD);

    // メニュー描画BitBlt転送元座標設定オフセット2→1→0
    p_buf += makeCom060708W2U(param.work_scr, p_buf);

    // メニュー描画BitBlt転送元座標設定Ｙ座標(0x09)
    *p_buf++ = makeCom09(param.rect.y);

    // メニュー描画BitBlt転送元座標設定Ｘ座標(0x0a)
    *p_buf++ = makeCom0a(param.rect.x);

    // メニュー描画BitBlt転送先座標設定オフセット2→1→0
    p_buf += makeCom0b0c0d(SC_UPDATE_WORK, p_buf);

    // メニュー描画BitBlt転送先座標設定Ｙ座標(0x0e)
    *p_buf++ = makeCom0e(param.rect.y);

    // メニュー描画BitBlt転送先座標設定Ｘ座標(0x0f)
    *p_buf++ = makeCom0f(param.rect.x);

    // メニュー描画BitBlt転送データ領域設定 width(0x10)
    *p_buf++ = makeCom10(param.rect.width);

    // メニュー描画BitBlt転送データ領域設定 height(0x11)
    *p_buf++ = makeCom11(param.rect.height);

    // 描画実行レジスタ 8bitデータの矩形領域転送
    *p_buf++ = makeCom01(COM01_MOVE_8BITRECT);


    return(p_buf - draw_command_buf);
}


/****************************************************************************/
/**
 * @brief   makeWork2UpdateCopy2
 * ワーク用画面（トレンド、リスト、スクロールエリア）から更新面へのコピーその２
 * 位置を変えてのコピーが必要なかったためmakeWork2UpdateCopy()に機能がなかった
 *
 * @author  A.Kubota
 * @date    2017/07/04
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           コピー領域(コピー元)
 *  SCPoint         dest;           コピー先座標
 *  unsigned int    work_scr;       描画元の画面指定
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      未使用
 *  int             arrangement;    未使用
 *  int             target_ID;      未使用
 *  unsigned short* str;            未使用
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      未使用
 *  unsigned short  distance;       未使用
 *  unsigned char   fore_color;     未使用
 *  unsigned char   back_color;     未使用
 */
/****************************************************************************/
int SCDrawCom::makeWork2UpdateCopy2(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    unsigned short* p_buf;


    // コマンド格納位置
    p_buf = draw_command_buf;


    // メニュー描画共通設定(0x03)
    *p_buf++ = makeCom03(SC_UPDATE_WORK, COM03_DATA_COORD);

    // メニュー描画BitBlt転送元座標設定オフセット2→1→0
    p_buf += makeCom060708W2U(param.work_scr, p_buf);

    // メニュー描画BitBlt転送元座標設定Ｙ座標(0x09)
    *p_buf++ = makeCom09(param.rect.y);

    // メニュー描画BitBlt転送元座標設定Ｘ座標(0x0a)
    *p_buf++ = makeCom0a(param.rect.x);

    // メニュー描画BitBlt転送先座標設定オフセット2→1→0
    p_buf += makeCom0b0c0d(SC_UPDATE_WORK, p_buf);

    // メニュー描画BitBlt転送先座標設定Ｙ座標(0x0e)
    *p_buf++ = makeCom0e(param.dest.y);

    // メニュー描画BitBlt転送先座標設定Ｘ座標(0x0f)
    *p_buf++ = makeCom0f(param.dest.x);

    // メニュー描画BitBlt転送データ領域設定 width(0x10)
    *p_buf++ = makeCom10(param.rect.width);

    // メニュー描画BitBlt転送データ領域設定 height(0x11)
    *p_buf++ = makeCom11(param.rect.height);

    // 描画実行レジスタ 8bitデータの矩形領域転送
    *p_buf++ = makeCom01(COM01_MOVE_8BITRECT);


    return(p_buf - draw_command_buf);
}


/****************************************************************************/
/**
 * @brief   makeSlideCopy
 * 指定画面から指定画面へスライドコピーする
 *
 * @author  A.Kubota
 * @date    2016/10/03
 *
 * @param[in]   SCDRAWPARAM&    param               描画依頼パラメータ
 * @param[out]  unsigned short* draw_command_buf    描画コマンドバッファ
 *
 * @return      int     作成した描画パラメータ数
 *
 * SCDRAWPARAM
 *  SCRect          rect;           コピー領域
 *  SCPoint         dest;           未使用
 *  unsigned int    work_scr;       Copy元の画面指定
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  unsigned int    work2_scr;      Copy先の画面指定
 *                                    SC_UPDATE_WORK    更新面
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
 *                                    SC_SCRAREA_WORK   スクロールエリア裏画面
 *  int             arrangement;    スライド方向
 *                                    SC_ARRANGE_LEFT   左移動
 *                                    SC_ARRANGE_CENTER 移動なし(通常コピー)
 *                                    SC_ARRANGE_RIGHT  右移動
 *                                    SC_ARRANGE_TOP    上移動
 *                                    SC_ARRANGE_BOTTOM 下移動
 *  int             target_ID;      未使用
 *  unsigned short* str;            未使用
 *  bool            frame;          未使用
 *  bool            fill;           未使用
 *  unsigned short  font_type;      未使用
 *  unsigned short  distance;       スライド距離(0～511dot)
 *  unsigned char   fore_color;     未使用
 *  unsigned char   back_color;     未使用
 *
 *　注1)Copy元の画面に更新面は設定できない
 *　注2)work_scrとwork2_scrに同じものを設定しないこと
 *　注3)distanceが0 or (512 or width or height)以上指定された場合は
 *      Slideなしでコピー
 */
/****************************************************************************/
int SCDrawCom::makeSlideCopy(
    SCDRAWPARAM&        param,
    unsigned short*     draw_command_buf
)
{
    unsigned short  src_x, src_y, dest_x, dest_y;
    unsigned short  copy_width, copy_height;
    unsigned short  distance = param.distance;
    unsigned short* p_buf;


    // スライド距離チェック
    if(distance > 0x1ff) {
        distance = 0;
    }


    switch(param.arrangement) {
    case SC_ARRANGE_LEFT:
        // 幅チェック
        if(distance >= param.rect.width) {
            distance = 0;
        }

        // コピー座標
        src_x = param.rect.x + distance;
        dest_x = param.rect.x;
        src_y = dest_y = param.rect.y;
        copy_width = param.rect.width - distance;
        copy_height = param.rect.height;
        break;

    case SC_ARRANGE_RIGHT:
        // 幅チェック
        if(distance >= param.rect.width) {
            distance = 0;
        }

        // コピー座標
        src_x = param.rect.x;
        dest_x = param.rect.x + distance;
        src_y = dest_y = param.rect.y;
        copy_width = param.rect.width - distance;
        copy_height = param.rect.height;
        break;

    case SC_ARRANGE_TOP:
        // 高さチェック
        if(distance >= param.rect.height) {
            distance = 0;
        }

        // コピー座標
        src_x = dest_x = param.rect.x;
        src_y = param.rect.y + distance;
        dest_y = param.rect.y;
        copy_width = param.rect.width;
        copy_height = param.rect.height - distance;
        break;

    case SC_ARRANGE_BOTTOM:
        // 高さチェック
        if(distance >= param.rect.height) {
            distance = 0;
        }

        // コピー座標
        src_x = dest_x = param.rect.x;
        src_y = param.rect.y;
        dest_y = param.rect.y + distance;
        copy_width = param.rect.width;
        copy_height = param.rect.height - distance;
        break;

    case SC_ARRANGE_CENTER:
    default:
        // コピー座標
        src_x = dest_x = param.rect.x;
        src_y = dest_y = param.rect.y;
        copy_width = param.rect.width;
        copy_height = param.rect.height;
        break;
    }

    // コマンド格納位置
    p_buf = draw_command_buf;


    // メニュー描画共通設定(0x03)
    *p_buf++ = makeCom03(param.work2_scr, COM03_DATA_COORD);

    // メニュー描画BitBlt転送元座標設定オフセット2→1→0
    p_buf += makeCom060708W2U(param.work_scr, p_buf);

    // メニュー描画BitBlt転送元座標設定Ｙ座標(0x09)
    *p_buf++ = makeCom09(src_y);

    // メニュー描画BitBlt転送元座標設定Ｘ座標(0x0a)
    *p_buf++ = makeCom0a(src_x);

    // メニュー描画BitBlt転送先座標設定オフセット2→1→0
    p_buf += makeCom0b0c0d(param.work2_scr, p_buf);

    // メニュー描画BitBlt転送先座標設定Ｙ座標(0x0e)
    *p_buf++ = makeCom0e(dest_y);

    // メニュー描画BitBlt転送先座標設定Ｘ座標(0x0f)
    *p_buf++ = makeCom0f(dest_x);

    // メニュー描画BitBlt転送データ領域設定 width(0x10)
    *p_buf++ = makeCom10(copy_width);

    // メニュー描画BitBlt転送データ領域設定 height(0x11)
    *p_buf++ = makeCom11(copy_height);

    // 描画実行レジスタ 8bitデータの矩形領域転送
    *p_buf++ = makeCom01(COM01_MOVE_8BITRECT);


    return(p_buf - draw_command_buf);
}

// ----------------------< !内部函数 >----------------------------------------
/*
 * @decs    : 画矩形
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
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

/*
 * @decs    : 重复画矩形
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int SCDrawCom::makeRepeatFillRect(
    SCRect&         rect,
    unsigned short* buf
)
{
    unsigned short* p_buf;


    // command 保存位置
    p_buf = buf;


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

/*
 * @decs    : 描画命令command-0x01
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom01(
    unsigned short  exec
)
{
    unsigned short  rtn = 0x01;


    // コマンド番号
    rtn = (rtn << 10) | exec;


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x02。选择screen plate
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom02(
    unsigned int        pal_no,
    unsigned int        scr_no
)
{
    unsigned short func;
    unsigned short rtn = 0x02;


    func = (pal_no)? 0x02 : 0x00;

    if(scr_no) {
        func |= 0x01;
    }


    // Command编号
    rtn = (rtn << 10) | func;


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x03。选择数据类型
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom03(
    unsigned int    work_scr,
    int             data_type
)
{
    unsigned short  rtn = 0x03;


    switch(work_scr) 
    {
    default:
    case SC_UPDATE_WORK:        // 更新用表示プレーン
        rtn = COM03_DEST_DISP;
        break;
    case SC_TREND_WORK1:        // トレンド用裏画面1
    case SC_TREND_WORK2:        // トレンド用裏画面2
    case SC_LIST_WORK:          // List用裏画面
    case SC_SCRAREA_WORK:       // スクロールエリア用裏画面
        rtn = COM03_DEST_NONE;
        break;
    }


    // 数据类型
    rtn |= data_type;


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x04。 设置后景色
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom04(
    unsigned char   color
)
{
    unsigned short  rtn = 0x04;


    // Command编号
    rtn = (rtn << 10) | (unsigned short)color;


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x05。 设置前景色
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom05(
    unsigned char   color
)
{
    unsigned short  rtn = 0x05;


    // Command编号
    rtn = (rtn << 10) | (unsigned short)color;


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x06/0x07/0x08。
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::makeCom060708D2U(
    unsigned int    display,
    unsigned short* p_buf
)
{
    int     command_num = 3;    // 現状固定


    if(display) {   // 表示面(0 or 1)
        p_buf[0] = UPDATE_OFSET06;
        p_buf[1] = UPDATE_OFSET07;
        p_buf[2] = UPDATE_OFSET08;
    }
    else {
        p_buf[0] = DISPLAY_OFSET06;
        p_buf[1] = DISPLAY_OFSET07;
        p_buf[2] = DISPLAY_OFSET08;
    }


    return(command_num);
}

/*
 * @decs    : 描画命令command-0x06/0x07/0x08。
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::makeCom060708W2U(
    unsigned int    display,
    unsigned short* p_buf
)
{
    int     command_num = 3;    // 初期値：3


    switch(display) {
    case SC_TREND_WORK1:        // トレンド裏画面1
        p_buf[0] = TREND1_OFSET06;
        p_buf[1] = TREND1_OFSET07;
        p_buf[2] = TREND1_OFSET08;
        break;

    case SC_TREND_WORK2:        // トレンド裏画面2
        p_buf[0] = TREND2_OFSET06;
        p_buf[1] = TREND2_OFSET07;
        p_buf[2] = TREND2_OFSET08;
        break;

    case SC_LIST_WORK:          // List裏画面
        p_buf[0] = LIST_OFSET06;
        p_buf[1] = LIST_OFSET07;
        p_buf[2] = LIST_OFSET08;
        break;

    case SC_SCRAREA_WORK:       // スクロールエリア裏画面
        p_buf[0] = SCRAREA_OFSET06;
        p_buf[1] = SCRAREA_OFSET07;
        p_buf[2] = SCRAREA_OFSET08;
        break;

    default:
        command_num = 0;
        break;
    }


    return(command_num);
}

/*
 * @decs    : 描画命令command-0x09。 设置原坐标地址Y
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom09(
    unsigned short  y
)
{
    unsigned short  rtn = 0x09;


    // command编号
    rtn = (rtn << 10) | (0x03ff & y);


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x0A。 设置原坐标地址X
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom0a(
    unsigned short  x
)
{
    unsigned short  rtn = 0x0a;


    // コマンド番号
    rtn = (rtn << 10) | (0x01ff & x);


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x0B/0x0C/0x0D。
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::makeCom0b0c0d(
    unsigned int    work_scr,
    unsigned short* p_buf
)
{
    int     command_num = 3;


    switch(work_scr) {
    case SC_UPDATE_WORK:
        p_buf[0] = UPDATE_OFSET0B;
        p_buf[1] = UPDATE_OFSET0C;
        p_buf[2] = UPDATE_OFSET0D;
        break;

    case SC_TREND_WORK1:
        p_buf[0] = TREND1_OFSET0B;
        p_buf[1] = TREND1_OFSET0C;
        p_buf[2] = TREND1_OFSET0D;
        break;

    case SC_TREND_WORK2:
        p_buf[0] = TREND2_OFSET0B;
        p_buf[1] = TREND2_OFSET0C;
        p_buf[2] = TREND2_OFSET0D;
        break;

    case SC_LIST_WORK:
        p_buf[0] = LIST_OFSET0B;
        p_buf[1] = LIST_OFSET0C;
        p_buf[2] = LIST_OFSET0D;
        break;

    case SC_SCRAREA_WORK:
        p_buf[0] = SCRAREA_OFSET0B;
        p_buf[1] = SCRAREA_OFSET0C;
        p_buf[2] = SCRAREA_OFSET0D;
        break;

    default:
        command_num = 0;
        break;
    }


    return(command_num);
}

/*
 * @decs    : 描画命令command-0x0E。 目标地址Y
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom0e(
    unsigned short  y
)
{
    unsigned short  rtn = 0x0e;


    // コマンド番号
    rtn = (rtn << 10) | (0x03ff & y);


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x0F。 目标地址X
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom0f(
    unsigned short  x
)
{
    unsigned short  rtn = 0x0f;


    // コマンド番号
    rtn = (rtn << 10) | (0x01ff & x);


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x10。 目标数据区域width
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom10(
    unsigned short  width
)
{
    unsigned short  rtn = 0x10;


    // コマンド番号
    rtn = (rtn << 10) | (0x01ff & width);


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x11。 目标数据区域height
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom11(
    unsigned short  height
)
{
    unsigned short  rtn = 0x11;


    // コマンド番号
    rtn = (rtn << 10) | (0x01ff & height);


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x13。 指定字符类型
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
unsigned short  SCDrawCom::makeCom13(
    unsigned short  font_type
)
{
    unsigned short  rtn = 0x13;


    // コマンド番号
    rtn = (rtn << 10) | (0x0007 & font_type);


    return(rtn);
}

/*
 * @decs    : 描画命令command-0x16/0x17/0x18/0x19。 指定调色板地址，设定R、G、B
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::makeCom16171819(
    unsigned char   pType,
    unsigned char   pNo,
    unsigned char   r,
    unsigned char   g,
    unsigned char   b,
    unsigned short* p_buf
)
{
    int     command_num = 0;


    // Pallete Number
    p_buf[0] = 0x16;
    p_buf[0] = (p_buf[0] << 10) | (unsigned short)pNo;
    if(pType) {
        p_buf[0] |= 0x0100;
    }
    command_num++;

    // Red
    p_buf[1] = 0x17;
    p_buf[1] = (p_buf[1] << 10) | (unsigned short)r;
    command_num++;

    // Green
    p_buf[2] = 0x18;
    p_buf[2] = (p_buf[2] << 10) | (unsigned short)g;
    command_num++;

    // Blue
    p_buf[3] = 0x19;
    p_buf[3] = (p_buf[3] << 10) | (unsigned short)b;
    command_num++;


    return(command_num);
}

/*
 * @decs    : 描画命令command-0x3E。 画字符串
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::makeCom3e(
    unsigned short* str,
    unsigned short  distance,
    unsigned short* p_buf
)
{
    unsigned short  work_com;
    int             command_num = 0;


    // 描画コマンド
    work_com = 0x3e;
    *p_buf++ = (work_com << 10) | (0x03ff & distance);
    command_num++;

    while(*str) {
        *p_buf++ = *str++;
        command_num++;

        if(command_num >= SC_MAX_FREE_STRING) {
            break;
        }
    }


    // NULL文字追加
    *p_buf = 0x0000;
    command_num++;


    return(command_num);
}

/*
 * @decs    : 描画命令command-0x3F。set plane ID
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::makeCom3f(
    unsigned short  set_plane,
    unsigned short  plane_id,
    unsigned short* p_buf
)
{
    unsigned short  com_work;
    int             command_num = 0;


    // 描画コマンド
    com_work = 0x3f;
    *p_buf++ = (com_work << 10) | (0x0003 & set_plane);
    command_num++;

    *p_buf++ = plane_id;
    command_num++;


    return(command_num);
}

/*
 * @decs    : 
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int SCDrawCom::getFontHeight(unsigned short  font_type)
{
    SCFONTINFO*     font_info;
    unsigned short  registFontNum;
    int             rtn = 0;


    registFontNum = static_cast<unsigned short>(sc_getFontNum());

    if(font_type < registFontNum) {
        font_info = sc_getFontInfo();
        rtn = font_info[font_type].f_height;
    }


    return(rtn);
}

/*
 * @decs    : 
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int SCDrawCom::getStringWidth(
    unsigned short  font_type,
    unsigned short* str
)
{
    int             str_width;
    SCFONTINFO*     font_info;
    unsigned char*  width_address;

    font_info = sc_getFontInfo();
    width_address = font_info[font_type].width;

    str_width = 0;

    while(*str) {
        str_width += width_address[*str];
        str++;

        // 次の文字幅を足しこんだときにプレーン幅より大きくなるなら
        if(str_width + width_address[*str] > SC_PLANE_WIDTH) {
            break;
        }
    }


    return(str_width);
}

/*
 * @decs    : 
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
void    SCDrawCom::getTargetPosition(
    int         arrange,
    SCRect&     rect,
    int         width,
    int         height,
    SCPoint*    pos
)
{
    int     work;


    switch(arrange) {
    default:
    case SC_ARRANGE_LEFT:
        pos->x = rect.x;
        break;

    case SC_ARRANGE_CENTER:
        work = (width - rect.width) / 2;
        if(work > (int)(rect.x)) {
            pos->x = 0;
        }
        else {
            pos->x = rect.x - work;
        }
        break;

    case SC_ARRANGE_RIGHT:
        work = width - rect.width;
        if(work > (int)(rect.x)) {
            pos->x = 0;
        }
        else {
            pos->x = (unsigned short)(rect.x - work);
        }
        break;
    }

    // Y座標はセンター固定
    work = (height - rect.height) / 2;
    if(work > (int)rect.y) {
        pos->y = 0;
    }
    else {
        pos->y = rect.y - work;
    }
}

/*
 * @decs    : 
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int SCDrawCom::getBigFontStringWidth(
    unsigned short  font_type,
    unsigned short  distance,
    unsigned short* str
)
{
    int     id, char_width, str_width;


    str_width = 0;

    if(*str) {
        // 文字幅の取得
        id = getBigFontID(font_type, *str);
        char_width = sc_getPictWidth(id);

        // プレーン幅より大きくならないこと(NULLなし対策にも)
        while(str_width + char_width <= SC_PLANE_WIDTH) {
            str_width += char_width;
            str++;

            if(*str == 0x0000) {
                break;
            }
            else {
                // 次の文字幅の取得
                id = getBigFontID(font_type, *str);
                char_width = sc_getPictWidth(id);
                char_width += distance;
            }
        }
    }


    return(str_width);
}

#if 1
/*
 * @decs    : 
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::getBigFontID(
    unsigned short  font_type,
    unsigned short  char_code
)
{
//     SC_BIG_FONT_TBL*    tbl;
//     int                 rtn = IDB_BIGFONT_L_0;


//     switch(font_type) {
//     case SC_BIGFONT_TYPE_XL:
//         tbl = (SC_BIG_FONT_TBL*)sc_big_font_xl_tbl;
//         rtn = IDB_BIGFONT_XL_0;
//         break;

//     default:
//     case SC_BIGFONT_TYPE_L:
//         tbl = (SC_BIG_FONT_TBL*)sc_big_font_l_tbl;
//         rtn = IDB_BIGFONT_L_0;
//         break;

//     case SC_BIGFONT_TYPE_M:
//         tbl = (SC_BIG_FONT_TBL*)sc_big_font_m_tbl;
//         rtn = IDB_BIGFONT_M_0;
//         break;

//     case SC_BIGFONT_TYPE_S:
//         tbl = (SC_BIG_FONT_TBL*)sc_big_font_s_tbl;
//         rtn = IDB_BIGFONT_S_0;
//         break;

// //  case SC_BIGFONT_TYPE_XS:
// //      tbl = (SC_BIG_FONT_TBL*)sc_big_font_xs_tbl;
// //      rtn = IDB_BIGFONT_XS_0;
// //      break;

//     case SC_BIGFONT_TYPE_EMC_L:
//         tbl = (SC_BIG_FONT_TBL*)sc_emc_big_font_l_tbl;
//         rtn = IDB_EMC_BF_L_0;
//         break;

//     case SC_BIGFONT_TYPE_EMC_S:
//         tbl = (SC_BIG_FONT_TBL*)sc_emc_big_font_s_tbl;
//         rtn = IDB_EMC_BF_S_0;
//         break;
//     }

//     while(tbl->fontID != -1) {
//         if(char_code == tbl->char_code) {
//             rtn = tbl->fontID;
//             break;
//         }
//         tbl++;
//     }


//     return(rtn);
    return 0;
}
#endif
#if 1
/*
 * @decs    : 
 * 
 * @params  ： 
 * 
 * @return  ： 
 */
int     SCDrawCom::getBigFontHeight(
    unsigned short  font_type
)
{
//     int pict_id;

//     switch(font_type) {
//     default:
//     case SC_BIGFONT_TYPE_XL:
//         pict_id = IDB_BIGFONT_XL_0;
//         break;

//     case SC_BIGFONT_TYPE_L:
//         pict_id = IDB_BIGFONT_L_0;
//         break;

//     case SC_BIGFONT_TYPE_M:
//         pict_id = IDB_BIGFONT_M_0;
//         break;

//     case SC_BIGFONT_TYPE_S:
//         pict_id = IDB_BIGFONT_S_0;
//         break;

// //  case SC_BIGFONT_TYPE_XS:
// //      pict_id = IDB_BIGFONT_XS_0;
// //      break;

//     case SC_BIGFONT_TYPE_EMC_L:
//         pict_id = IDB_EMC_BF_L_0;
//         break;

//     case SC_BIGFONT_TYPE_EMC_S:
//         pict_id = IDB_EMC_BF_S_0;
//         break;
//     }

//     return(sc_getPictHeight(pict_id));
    return 0;
}
#endif
