///
/// file        HmiMain.cpp
/// brief       HMI main process file
///
/// author      Wang.Yu
/// version     00.01.00
/// date        2018/11/26
///
#include <thread>
#include "Queue-pthread_s.h"
#include "HmiMain.h"
#include "HmiEvent.h"
#include "HMIControl.h"

extern "C" void SendEvent2HMI(
    pf_touch_event_t            ev,
    pf_point_t                  pnt,
    unsigned int                level,
    pf_drag_flick_direction_t   direc
)
{
    // HmiEvent ev(HMI_EV_MOUSE_PRESSED, (unsigned long)pnt.x);
    HmiEvent ev_info(HmiEvent::HMI_EV_MOUSE_PRESSED, (unsigned long)pnt.x);
    HMIMAIN->SetReady(true);
    HMIMAIN->AddEventQueue(ev_info);
}
/// function    HmiMain
/// brief       constructor
HmiMain::HmiMain() : is_ready(false)
{
}

/// function    ~HmiMain
/// brief       deconstructor
HmiMain::~HmiMain()
{
}

/// function    main
/// brief       HMI main process function
///
/// param       none
/// return      none
void HmiMain::main(void)
{
    // 鼠标等输入设备的配置
    pf_touch_config_t           t_conf;

    t_conf.fpga_sampling = 0;
    t_conf.flick_sampling = 0;
    t_conf.longtap_detection_time = 0;
    t_conf.touch_threshold = 0;
    t_conf.drag_flick_threshold_level1 = 0;
    t_conf.drag_flick_threshold_level2 = 0;
    t_conf.drag_flick_threshold_level3 = 0;
    t_conf.width_resolution = 0;
    t_conf.height_resolution = 0;

    GDC_DRV_SetTouchPanelConfigData(SendEvent2HMI, &t_conf);

    HmiEvent ev(HmiEvent::HMI_EV_NONE);
    
    while(true) 
    {
        // 事件处理
        while(true) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // 事件处理
            if(!IsEmpty() && GetEventQueue((HmiEvent&) ev))
            {
                eventHandler(ev);
            }
        }
    }
}

/// function    eventHandler
/// brief       HMI main event process function
///
/// param       ev  : event popped from queue
/// return      none
void HmiMain::eventHandler(HmiEvent& ev)
{
    HmiEvent::HmiEventType type = HmiEvent::HMI_EV_NONE;
    unsigned long param;

    // get event type & param
    type = ev.GetEvent();
    switch(type) {
    case HmiEvent::HMI_EV_KEYDOWN:
        param = ev.GetULArg();
        std::cout << std::endl << "\"" << (char)param << "\" key is pushed down!" << std::endl;
        // std::cout << std::endl << "\"" << param << "\" key is pushed down!" << std::endl;
        break;
    case HmiEvent::HMI_EV_MOUSE_PRESSED:
        param = ev.GetULArg();
        std::cout << std::endl << "\"" << (unsigned long)param << "\" key is pushed down!" << std::endl;
    default: // HMI_EV_NONE or not defined
        break;
    }
}
