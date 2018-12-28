// Copyright (c) 2016 Yokogawa Electric Corporation. All rights reserved.
/**
* @file sclParts.cpp
* SCPartsクラス定義
*
* @author   A.Kubota
* @version  R1.01.01    2016/02/29  A.Kubota    New
*/

#include "sclBase.h"
#include "sclParts.h"
#include "sclCallback.h"
#include "sclDrawCom.h"


// クラス情報の設定
M_IMPLEMENT_RUNTIME(SCParts, SCUpdateTypeNONE, SCCore)


SCParts::SCParts(
    const short     id,
    const SCRect&   area,
    unsigned char   fore_color,
    unsigned char   back_color,
    unsigned int    work_scr
) : SCCore(area.x, area.y, area.width, area.height),
    m_parent(0),
    m_next(0),
    m_prev(0),
    m_oTrend(0),
    m_callback(0),
    m_work_scr(work_scr),
    m_id(id),
    m_fVisible(true),
    m_fEnable(true),
    m_fPushed(false),
    m_fore_color(fore_color),
    m_back_color(back_color)
{
}


SCParts::~SCParts(
)
{
    if(m_callback) {
        RemoveAllCallbacks();
    }
}


/****************************************************************************/
/**
 * @brief   Draw関数
 * 自分自身の初期表示のための描画コマンドを作成する
 *
 * @author  A.Kubota
 * @date    2016/03/07
 *
 * @param[in/out]   unsigned short*     com     描画コマンド作成バッファ
 *
 * @return  int     作成された描画コマンド数
 */
/****************************************************************************/
int     SCParts::Draw(
    unsigned short*     para
)
{
    return(0);
}


/****************************************************************************/
/**
 * @brief   DrawNumString関数
 * 数値文字列表示のための描画コマンドを作成する
 *
 * @author  A.Kubota
 * @date    2016/10/13
 *
 * @param[in]       SCDRAWPARAM&        param   描画依頼パラメータ
 * @param[out]      unsigned short*     com     描画コマンド作成バッファ
 *
 * @return  int     作成された描画コマンド数
 *
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
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
 *                                    SC_PARTS_FONT_TYPE_LFONT    Font L
 *                                    SC_PARTS_FONT_TYPE_MFONT    Font M
 *                                    SC_PARTS_FONT_TYPE_SFONT    Font S
 *                                    SC_PARTS_FONT_TYPE_XLPICT   BigFont XL
 *                                    SC_PARTS_FONT_TYPE_LPICT    BigFont L
 *                                    SC_PARTS_FONT_TYPE_MPICT    BigFont M
 *                                    SC_PARTS_FONT_TYPE_SPICT    BigFont S
 *                                    SC_PARTS_FONT_TYPE_XSPICT   BigFont XS
 *                                    SC_PARTS_FONT_TYPE_EMCLPICT EMCBigFont L
 *                                    SC_PARTS_FONT_TYPE_EMCSPICT EMCBigFont S
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     文字の色(通常フォント用)
 *  unsigned char   back_color;     背景の色(通常フォント用)
 */
/****************************************************************************/
int     SCParts::DrawNumString(
    SCDRAWPARAM&        param,
    unsigned short*     com
)
{
    int     rtn = 0;


    switch(param.font_type) {
    case SC_PARTS_FONT_TYPE_LFONT:
    case SC_PARTS_FONT_TYPE_MFONT:
    case SC_PARTS_FONT_TYPE_SFONT:
        rtn = SCDrawCom::makeFreeString(param, com);
        break;

    case SC_PARTS_FONT_TYPE_XLPICT:
    case SC_PARTS_FONT_TYPE_LPICT:
    case SC_PARTS_FONT_TYPE_MPICT:
    case SC_PARTS_FONT_TYPE_SPICT:
    case SC_PARTS_FONT_TYPE_XSPICT:
    case SC_PARTS_FONT_TYPE_EMCLPICT:
    case SC_PARTS_FONT_TYPE_EMCSPICT:
        param.font_type -= SC_PARTS_FONT_TYPE_XLPICT;
                // フォントタイプ SC_BIGFONT_TYPEへの変換
        rtn = SCDrawCom::makeBigFontString(param, com);
        break;

    default:
        break;
    }


    return(rtn);
}


/****************************************************************************/
/**
 * @brief   DrawTagString関数
 * 数値文字列表示のための描画コマンドを作成する
 *
 * @author  A.Kubota
 * @date    2016/10/13
 *
 * @param[in]       SCDRAWPARAM&        param   描画依頼パラメータ
 * @param[out]      unsigned short*     com     描画コマンド作成バッファ
 *
 * @return  int     作成された描画コマンド数
 *
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
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
 *                                    SC_PARTS_FONT_TYPE_LFONT    Font L
 *                                    SC_PARTS_FONT_TYPE_MFONT    Font M
 *                                    SC_PARTS_FONT_TYPE_SFONT    Font S
 *                                    SC_PARTS_FONT_TYPE_XSPICT   BigFont XS
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     文字の色(通常フォント用)
 *  unsigned char   back_color;     背景の色(通常フォント用)
 */
