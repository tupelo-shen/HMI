/**
* @file sclBoard.cpp
*           
*
* @author   Tupelo
* @version  2018-12-27
*/

#include <cstddef>
#include <thread>
#include <chrono>
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
// M_IMPLEMENT_RUNTIME(SCBoard, SCUpdateTypeCYCLIC, SCCore)

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

/****************************************************************************/
/**
 * @brief           FlushDrawCom 函数
 *                  刷新作成的描画命令
 *
 * @author  
 * @date    
 *
 * @param[in]       int             cmd_num     作成的描画命令数量
 * @param[in]       unsigned short* cmd         作成的描画命令缓冲buffer
 *
 * @return    
 */
/****************************************************************************/
void    SCBoard::FlushDrawCom(
    int             cmd_num,
    unsigned short* cmd
) const
{
    int     sts;
    int     result_num;


    // 向驱动程序发送描画命令
    sts = PF_GDC_DRV_SetMenuCommand(cmd, cmd_num);

    if(sts != 0) 
    {
        // 错误处理
        // Timer::sleep(10);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Todo: 如果是嵌入式系统，需要自己实现时间处理函数
        PF_GDC_DRV_ClearMenuCommand();
        PF_GDC_DRV_SetMenuCommand(cmd, cmd_num);
    }


    // Flush 描画命令
    result_num = PF_GDC_DRV_FlushMenuCommand();

    if(result_num != cmd_num) 
    {
        ;   // 错误处理
    }
}

/****************************************************************************/
/**
 * @brief           DrawBackground 函数
 *                  描画背景。
 *                  思路： 对描画的矩形区域，使用背景色进行填充。如果描画带边框的区域
 *                  将要描画的区域减去边框所占的宽度后剩余的区域进行特定颜色的填充。
 *
 * @author  
 * @date    
 *
 * @param[in]       const SCRect&   area        描画区域
 * @param[in]       unsigned short* com         描画命令
 *
 * @return          int     更新后的描画命令数
 */
/****************************************************************************/
int SCBoard::DrawBackground(
    const SCRect&   area,
    unsigned short* com
)
{
    SCDRAWPARAM     param;
    unsigned short* com_pos;
    int             com_num, all_com_num;
    bool            isDialog = IsDialog();


    // 描画命令相关变量的初始化
    com_pos = com;
    all_com_num = com_num = 0;


    // 背景填充
    param.work_scr = SC_UPDATE_WORK;    // 指定表示Plane(更新用)
    param.fill = true;                  // 填充
    param.back_color = GetBackColor();  // 填充颜色
    param.rect = area;                  // 填充区域
    // param.frame = (area == m_area)? isDialog : false;
    param.frame = true; // 临时修改-沈万江2019/01/18;
                                        // 边框 画面（没有）/Dialog（有）
    param.fore_color = COLOR_GRAY_88;
    com_num = SCDrawCom::makeRect(param, com_pos);
    com_pos += com_num;
    all_com_num += com_num;

    if(param.frame) 
    {
        SCRect  rect = m_area;

        rect.x += 1;
        rect.y += 1;
        if(rect.width > 2) 
        {
            rect.width -= 2;
        }
        if(rect.height > 2) 
        {
            rect.height -=2;
        }

        param.work_scr = SC_UPDATE_WORK;    // 指定表示Plane(更新用)
        param.fill = false;                 // 非填充
        param.rect = rect;                  // 描画区域
        param.frame = true;                 // 边框 画面（没有）/Dialog（有）
        param.fore_color = COLOR_GRAY_88;
        com_num = SCDrawCom::makeRect(param, com_pos);
        com_pos += com_num;
        all_com_num += com_num;

    }

    return(all_com_num);
}

