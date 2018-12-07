///
/// file        HmiEvent.cpp
/// brief       HMI event class file
///
/// author      Wang.Yu
/// version     00.01.00
/// date        2018/11/26
///
#include "HmiEvent.h"

/// function    HmiEvent
/// brief       constructor
///
/// param       none
/// return      none
HmiEvent::HmiEvent() : type(HMI_EV_NONE)
{
}

/// function    HmiEvent
/// brief       constructor
///
/// param       ev : event type
/// return      none
HmiEvent::HmiEvent(HmiEventType ev) : type(ev)
{
}

/// function    HmiEvent
/// brief       constructor
///
/// param       ev : event type
/// param       ul : event type
/// return      none
HmiEvent::HmiEvent(HmiEventType ev, unsigned long lp) :
    type(ev),
    arg(lp)
{
}

/// function    ~HmiEvent
/// brief       deconstructor
HmiEvent::~HmiEvent()
{
}