/****************************************************************************/
int     SCParts::DrawTagString(
    SCDRAWPARAM&        param,
    unsigned short*     com
)
{
    int     rtn = 0;


    switch(param.font_type) {
    case SC_PARTS_FONT_TYPE_LFONT:
    case SC_PARTS_FONT_TYPE_MFONT:
    case SC_PARTS_FONT_TYPE_SFONT:
        rtn = SCDrawCom::makeFreeString(param, com);
        break;

    case SC_PARTS_FONT_TYPE_XSPICT:
        param.font_type -= SC_PARTS_FONT_TYPE_XLPICT;
                // フォントタイプ SC_BIGFONT_TYPEへの変換
        rtn = SCDrawCom::makeBigFontString(param, com);
        break;

    default:
    case SC_PARTS_FONT_TYPE_XLPICT:
    case SC_PARTS_FONT_TYPE_LPICT:
    case SC_PARTS_FONT_TYPE_MPICT:
    case SC_PARTS_FONT_TYPE_SPICT:
    case SC_PARTS_FONT_TYPE_EMCLPICT:
    case SC_PARTS_FONT_TYPE_EMCSPICT:
        break;
    }


    return(rtn);
}


/****************************************************************************/
/**
 * @brief   DrawUnitString関数
 * 数値文字列表示のための描画コマンドを作成する
 *
 * @author  A.Kubota
 * @date    2016/10/13
 *
 * @param[in]       SCDRAWPARAM&        param   描画依頼パラメータ
 * @param[out]      unsigned short*     com     描画コマンド作成バッファ
 *
 * @return  int     作成された描画コマンド数
 *
 *
 * SCDRAWPARAM
 *  SCRect          rect;           文字列領域
 *  unsigned int    work_scr;       描画する裏画面
 *                                    SC_UPDATE_WORK    表示プレーン(更新用)
 *                                    SC_TREND_WORK1    トレンド裏画面1
 *                                    SC_TREND_WORK2    トレンド裏画面2
 *                                    SC_LIST_WORK      List裏画面
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
 *                                    SC_PARTS_FONT_TYPE_LFONT    Font L
 *                                    SC_PARTS_FONT_TYPE_MFONT    Font M
 *                                    SC_PARTS_FONT_TYPE_SFONT    Font S
 *                                    SC_PARTS_FONT_TYPE_XSPICT   BigFont XS
 *                                    SC_PARTS_FONT_TYPE_EMCSPICT EMCBigFont S
 *  unsigned short  distance;       字間(0～1023ドット)
 *  unsigned char   fore_color;     文字の色(通常フォント用)
 *  unsigned char   back_color;     背景の色(通常フォント用)
 */
/****************************************************************************/
int     SCParts::DrawUnitString(
    SCDRAWPARAM&        param,
    unsigned short*     com
)
{
    int     rtn = 0;


    switch(param.font_type) {
    case SC_PARTS_FONT_TYPE_LFONT:
    case SC_PARTS_FONT_TYPE_MFONT:
    case SC_PARTS_FONT_TYPE_SFONT:
        rtn = SCDrawCom::makeFreeString(param, com);
        break;

    case SC_PARTS_FONT_TYPE_XSPICT:
    case SC_PARTS_FONT_TYPE_EMCSPICT:
        param.font_type -= SC_PARTS_FONT_TYPE_XLPICT;
                // フォントタイプ SC_BIGFONT_TYPEへの変換
        rtn = SCDrawCom::makeBigFontString(param, com);
        break;

    default:
    case SC_PARTS_FONT_TYPE_XLPICT:
    case SC_PARTS_FONT_TYPE_LPICT:
    case SC_PARTS_FONT_TYPE_MPICT:
    case SC_PARTS_FONT_TYPE_SPICT:
    case SC_PARTS_FONT_TYPE_EMCLPICT:
        break;
    }


    return(rtn);
}


/****************************************************************************/
/**
 * @brief   DrawBackground関数
 * 親部品の背景や、トレンド部品と重なっている場合の背景描画
 *
 * @author  A.Kubota
 * @date    2016/03/15
 *
 * @param[in/out]   unsigned short* para    描画コマンド作成バッファ
 *
 * @return  int     作成された描画コマンド数
 */
/****************************************************************************/
int     SCParts::DrawBackground(
    unsigned short*     para
)
{
    SCDRAWPARAM     param;
    SCBoard*        parent;
    unsigned short* paraPtr;
    int             commandNum;
    int             allCommandNum = 0;


    paraPtr = para;
    parent = Parent();


    // 背景色で塗りつぶし
    param.work_scr = m_work_scr;        // 表示プレーン(更新用)指定
    param.fill = true;                  // 塗りつぶしあり
    param.back_color = ( (parent) && (m_back_color == COLOR_TRANSPARENT) )?
            parent->GetBackColor() : m_back_color;
                                        // 塗りつぶしの色
    param.rect = m_area;                // 塗りつぶし領域
    param.frame = false;                // 枠線なし
    commandNum = SCDrawCom::makeRect(param, paraPtr);
    paraPtr += commandNum;
    allCommandNum += commandNum;


    // トレンド部品と重なっていればトレンド背景描画(現状認めていない)
    if(m_oTrend) {
        //commandNum = m_oTrend->CopyWaveArea(m_area, paraPtr);
        commandNum = 0;
        paraPtr += commandNum;
        allCommandNum += commandNum;
    }


    return(allCommandNum);
}


