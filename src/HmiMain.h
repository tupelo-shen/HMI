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


// 文字间隔信息（0：等宽字体，1：等比例字体）
#define FONT_MONOSPACED     (0)             // 等宽字体
#define FONT_PROPORTIONAL   (1)             // EMC实验使用画面

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
    int                 registFont(unsigned short lang_id, unsigned char* buf);
    int                 registPalette(void);
    int                 VramAccessBlock(unsigned int addr, const void* data, unsigned int size);

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
    
    // static HeapManager* hmi_heap;
};

#endif /* __HMI_EVENT_H__ */