/****************************************************************************/
/**
 * @brief           Initialize 函数
 *                  初始化函数
 *
 * @author  
 * @date    
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::Initialize(void)
{
    SCParts*    target = GetChild();


    while(target) {
        // 调用所有子部品的Initialize（）函数，对其进行初始化。但是对于List派生部品，没有初始化。
        // if(target->IsFamily(M_RUNTIME_TYPE(SCListManager))) 
        // {
        //     // do nothing;
        // }
        // else {
        //     target->Initialize();
        // }
        target->Initialize();
        target = target->Next();
    }
}

/****************************************************************************/
/**
 * @brief           ReDrawDisplay 函数
 *                  重绘函数
 *
 * @author  
 * @date    
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::ReDrawDisplay(void)
{
    unsigned short  draw_com[SC_DRAW_COM_MAX];
    int com_num;

    // 画面初始化描画(作成描画命令)
    com_num = this->Draw(draw_com);

    if(com_num > 0) {
        this->Display(com_num, draw_com);
    }
}

/****************************************************************************/
/**
 * @brief           UpdateDisplay 函数
 *                  更新显示
 *
 * @author  
 * @date    
 *
 * @param[in]       const int updateType    更新类型
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::UpdateDisplay(
    const int updateType
)
{
    SCBoard*        active;
    unsigned short  draw_com[SC_DRAW_COM_MAX];
    int             com_num;


    active = GetActiveBoard();

    // 更新处理
    com_num = active->Update(updateType, draw_com);

    if(com_num > 0) {
        active->Display(com_num, draw_com);
    }
}

/****************************************************************************/
/**
 * @brief           TDown 函数
 *                  鼠标/触摸按下事件处理函数
 *
 * @author  
 * @date    
 *
 * @param[in]       const unsigned short x   按下位置的x坐标
 * @param[in]       const unsigned short y   按下位置的y坐标
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::TDown(
    const unsigned short    x,
    const unsigned short    y
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) 
    {   
        // 如果存在Dialog，将PressDown事件发送给Dialog处理
        active->TDown(x, y);
    }
    else 
    {          
        // 如果不存在Dialog，则交由相应的子部品进行处理
        SCPoint     point;
        SCParts*    target;

        point.Set(x, y);

        target = GetOperableChild(point);   // 取得指定点的可操作子部品

        if(target) 
        {
            m_captured = target;        // 成为捕获焦点的子部品
            m_captured->TDown(point);   // 调用子部品的Down处理函数
        }
    }
}

/****************************************************************************/
/**
 * @brief           TMove 函数
 *                  鼠标/触摸拖动事件处理函数
 *
 * @author  
 * @date    
 *
 * @param[in]       const unsigned short x      拖动位置的x坐标
 * @param[in]       const unsigned short y      拖动位置的y坐标
 * @param[in]       const unsigned int level    拖动层次
 * @param[in]       const int direction         拖动方向
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::TMove(
    const unsigned short    x,
    const unsigned short    y,
    const unsigned int      level,
    const int               direction
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) 
    {    
        // 如果存在上层Dialog，将Move事件发送给上层Dialog处理
        active->TMove(x, y, level, direction);
    }
    else 
    {          
        // 如果不存在上层Dialog，则交由相应的子部品进行处理
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_captured->TMove(point, level, direction);
                    // 调用当前获得焦点的子部品的Move处理函数
        }
    }
}

/****************************************************************************/
/**
 * @brief           TUp 函数
 *                  鼠标/触摸抬起事件处理函数
 *
 * @author  
 * @date    
 *
 * @param[in]       const unsigned short x   抬起位置的x坐标
 * @param[in]       const unsigned short y   抬起位置的y坐标
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::TUp(
    const unsigned short    x,
    const unsigned short    y
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // 如果存在上层Dialog，将Up事件发送给上层Dialog处理
        active->TUp(x, y);
    }
    else {          // 如果不存在上层Dialog，则交由相应的子部品进行处理
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_touch_point = point;      // 保存释放位置(Callback处理使用)
            m_captured->TUp(point);     // 调用当前获取焦点的子部品的Up处理函数
            m_captured = 0;             // 当前获取焦点的子部品释放掉焦点
        }
    }
}

/****************************************************************************/
/**
 * @brief           TFlick 函数
 *                  鼠标/触摸拖动事件处理函数
 *
 * @author  
 * @date    
 *
 * @param[in]       const unsigned short x      拖动位置的x坐标
 * @param[in]       const unsigned short y      拖动位置的y坐标
 * @param[in]       const unsigned int level    拖动层次
 * @param[in]       const int direction         拖动方向
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::TFlick(
    const unsigned short    x,
    const unsigned short    y,
    const unsigned int      level,
    const int               direction
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // 如果存在上层Dialog，将Up事件发送给上层Dialog处理
        active->TFlick(x, y, level, direction);
    }
    else {          // 如果不存在上层Dialog，则交由相应的子部品进行处理
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_touch_point = point;              // 保存Flick位置(Callback回调函数使用)
            m_touch_level = level;              // 保存Flick Level(Callback回调函数使用)
            m_touch_direction = direction;      // 保存Flick方向(Callback回调函数使用)
            m_captured->TFlick(point, level, direction);
                                                // 调用当前获取焦点的子部品的Flick处理函数
            m_captured = 0;                     // 当前获取焦点的子部品释放掉焦点
        }
    }
}

/****************************************************************************/
/**
 * @brief           TLongTap 函数
 *                  鼠标/触摸长按事件处理函数
 *
 * @author  
 * @date    
 *
 * @param[in]       const unsigned short x   按下位置的x坐标
 * @param[in]       const unsigned short y   按下位置的y坐标
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::TLongTap(
    const unsigned short    x,
    const unsigned short    y
)
{
    SCBoard*    active;


    active = GetActiveBoard();
    if(active != this) {    // 如果存在上层Dialog，将Up事件发送给上层Dialog处理
        active->TLongTap(x, y);
    }
    else {          // 如果不存在上层Dialog，则交由相应的子部品进行处理
        if(m_captured) {
            SCPoint point;

            point.Set(x, y);
            m_touch_point = point;      // 保存长按位置(Callback回调函数使用)
            m_captured->TLongTap(point);// 调用当前获取焦点的子部品的LongTap处理函数
            m_captured = 0;             // 当前获取焦点的子部品释放掉焦点
        }
    }
}

/****************************************************************************/
/**
 * @brief           GetChild 函数
 *                  根据指定位置获取相应的子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       const unsigned short x   子部品的位置x坐标
 * @param[in]       const unsigned short y   子部品的位置y坐标
 *
 * @return          SCParts*        子部品的指针
 */
