/**
* @file sclBoard.cpp
*           
*
* @author   Tupelo
* @version  2018-12-27
*/

#include <cstddef>
#include "GDC_Driver.h"
// #include "ana_stl/Timer.hpp"
#include "sclBoard.h"
#include "sclParts.h"
// #include "sclListManager.h"
#include "sclDrawCom.h"
// #include "sclDlgPasswordRelease.h"
// #include "liq-4wire/HMI/LiqHMIControl.h"
// #include "rc_string_id.h"
// #include "dlgCommon.h"


// using namespace ana_stl;


// 设定Class信息
M_IMPLEMENT_RUNTIME(SCBoard, SCUpdateTypeNONE, SCCore)
/****************************************************************************/
/**
 * @brief       SCBoard()
 *              构造函数
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      void
 */
/****************************************************************************/
SCBoard::SCBoard(
    const SCRect& area,
    const short sID
) : SCCore(area.x, area.y, area.width, area.height),
    m_child(0),
    m_captured(0),
    m_dialog(0),
    m_parent(0),
    m_fModal(false),
    m_screenID(sID)
{
}

/****************************************************************************/
/**
 * @brief       SCBoard()
 *              析构函数
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      void
 */
/****************************************************************************/
SCBoard::~SCBoard()
{
    SCBoard*    dialog;


    dialog = GetDialog();
    if(dialog != NULL) {
        delete dialog;
    }

    DestroyAllParts();
}

/****************************************************************************/
/**
 * @brief           Draw()
 *                  填充描画画面背景以及管理的子部品初期化的表示等的描画命令缓冲区
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   unsigned short*     com     填充描画命令缓冲区buffer
 *
 * @return  int     填充的描画命令数
 */
/****************************************************************************/
int SCBoard::Draw(unsigned short* com)
{
    int             allCommandNum = 0;
    unsigned short* comPos;


    comPos = com;


    // 填充描画背景的命令
    allCommandNum = DrawBackground(m_area, comPos);
    comPos += allCommandNum;


    // 调用子部品的Draw（）函数，描画board上管理的子部品的北京
    if(m_child) {
        int         commandNum;
        SCParts*    child = m_child;

        do {
            // 描画子部品
            commandNum = child->Draw(comPos);

            // 描画命令的个数，以及描画位置的保存
            allCommandNum += commandNum;
            comPos += commandNum;               // 其作用不理解？？？？？

            // 如果有必要的话，Flush描画命令
            if(allCommandNum > SC_DRAW_COM_LM) {
                FlushDrawCom(allCommandNum, com);
                allCommandNum = 0;
                comPos = com;
            }

            // 下一个子部品
            child = child->Next();
        } while(child);
    }

    return(allCommandNum);
}

/****************************************************************************/
/**
 * @brief           Update() 函数
 *                  更新描画画面背景以及管理的子部品初期化的表示等的描画命令缓冲区
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   unsigned short*     com     更新描画命令缓冲区buffer
 *
 * @return  int     更新的描画命令数
 */
/****************************************************************************/
int SCBoard::Update(const int updateType, unsigned short* com)
{
    int     allCommandNum = 0;

    // 调用子部品的Update()函数，对子部品的描画进行更新显示
    if(m_child) 
    {
        int             commandNum;
        unsigned short* comPos = com;
        SCParts*        child = m_child;

        do {
            // 描画子部品
            commandNum = child->Update(updateType, comPos);

            // 描画命令的个数，以及描画位置的保存
            allCommandNum += commandNum;
            comPos += commandNum;

            // 如果有必要的话，Flush描画命令
            if(allCommandNum > SC_DRAW_COM_LM) {
                FlushDrawCom(allCommandNum, com);
                allCommandNum = 0;
                comPos = com;
            }

            // 下一个子部品
            child = child->Next();
        } while(child);
    }

    return(allCommandNum);
}

/****************************************************************************/
/**
 * @brief           Display() 函数
 *                  表示面和更新面的flip()处理，从表示面到更新面的拷贝处理；
 *                  添加描画命令，执行描画命令
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in]       int             cmd_num     已创建的描画命令个数
 * @param[in/out]   unsigned short*     com     填充描画命令缓冲区
 *
 * @return  int     none
 */
