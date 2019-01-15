/*
 * @Module Name：    HmiEvent.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-12-06
 */
#ifndef __HMI_EVENT_H__
#define __HMI_EVENT_H__

#include "GDC_Driver.h"

// event type
typedef enum 
{
    HMI_EV_NONE,                                // none
    HMI_EV_RESTART,                             // restart
    HMI_EV_CHG_SCREEN,                          // change screen
    HMI_EV_CHG_SCREEN_FOR_NOT_UPDATING_ID,      // change screen(pre_screen_id unchanged)
    HMI_EV_REFRESH_SCREEN,                      // repaint the screen
    // HMI_EV_RESET_SCREEN,                        // 設定再取得＆Dialog消去＆画面再描画
    // HMI_EV_DATABASE,                // データベース更新
    HMI_EV_CYCLIC,                  // 定周期イベント
    // HMI_EV_TREND,                   // トレンド更新周期
    HMI_EV_TOUCH,                               // touch event
    // HMI_EV_BLINK_ON,                // ブリンクOn(一つにするかも)x_kubota
    // HMI_EV_BLINK_OFF,               // ブリンクOff
    HMI_EV_REGIST_DLOG,                         // regist dialog
    HMI_EV_DESTROY_DLOG,                        // destroy dialog
    HMI_EV_TIMER,                               // timer update
    HMI_EV_TIMER_ENABLE,                        // timer enabled
    HMI_EV_TIMER_DISABLE,                       // timer disabled
    // HMI_EV_ALARM_ON,                // アラーム点灯イベント
    // HMI_EV_ALARM_OFF,               // アラーム消灯イベント
    // HMI_EV_SQUAWK,                  // スコーク設定ベント
    // HMI_EV_SQUAWK_ON,               // スコーク点灯イベント
    // HMI_EV_SQUAWK_OFF,              // スコーク消灯イベント
    // HMI_EV_SQUAWK_END,              // スコーク終了(HMIからの終了処理)
    // HMI_EV_CLEAR_WATCH_DOG,         // WatchDogカウントクリアイベント
    // HMI_EV_FILE_OPE_RESULT          // ファイル関連処理の結果通知
} HmiEventType;

class HmiEvent
{    
public:
    HmiEvent();
    HmiEvent(HmiEventType ev);
    HmiEvent(HmiEventType ev, unsigned long lp);
    HmiEvent(HmiEventType ev, pf_touch_event_t t_ev, pf_point_t pnt,
            unsigned int level, pf_drag_flick_direction_t direc);
    HmiEvent(HmiEventType ev, short parm);
    HmiEvent(HmiEventType ev, unsigned int parm);
    HmiEvent(HmiEventType ev, unsigned short mode, unsigned short id);
    ~HmiEvent();

    HmiEventType        GetEvent() {return(m_type);}
    unsigned long       GetULong() {return(lparm);}
    void                SetULong(unsigned long ul) {lparm = ul;}
    void                GetTouchEventInfo(pf_touch_log_t* touch_ev);
    unsigned short      GetUShort(int arrayNo) {return(sparm[arrayNo]);}
    void                SetUShort(int arrayNo, unsigned short parm) {sparm[arrayNo] = parm;}
    unsigned int        GetUInt(int arrayNo) {return(iparm[arrayNo]);}
    void                SetUInt(int arrayNo, unsigned int parm) {iparm[arrayNo] = parm;}
private:
    HmiEventType        m_type;
    unsigned long       lparm;
    unsigned int        iparm[3];
    unsigned short      sparm[2];
};

/*** 将事件添加到事件队列中的处理函数 ***/
#ifdef __cplusplus
extern "C" {
#endif
bool                    HMIChangeScreen(short screen_id, unsigned short sparm1,
                                unsigned long lparm, unsigned int iparm0,
                                unsigned int iparm1, unsigned int iparm2);
void                    HMIEventChangeScreen(short screen_id, unsigned short sparm1,
                            unsigned long lparm, unsigned int iparm0,
                            unsigned int iparm1, unsigned int iparm2);
#ifdef __cplusplus
}   /* extern C */
#endif

#endif /* __HMI_EVENT_H__ */