/****************************************************************************/
SCParts*    SCBoard::GetChild(
    const unsigned short        x,
    const unsigned short        y
) const
{
    SCPoint point;

    point.Set(x, y);

    return(GetChild(point));
}

/****************************************************************************/
/**
 * @brief           GetChild 函数
 *                  根据指定位置获取相应的子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       const SCPoint&   子部品的指定位置坐标point（x，y）
 *
 * @return          SCParts*        子部品的指针
 */
/****************************************************************************/
SCParts*    SCBoard::GetChild(
    const SCPoint&  point
) const
{
    SCParts*    target;
    SCRect      area;


    target = GetLastChild();


    while(target) {
        // 获取子部品的区域
        target->GetArea(&area);

        // 检查子部品的区域中是否包含指定的坐标点point（x，y）
        if(area.Contains(point)) {
            return(target);
        }

        target = target->Prev();
    }


    return(target);
}

/****************************************************************************/
/**
 * @brief           GetChild 函数
 *                  根据指定ID获取相应的子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       const SCPoint& id  子部品的指定ID
 *
 * @return          SCParts*        子部品的指针
 */
/****************************************************************************/
SCParts*    SCBoard::GetChild(
    const short id
) const
{
    SCParts*    target = GetChild();


    while(target) {
        // 与子部品分配的ID进行比较
        if(id == target->GetID()) {
            return(target);
        }

        target = target->Next();
    }


    return(target);
}

/****************************************************************************/
/**
 * @brief           GetLastChild 函数
 *                  获得最后一个子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       void
 *
 * @return          SCParts*        子部品的指针
 */
/****************************************************************************/
SCParts*    SCBoard::GetLastChild(void) const
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

