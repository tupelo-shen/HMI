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
        break;
    default: // HMI_EV_NONE or not defined
        break;
    }
}
