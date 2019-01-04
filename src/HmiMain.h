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
    virtual void        start(void);
    void                main(void);    
    bool                IsReady(void) {return(is_ready);}
    void                SetReady(bool ready) {is_ready = ready;}
    // std::mutex& Mutex(void) {return(mtx);}
    // std::condition_variable& ConditionVariable(void) {
    //     return(cv);
    // }
    void                AddEventQueue(HmiEvent ev) {q_events.push(ev);}
    bool                GetEventQueue(HmiEvent& ev) {return q_events.try_pop(ev);}
    bool                IsEmpty(void) {return q_events.empty();}


    void                eventHandler(HmiEvent& ev);
    int                 registPalette(void);

    // static HeapManager* getHMIHeap(void){return (hmi_heap);}

private :
    // std::deque<HmiEvent>   deq;
    // std::mutex              mtx;
    // std::condition_variable cv;
    Queue_s<HmiEvent>   q_events;
    bool                is_ready;
    
    // static HeapManager* hmi_heap;
};

#endif /* __HMI_EVENT_H__ */