/****************************************************************************/
/**
 * @brief           GetOperableChild 函数
 *                  获取可操作的子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       const unsigned short x   子部品的位置x坐标
 * @param[in]       const unsigned short y   子部品的位置y坐标
 *
 * @return          SCParts*        子部品的指针
 */
/****************************************************************************/
SCParts*    SCBoard::GetOperableChild(
    const unsigned short        x,
    const unsigned short        y
) const
{
    SCPoint point;

    point.Set(x, y);

    return(GetOperableChild(point));
}

/****************************************************************************/
/**
 * @brief           GetOperableChild 函数
 *                  获取可操作的子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       const SCPoint&   子部品的指定位置坐标point（x，y）
 *
 * @return          SCParts*        子部品的指针
 */
/****************************************************************************/
SCParts*    SCBoard::GetOperableChild(
    const SCPoint&  point
) const
{
    SCParts*    target;
    SCRect      area;


    target = GetLastChild();


    while(target) {
        if(target->GetVisible()) {      // 仅仅检查可见部品

            // 获取子部品的区域
            target->GetArea(&area);

            // 是否包含指定的坐标点
            if(area.Contains(point)) {
                if(target->GetEnable() == false) {  // 该部品是否被使能
                    target = 0;
                }
                break;
            }
        }

        target = target->Prev();
    }


    return(target);
}

/****************************************************************************/
/**
 * @brief           RegistParts 函数
 *                  注册子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       SCParts* target 要注册的子部品
 *
 * @return          void
 */
/****************************************************************************/
void    SCBoard::RegistParts(
    SCParts*    target
)
{
    SCParts*    last;

    if(target) 
    {
        // 将自己注册为指定对象部品的父部品
        target->Parent(this);

        // 检索当前注册的最后一个子部品
        last = GetLastChild();

        if(last) {
            last->Next(target); // 将target作为last的下一个部品
            target->Prev(last); // 将target的上一个部品设为last
        }
        else {
            m_child = target;   // 把target作为管理的子部品
            target->Prev(0);    // 该target没有前一个部品
        }
        target->Next(0);        // 对象的下一个部品设为没有

        // 确认是否与trend重叠
    }
}

/****************************************************************************/
/**
 * @brief           UnregistParts 函数
 *                  解除注册子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       SCParts* target 要解除注册的子部品
 *
 * @return          void
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 * @brief           DestroyParts 函数
 *                  销毁指定的子部品
 *
 * @author  
 * @date    
 *
 * @param[in]       SCParts* target 要销毁的子部品
 *
 * @return          void
 */
/****************************************************************************/
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
    // return( (IsDialog())? COLOR_WHITE : COLOR_YELLOW); // Todo: COLOR_GRAY_EE );
    return( (IsDialog())? COLOR_WHITE : COLOR_GRAY_EE); // Todo: COLOR_GRAY_EE );
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

/****************************************************************************/
/**
 * @brief       DestroyDialog
 *              销毁Dialog
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      返回dialog所在的Board
 */
/****************************************************************************/
void        SCBoard::DestroyDialog(void)
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

/****************************************************************************/
/**
 * @brief       DestroyAllDialog
 *              销毁所有Dialog
 *
 * @author      Tupelo
 * @date        2018/12/27
 *
 * @param[in]   void
 *
 * @return      返回dialog所在的Board
 */
/****************************************************************************/
void        SCBoard::DestroyAllDialog(void)
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
    // SCRect rect;
    // GetArea(&rect);

    // SCDlgPasswordRelease* pDlg = new SCDlgPasswordRelease(rect,static_cast<SCDlgPasswordRelease::ENUM_PASSLOCKINFO>(passMode));
    // SCCallbackPasswordRelease* cbkLockRelease = new SCCallbackPasswordRelease(funcPassRelease, partsID);

    // pDlg->InitDialog(cbkLockRelease);

    // LiqHMIEvRegistDialog(reinterpret_cast<unsigned long>(pDlg));
}

void SCBoard::CreateRunningDlg()
{
    #if 0
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
    #endif
}