/****************************************************************************/
void SCBoard::Display(int cmd_num, unsigned short* com) const
{
    SCDRAWPARAM     param;
    unsigned int    pal_no, scr_no;


    // 获取当前画面的编号（screen_id）、plate编号；计算切换画面后的画面编号
    PF_GDC_DRV_GetPaletteAndScreen(&pal_no, &scr_no);
    scr_no = 1 - scr_no;

    // Flush处理
    if(cmd_num > SC_DRAW_COM_LM) {
        FlushDrawCom(cmd_num, com);
        cmd_num = 0;
    }

    // 添加Flip命令
    com[cmd_num] = SCDrawCom::makeCom02(pal_no, scr_no);
    cmd_num++;

    // Flush处理
    if(cmd_num > SC_DRAW_COM_LM) {
        FlushDrawCom(cmd_num, com);
        cmd_num = 0;
    }

    // 把表示面拷贝到更新面的命令添加
    param.rect = m_area;
    param.work_scr = scr_no;
    cmd_num += SCDrawCom::makeDisp2UpdateCopy(param, &com[cmd_num]);


    // Flush处理
    FlushDrawCom(cmd_num, com);
}

todo
/****************************************************************************/
/**
 * @brief   FlushDrawCom関数
 * 作成した描画コマンドのFlush処理
 *
 * @author  A.Kubota
 * @date    2016/10/03
 *
 * @param[in]       int             cmd_num     作成済みの描画コマンド数
 * @param[in]       unsigned short* cmd         描画コマンド作成バッファ
 *
 * @return  int     作成された描画コマンド数
 */
/****************************************************************************/
void    SCBoard::FlushDrawCom(
    int             cmd_num,
    unsigned short* cmd
) const
{
    int     sts;
    int     result_num;


    // 描画コマンドをドライバへ
    sts = PF_GDC_DRV_SetMenuCommand(cmd, cmd_num);

    if(sts != 0) {
        // エラー処理
        Timer::sleep(10);
        PF_GDC_DRV_ClearMenuCommand();
        PF_GDC_DRV_SetMenuCommand(cmd, cmd_num);
    }


    // 描画コマンドフラッシュ
    result_num = PF_GDC_DRV_FlushMenuCommand();

    if(result_num != cmd_num) {
        ;   // エラー処理
    }
}


int SCBoard::DrawBackground(
    const SCRect&   area,
    unsigned short* com
)
{
    SCDRAWPARAM     param;
    unsigned short* com_pos;
    int             com_num, all_com_num;
    bool            isDialog = IsDialog();


    // 描画コマンド関連の変数初期化
    com_pos = com;
    all_com_num = com_num = 0;


    // 背景塗りつぶし
    param.work_scr = SC_UPDATE_WORK;    // 表示プレーン(更新用)指定
    param.fill = true;                  // 塗りつぶしあり
    param.back_color = GetBackColor();  // 塗りつぶしの色
    param.rect = area;                  // 塗りつぶし領域
    param.frame = (area == m_area)? isDialog : false;
                                        // 枠線 画面(なし)/Dialog(あり)
    param.fore_color = COLOR_GRAY_88;
    com_num = SCDrawCom::makeRect(param, com_pos);
    com_pos += com_num;
    all_com_num += com_num;

    if(param.frame) {
        SCRect  rect = m_area;

        rect.x += 1;
        rect.y += 1;
        if(rect.width > 2) {
            rect.width -= 2;
        }
        if(rect.height > 2) {
            rect.height -=2;
        }

        param.work_scr = SC_UPDATE_WORK;    // 表示プレーン(更新用)指定
        param.fill = false;                 // 塗りつぶしなし
        param.rect = rect;                  // 領域
        param.frame = true;                 // 枠線 画面(なし)/Dialog(あり)
        param.fore_color = COLOR_GRAY_88;
        com_num = SCDrawCom::makeRect(param, com_pos);
        com_pos += com_num;
        all_com_num += com_num;

    }


    return(all_com_num);
}


void    SCBoard::Initialize(
)
{
    SCParts*    target = GetChild();


    while(target) {
        // 子部品すべてにInitialize()の要求(ただしリスト派生部品は初期化しない)
        if(target->IsFamily(M_RUNTIME_TYPE(SCListManager))) {
            // do nothing;
        }
        else {
            target->Initialize();
        }

        target = target->Next();
    }
}


