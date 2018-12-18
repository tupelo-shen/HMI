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

class HmiMain {
public :
    HmiMain();
    virtual ~HmiMain();
    
public :
    void                Start(void) {main();}
    bool                IsReady(void) {return(is_ready);}
    void                SetReady(bool ready) {is_ready = ready;}
    // std::mutex& Mutex(void) {return(mtx);}
    // std::condition_variable& ConditionVariable(void) {
    //     return(cv);
    // }
    void                AddEventQueue(HmiEvent ev) {q_events.push(ev);}
    bool                GetEventQueue(HmiEvent& ev) {return q_events.try_pop(ev);}
    bool                IsEmpty(void) {return q_events.empty();}
    
private :
    void                main(void);
    void                eventHandler(HmiEvent& ev);
// public:
//     static void         SendEvent2HMI(pf_touch_event_t            ev,
//                                         pf_point_t                  pnt,
//                                         unsigned int                level,
//                                         pf_drag_flick_direction_t   direc);

private :
    // std::deque<HmiEvent>   deq;
    // std::mutex              mtx;
    // std::condition_variable cv;
    Queue_s<HmiEvent>   q_events;
    bool                is_ready;

};

#endif /* __HMI_EVENT_H__ */
