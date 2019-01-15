///
/// file        HmiEvent.cpp
/// brief       HMI event class file
///
/// author      Wang.Yu
/// version     00.01.00
/// date        2018/11/26
///
#include "HmiEvent.h"
#include "HMIControl.h"
/// function    HmiEvent
/// brief       constructor
///
/// param       none
/// return      none
HmiEvent::HmiEvent() : 
    m_type(HMI_EV_NONE)
{
}

/// function    HmiEvent
/// brief       constructor
///
/// param       ev : event type
/// return      none
HmiEvent::HmiEvent(HmiEventType ev) : 
    m_type(ev)
{
}

/****************************************************************************/
/**
 * @brief           HmiEvent()
 *                  构造函数
 *
 * @param[in]       ev:     事件类型
 *                  lp:     unsigned long 型参数
 *
 * @return          void
 */
/****************************************************************************/
HmiEvent::HmiEvent(HmiEventType ev, unsigned long lp) :
    m_type(ev),
    lparm(lp)
{
}

/****************************************************************************/
/**
 * @brief           HmiEvent()
 *                  构造函数，根据touch事件数据进行初始化
 *
 * @param[in]       ev:     事件类型
 *                  t_ev:   触摸事件
 *                  pnt:    触摸点的坐标（x，y）
 *                  level:  
 *                  direct: 拖拽方向
 *
 * @return          void
 */
/****************************************************************************/
HmiEvent::HmiEvent(
    HmiEventType                ev,
    pf_touch_event_t            t_ev,
    pf_point_t                  pnt,
    unsigned int                level,
    pf_drag_flick_direction_t   direc
) : m_type(ev)
{
    iparm[0] = (unsigned int)t_ev;
    iparm[1] = level;
    iparm[2] = (unsigned int)direc;

    sparm[0] = pnt.x;
    sparm[1] = pnt.y;
}

/****************************************************************************/
/**
 * @brief           HmiEvent()
 *                  构造函数
 *
 * @param[in]       ev:     事件类型
 *                  parm:   short型参数
 *
 * @return          void
 */
/****************************************************************************/
HmiEvent::HmiEvent(HmiEventType ev, short parm) : 
    m_type(ev)
{
    sparm[0] = (unsigned short)parm;
}

/****************************************************************************/
/**
 * @brief           HmiEvent()
 *                  构造函数
 *
 * @param[in]       ev:     事件类型
 *                  parm:   int型参数
 *
 * @return          void
 */
/****************************************************************************/
HmiEvent::HmiEvent(HmiEventType ev, unsigned int parm) : 
    m_type(ev)
{
    iparm[0] = parm;
}

/****************************************************************************/
/**
 * @brief           HmiEvent()
 *                  构造函数
 *
 * @param[in]       ev:     事件类型
 *                  mode:
 *                  id:     
 *
 * @return          void
 */
/****************************************************************************/
HmiEvent::HmiEvent(HmiEventType ev, unsigned short mode, unsigned short id) : 
    m_type(ev)
{
    sparm[0] = mode;
    sparm[1] = id;
}

/****************************************************************************/
/**
 * @brief           ~HmiEvent()
 *                  析构函数
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
HmiEvent::~HmiEvent()
{
}

/****************************************************************************/
/**
 * @brief           GetTouchEventInfo()
 *                  获取touch事件的信息
 *
 * @param[in/out]   touch_ev: 保存touch事件信息
 *
 * @return          void
 */
/****************************************************************************/
void HmiEvent::GetTouchEventInfo(pf_touch_log_t* touch_ev)
{
    touch_ev->event = (pf_touch_event_t)iparm[0];
    touch_ev->level = iparm[1];
    touch_ev->direction = (pf_drag_flick_direction_t)iparm[2];
    touch_ev->point.x = sparm[0];
    touch_ev->point.y = sparm[1];
}

/****************************************************************************/
/**
 * @brief           HMIChangeScreen()
 *                  切换到指定画面
 *
 * @param[in]       screen_id:  画面ID
 *                  sparm1:     自由设定
 *                  lparm:      自由设定
 *                  iparm0:     自由设定
 *                  iparm1:     自由设定
 *                  iparm2:     自由设定
 *
 * @return          true：       成功
 *                  false：      失败
 */
/****************************************************************************/
extern "C" bool HMIChangeScreen(
    short           screen_id,
    unsigned short  sparm1,
    unsigned long   lparm,
    unsigned int    iparm0,
    unsigned int    iparm1,
    unsigned int    iparm2
)
{
    bool    rtn = false;


    if(HMIMAIN != 0) {
        HMIEventChangeScreen(screen_id, sparm1, lparm, iparm0, iparm1, iparm2);
        rtn = true;
    }


    return(rtn);
}

/****************************************************************************/
/**
 * @brief           HMIChangeScreen()
 *                  切换到指定画面
 *
 * @param[in]       screen_id:  画面ID
 *                  sparm1:     自由设定
 *                  lparm:      自由设定
 *                  iparm0:     自由设定
 *                  iparm1:     自由设定
 *                  iparm2:     自由设定
 *
 * @return          true：       成功
 *                  false：      失败
 */
/****************************************************************************/
extern "C" void HMIEventChangeScreen(
    short           screen_id,
    unsigned short  sparm1,
    unsigned long   lparm,
    unsigned int    iparm0,
    unsigned int    iparm1,
    unsigned int    iparm2
)
{
    // 生成event
    HmiEvent ev_info(HMI_EV_CHG_SCREEN, screen_id);

    // 自由设定的参数
    ev_info.SetUShort(1, sparm1);
    ev_info.SetULong(lparm);
    ev_info.SetUInt(0, iparm0);
    ev_info.SetUInt(1, iparm1);
    ev_info.SetUInt(2, iparm2);

    // {
        // LockGuard   lock(HMIMAIN->mutex());

        HMIMAIN->setReady(true);
        HMIMAIN->addEventQueue(ev_info);
    // }
    // HMIMAIN->conditionVariable().notify_one();
}