void    SCBoard::ReDrawDisplay(
)
{
    unsigned short  draw_com[SC_DRAW_COM_MAX];
    int com_num;

    // 画面初期描画(描画コマンド作成)
    com_num = this->Draw(draw_com);

    if(com_num > 0) {
        this->Display(com_num, draw_com);
    }
}


void    SCBoard::UpdateDisplay(
    const int updateType
)
{
    SCBoard*        active;
    unsigned short  draw_com[SC_DRAW_COM_MAX];
    int             com_num;


    active = GetActiveBoard();

    // 更新処理
    com_num = active->Update(updateType, draw_com);

    if(com_num > 0) {
        active->Display(com_num, draw_com);
    }
}


void    SCBoard::TDown(
    const unsigned short    x,
    const unsigned short    y
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // ダイアログがあれば、ダイアログにイベント配送
        active->TDown(x, y);
    }
    else {          // ダイアログがなければ、子部品へ
        SCPoint     point;
        SCParts*    target;

        point.Set(x, y);

        target = GetOperableChild(point);   // 指定点の操作可能な子部品を取得

        if(target) {
            m_captured = target;        // キャプチャー部品とする
            m_captured->TDown(point);   // キャプチャー部品のダウン処理呼び出し
        }
    }
}


void    SCBoard::TMove(
    const unsigned short    x,
    const unsigned short    y,
    const unsigned int      level,
    const int               direction
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // ダイアログがあれば、ダイアログにイベント配送
        active->TMove(x, y, level, direction);
    }
    else {          // ダイアログがなければ、子部品へ
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_captured->TMove(point, level, direction);
                    // キャプチャー部品のムーヴ処理呼び出し
        }
    }
}


void    SCBoard::TUp(
    const unsigned short    x,
    const unsigned short    y
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // ダイアログがあれば、ダイアログにイベント配送
        active->TUp(x, y);
    }
    else {          // ダイアログがなければ、子部品へ
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_touch_point = point;      // リリース位置保存(Callback処理用)
            m_captured->TUp(point);     // キャプチャー部品のアップ処理呼び出し
            m_captured = 0;             // キャプチャー部品をクリア
        }
    }
}


void    SCBoard::TFlick(
    const unsigned short    x,
    const unsigned short    y,
    const unsigned int      level,
    const int               direction
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // ダイアログがあれば、ダイアログにイベント配送
        active->TFlick(x, y, level, direction);
    }
    else {          // ダイアログがなければ、子部品へ
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_touch_point = point;      // フリック位置保存(Callback処理用)
            m_touch_level = level;      // フリックレベル保存(Callback処理用)
            m_touch_direction = direction;  // フリック方向保存(Callback処理用)
            m_captured->TFlick(point, level, direction);
                                    // キャプチャー部品のフリック処理呼び出し
            m_captured = 0;         // キャプチャー部品をクリア
        }
    }
}


void    SCBoard::TLongTap(
    const unsigned short    x,
    const unsigned short    y
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // ダイアログがあれば、ダイアログにイベント配送
        active->TLongTap(x, y);
    }
    else {          // ダイアログがなければ、子部品へ
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_touch_point = point;      // 長押し位置保存(Callback処理用)
            m_captured->TLongTap(point);// キャプチャー部品の長押し処理呼び出し
            m_captured = 0;             // キャプチャー部品をクリア
        }
    }
}


SCParts*    SCBoard::GetChild(
    const unsigned short        x,
    const unsigned short        y
) const
{
    SCPoint point;

    point.Set(x, y);

    return(GetChild(point));
}


SCParts*    SCBoard::GetChild(
    const SCPoint&  point
) const
{
    SCParts*    target;
    SCRect      area;


    target = GetLastChild();


    while(target) {
        // 子部品の領域取得
        target->GetArea(&area);

        // 指定の点が領域に含まれるかチェック
        if(area.Contains(point)) {
            return(target);
        }

        target = target->Prev();
    }


    return(target);
}


SCParts*    SCBoard::GetChild(
    const short id
) const
{
    SCParts*    target = GetChild();


    while(target) {
        // 子部品のID取得
        if(id == target->GetID()) {
            return(target);
        }

        target = target->Next();
    }


    return(target);
}