/****************************************************************************/
/**
 * @brief   Update関数
 * 自分自身の更新用の描画コマンドを作成する
 *
 * @author  A.Kubota
 * @date    2016/03/07
 *
 * @param[in]       const int       updateType  更新タイプ
 * @param[in/out]   unsigned short* para        描画コマンド作成バッファ
 *
 * @return  int     作成された描画コマンド数
 */
/****************************************************************************/
int SCParts::Update(
    const int       updateType,
    unsigned short* para
)
{
    unsigned short* paraPtr;
    int             commandNum;
    int             allCommandNum = 0;


    if(GetUpdateType() & updateType) {
        // 可視属性チェック
        if(GetVisible() == false) {
            return(0);
        }


        paraPtr = para;


        commandNum = DrawBackground(paraPtr);
        paraPtr += commandNum;
        allCommandNum += commandNum;

        // 自分自身の描画
        allCommandNum += Draw(paraPtr);
    }

    return(allCommandNum);
}


/****************************************************************************/
/**
 * @brief   ReDrawDisplay関数
 * 自分自身の描画と画面への表示を行う
 *
 * @author  A.Kubota
 * @date    2016/03/07
 *
 * @param           なし
 *
 * @return          なし
 */
/****************************************************************************/
void    SCParts::ReDrawDisplay(
)
{
    unsigned short  para[SC_DRAW_COM_MAX];
    int             paraNum;


    // 自分自身の領域をBoardの背景で塗りつぶし
    paraNum = DrawBackground(para);

    // 自分自身を描画
    paraNum += Draw(&para[paraNum]);

    if(paraNum > 0) {
        if(m_work_scr == SC_UPDATE_WORK) {
            // 表示処理
            m_parent->Display(paraNum, para);
        }
        else {
            // 描画コマンドフラッシュ処理
            m_parent->FlushDrawCom(paraNum, para);
        }
    }
}


/****************************************************************************/
/**
 * @brief   AddCallback関数
 * 指定されたコールバックを登録する
 *
 * @author  A.Kubota
 * @date    2016/03/15
 *
 * @param[in]       SCCallback*     cb      コールバック
 *
 * @return          なし
 */
/****************************************************************************/
void    SCParts::AddCallback(
    SCCallback*     cb
)
{
    if(cb != 0) {
        SCCallback* last;

        last = m_callback;

        if(last == 0) {                     // コールバック未登録
            m_callback = cb;
        }
        else {                              // コールバック登録あり
            while(last->Next() != 0) {      // 最後のコールバックの取得処理
                last = last->Next();
            }

            last->Next(cb);
        }
    }
}


/****************************************************************************/
/**
 * @brief   DoCallback関数
 * 登録されたコールバックの中から引数で指定されたタイプのものを実行する
 *
 * @author  A.Kubota
 * @date    2016/03/15
 *
 * @param[in]       int         type    コールバックタイプ
 *
 * @return          なし
 */
/****************************************************************************/
void    SCParts::DoCallback(
    int     type
)
{
    SCCallback* cb;


    cb = m_callback;


    while(cb) {
        if(cb->GetType() == type) {
            cb->DoCallback();
        }

        cb = cb->Next();
    }
}


/****************************************************************************/
/**
 * @brief   RemoveAllCallbacks関数
 * 登録されたコールバックをすべて削除する
 *
 * @author  A.Kubota
 * @date    2016/03/15
 *
 * @param           なし
 *
 * @return          なし
 */
/****************************************************************************/
void    SCParts::RemoveAllCallbacks(
)
{
    SCCallback* cb;
    SCCallback* next;


    cb = m_callback;


    while(cb) {
        next = cb->Next();
        delete cb;

        cb = next;
    }

    m_callback = 0;
}


void    SCParts::TDown(
    const SCPoint&  point
)
{
    m_fPushed = true;
}


void    SCParts::TMove(
    const SCPoint&      point,
    const unsigned int  level,
    const int           direction
)
{
    bool    next;


    next = m_area.Contains(point);

    if(m_fPushed != next) {
        m_fPushed = next;
    }
}


void    SCParts::TUp(
    const SCPoint&  point
)
{
    if(m_fPushed) {
        m_fPushed = false;

        DoCallback(SCCallbackTypeTAP);
    }
}


void    SCParts::TFlick(
    const SCPoint&      point,
    const unsigned int  level,
    const int           direction
)
{
    if(m_fPushed) {
        m_fPushed = false;
    }

    DoCallback(SCCallbackTypeFLICK);
}


void    SCParts::TLongTap(
    const SCPoint&  point
)
{
    if(m_fPushed) {
        m_fPushed = false;

        DoCallback(SCCallbackTypeLONGTAP);
    }
}


