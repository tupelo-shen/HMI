/*
 * @Module Name：    HmiMain.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-12-06
 */
#ifndef __HMI_MAIN_H__
#define __HMI_MAIN_H__

#include "Queue-pthread_s.h"
#include "HmiEvent.h"
#include "GDC_Driver.h"
#include "sclBoard.h"

#define HMI_SCREEN_W        (320)           // 画面宽(dot)
#define HMI_SCREEN_H        (240)           // 画面高(dot)
// 文字间隔信息（0：等宽字体，1：等比例字体）
#define FONT_MONOSPACED     (0)             // 等宽字体
#define FONT_PROPORTIONAL   (1)             // EMC实验使用画面

// 画面分类
enum 
{
    HMI_SCREEN_NONE=-1,         // 画面無
    HMI_SCREEN_START=5001,      // 先頭画面
    HMI_SCREEN_CAL_LIST = HMI_SCREEN_START, // 校正リスト画面
    HMI_SCREEN_CAL,             // 校正画面

    // Main画面
    HMI_SCREEN_MAIN,            // Main画面
    HMI_SCREEN_TREND,           // TREND画面

    // Monitor画面
    HMI_SCREEN_MONITOR,

    // Home画面
    HMI_SCREEN_HOME,

    // 変換器メニュー画面
    HMI_SCREEN_CONVERTER_MENU,  // 変換器メニュー画面
    HMI_SCREEN_CONV_DETAIL,     // 変換器詳細画面
    HMI_SCREEN_CONV_OPE_MENU,   // オペレーションメニュー画面
    HMI_SCREEN_ADJUST_TOUCH,    // タッチパネル調整・確認画面
    HMI_SCREEN_ADJUST_BRIGHTNESS,// 輝度調整画面
    HMI_SCREEN_ADJUST_TIME,     // 時刻設定画面

    // パスワード設定画面
    HMI_SCREEN_PASSWORD_SETTING,

    // 設定画面
    HMI_SCREEN_SETTING,

    // アラーム画面
    HMI_SCREEN_ALARM_LIST,      // Alarmリスト画面
    HMI_SCREEN_ALARM_DETAIL,    // Alarm詳細画面

    // Bluetooth画面
    HMI_SCREEN_BLUETOOTH_PAIRING,

    // ファイルチューザ画面
    HMI_SCREEN_FILE_CHOOSER,

    // AI/AO調整画面
    HMI_SCREEN_AO_ADJUST,
    HMI_SCREEN_AI_ADJUST,

    // センサメニュー画面
    HMI_SCREEN_SENSOR_MENU,     // センサメニュー画面
    HMI_SCREEN_SENSOR_OPE_MENU, // センサオペレーションメニュー画面

    HMI_SCREEN_SENSOR_DETAIL,   // センサ詳細画面

    // モドバスアドレスサーチ画面
    HMI_SCREEN_MODBUS_SEARCH,   // モドバスアドレスサーチ画面

    HMI_SCREEN_SELECT_LANGUAGE, // 言語選択画面

    HMI_SCREEN_EMC,             // EMC試験用画面
    HMI_SCREEN_END,             // 全画面数
    // これより後には画面IDを追加しない

    HMI_SCREEN_TEST=9999        // Test用画面
};

class HmiMain {
public :
    HmiMain();
    virtual ~HmiMain();
    
public :
    virtual void        start(void);
    void                main(void);    
    bool                isReady(void) {return(is_ready);}
    void                setReady(bool ready) {is_ready = ready;}
    // std::mutex& Mutex(void) {return(mtx);}
    // std::condition_variable& ConditionVariable(void) {
    //     return(cv);
    // }
    void                addEventQueue(HmiEvent ev) {q_events.push(ev);}
    bool                getEventQueue(HmiEvent& ev) {return q_events.try_pop(ev);}
    bool                isEmpty(void) {return q_events.empty();}

    // 事件处理
    void                eventHandler(HmiEvent& ev);

    // 初始化及相应的配置设置
    void                configureTouchPanel(void);
    int                 registFont(unsigned short lang_id, unsigned char* buf);
    int                 registPalette(void);
    int                 VramAccessBlock(unsigned int addr, const void* data, unsigned int size);

    // 画面刷新
    void                updateCyclic(void);
    void                updateTimer(void);

    // 画面操作
    SCBoard*            getActiveBoard();
    SCBoard*            getBoard(void) {return(m_screen);}
    short               getScreenID(void) {return(m_screen_id);}
    void                startScreen(void);
    void                changeScreen(short screen_id, HmiEvent& ev_info,
                            bool isNeedForChangePreScreenID = true);
    void                setNewScreen(short screen_id, HmiEvent& ev_info);
    void                procTouch(pf_touch_log_t& t_ev);
private:
    void                loadFile(const char * fname, unsigned char* fdata, unsigned long* fsize);
    unsigned int        readLine(const char* src, char* dst);
    unsigned int        get2CharsFromStr(const char* src, char* dst);
    unsigned char       strToUC(const char* src);
    unsigned long       convertToBin(const char* src, unsigned char* dst, unsigned long size);
    // static HeapManager* getHMIHeap(void){return (hmi_heap);}

private :
    // std::deque<HmiEvent>   deq;
    // std::mutex              mtx;
    // std::condition_variable cv;
    Queue_s<HmiEvent>   q_events;
    bool                is_ready;
    
    short               m_screen_id;
    SCBoard*            m_screen;
    bool                m_hmi_timer;

    // static HeapManager* hmi_heap;
};

#endif /* __HMI_EVENT_H__ */