SCParts*    SCBoard::GetLastChild(
) const
{
    SCParts*    target = GetChild();
    SCParts*    next;


    while(target) {
        next = target->Next();
        if(next == 0) {
            return(target);
        }

        target = next;
    }


    return(target);
}


SCParts*    SCBoard::GetOperableChild(
    const unsigned short        x,
    const unsigned short        y
) const
{
    SCPoint point;

    point.Set(x, y);

    return(GetOperableChild(point));
}


SCParts*    SCBoard::GetOperableChild(
    const SCPoint&  point
) const
{
    SCParts*    target;
    SCRect      area;


    target = GetLastChild();


    while(target) {
        if(target->GetVisible()) {      // 可視の部品のみチェックする

            // 子部品の領域取得
            target->GetArea(&area);

            // 指定の点が領域に含まれるかチェック
            if(area.Contains(point)) {
                if(target->GetEnable() == false) {  // 選択不可であれば
                    target = 0;
                }
                break;
            }
        }

        target = target->Prev();
    }


    return(target);
}


void    SCBoard::RegistParts(
    SCParts*    target
)
{
    SCParts*    last;


    if(target) {
        // 対象部品の親部品として自分自身を登録
        target->Parent(this);

        // 現在の最終登録部品をとってくる
        last = GetLastChild();

        if(last) {
            last->Next(target); // lastの次部品をtargetにする
            target->Prev(last); // targetの前部品をlastにする
        }
        else {
            m_child = target;   // 管理している子部品をtargetにする
            target->Prev(0);    // 対象の前部品をなしにする
        }
        target->Next(0);        // 対象の次部品をなしにする

        // トレンドとの重なり確認(やるならここで)
    }
}


void    SCBoard::UnregistParts(
    SCParts*    target
)
{
    if(target) {
        SCParts*    prev;
        SCParts*    next;

        prev = target->Prev();      // targetの前部品取得
        next = target->Next();      // targetの次部品取得

        if(prev == 0) {             // 長子 or 一人っ子
            m_child = next;         // 管理部品を弟 or なしに
            if(next) {
                next->Prev(0);      // 長子にする
            }
        }
        else {
            prev->Next(next);       // prevの直下をnextにする
            if(next) {
                next->Prev(prev);   // nextの直上をprevにする
            }

            target->Prev(0);        // 前部品をなしにする
        }

        target->Next(0);            // 次部品をなしにする
    }
}


void    SCBoard::DestroyParts(
    SCParts*    target
)
{
    if(target) {
        UnregistParts(target);
        delete target;
    }
}

/****************************************************************************/
/**
 * @brief       DestroyAllParts
 *              销毁所有管理的子部品
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      bool
 * @retval      true
 * @retval      false
 */
/****************************************************************************/
void SCBoard::DestroyAllParts(void)
{
    SCParts*    target;
    SCParts*    next;


    target = GetChild();

    while(target) {
        // 获取下一个子部品
        next = target->Next();

        // 销毁子部品
        delete target;

        target = next;
    }

    m_child = NULL;         // 没有管理的子部品
    m_captured = NULL;      // 没有捕获的touch子部品
}

/****************************************************************************/
/**
 * @brief       GetBackColor
 *              获取背景色
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      返回颜色
 */
/****************************************************************************/
unsigned short  SCBoard::GetBackColor(void)
{
    return( (IsDialog())? COLOR_WHITE : COLOR_GRAY_EE );
}

/****************************************************************************/
/**
 * @brief       IsDialog
 *              是否为dialog，如果存在父画面，就是dialog
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      bool
 * @retval      true
 * @retval      false
 */
/****************************************************************************/
bool    SCBoard::IsDialog(void)
{
    bool rtn = false;

    if(m_parent) 
    {
        rtn = true;
    }

    return(rtn);
}

/****************************************************************************/
/**
 * @brief       GetActiveBoard
 *              返回dialog所在的Board
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      返回dialog所在的Board
 */
/****************************************************************************/
SCBoard*    SCBoard::GetActiveBoard(void)
{
    SCBoard*    rtn = this;

    if(m_dialog) {
        rtn = m_dialog->GetActiveBoard();
    }

    return(rtn);
}

/****************************************************************************/
/**
 * @brief       RegistDialog
 *              注册dialog
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      返回dialog所在的Board
 */
/****************************************************************************/
void SCBoard::RegistDialog( SCBoard* dialog )
{
    if(dialog != NULL) {
        if(dialog->IsFamily(M_RUNTIME_TYPE(SCBoard))) {
            SCBoard*    active;

            active = GetActiveBoard();

            active->SetDialog(dialog);
            dialog->SetParent(active);
        }
    }
}


void        SCBoard::DestroyDialog(
)
{
    SCBoard*    active;


    active = GetActiveBoard();

    if(active != this) {
        SCBoard*    parent;

        parent = active->GetParent();

        parent->SetDialog(0);
        delete active;
    }
}


void        SCBoard::DestroyAllDialog(
)
{
    SCBoard*    active;


    active = GetActiveBoard();

    while(active != this) {
        DestroyDialog();

        active = GetActiveBoard();
    }
}

/************************************************************************************************************/
/**
 * @brief   CreatePassReleaseDlg関数
 * パスワード解除ダイアログの生成
 *
 * @author  Nobuhiro Mitomo
 *
 * @param[in]   int     partsID         部品ID（パスワード解除時のコールバック関数に渡されるID）
 * @param[in]   int     passMode        パスワードモード
 *                                       SCDlgPasswordRelease::PASS_COMMISIONING    Commissioningモード
 *                                       SCDlgPasswordRelease::PASS_EXECUTE         実行モード
 *                                       SCDlgPasswordRelease::PASS_SERVICE         サービスマンモード
 * @param[in]   void*   funcPassRelease 登録するコールバック関数（パスワード解除時）
 *
 * @return  int     作成された描画コマンド数
 */
/************************************************************************************************************/
void SCBoard::CreatePassReleaseDlg(int partsID, int passMode, void (*funcPassRelease)(int))
{
    SCRect rect;
    GetArea(&rect);

    SCDlgPasswordRelease* pDlg = new SCDlgPasswordRelease(rect,static_cast<SCDlgPasswordRelease::ENUM_PASSLOCKINFO>(passMode));
    SCCallbackPasswordRelease* cbkLockRelease = new SCCallbackPasswordRelease(funcPassRelease, partsID);

    pDlg->InitDialog(cbkLockRelease);

    LiqHMIEvRegistDialog(reinterpret_cast<unsigned long>(pDlg));
}

void SCBoard::CreateRunningDlg()
{
    // 確認ダイアログ ボタンデータ構造体
    typedef struct _BTN_MSG
    {
        int  target_id;             // 文字列ID
        void (*btn_callback)(int);  // コールバック関数
    }BTN_MSG;

    // 確認ダイアログ メッセージ文字列データ
    static const int target_id_message_saving[]=
    {
        IDS_DIALOG_SAVING_1,
        IDS_NULL_STRING,
        IDS_DIALOG_SAVING_2,
    };

    // 確認ダイアログ ボタンデータ(ダミー)
    static const BTN_MSG target_btn_message_saving[]=
    {
        {IDS_OK,    0   },
    };

    // ダイアログメッセージ
    SCDlgMsg dlgMsg[sizeof(target_id_message_saving)/sizeof(target_id_message_saving[0])];
    for(unsigned int i = 0; i < sizeof(target_id_message_saving)/sizeof(target_id_message_saving[0]); ++i)
    {
        dlgMsg[i].Set(target_id_message_saving[i],0x0000,SC_ARRANGE_CENTER);
    }

    // ボタン文字列
    SCDlgBtn dlgBtn[sizeof(target_btn_message_saving)/sizeof(target_btn_message_saving[0])];
    for(unsigned int i = 0; i < sizeof(target_btn_message_saving)/sizeof(target_btn_message_saving[0]); ++i)
    {
        dlgBtn[i].Set(target_btn_message_saving[i].target_id,0x0000,SC_ARRANGE_CENTER,target_btn_message_saving[i].btn_callback,0);
    }

    // ダイアログ生成
    DLGCommonBlink* pDLg = new DLGCommonBlink(m_area,3,0,dlgMsg,dlgBtn,0);
    // Mitomo modify
    LiqHMIEvRegistDialog(reinterpret_cast<unsigned long>(pDLg));

    //RegistDialog(pDLg);
    //pDLg->ReDrawDisplay();
}
