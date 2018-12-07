/*
 * @Module Name：    HmiEvent.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-12-06
 */
#ifndef __HMI_EVENT_H__
#define __HMI_EVENT_H__

class HmiEvent
{
    typedef union _HmiEventParam
    {
        unsigned long   lp;
        unsigned int    ip[2];
        unsigned short  sp[4];
        
        _HmiEventParam() {}
        _HmiEventParam(unsigned long ul) {lp = ul;}
    } EHmiEventParam;
public:
    typedef enum 
    {
        HMI_EV_NONE,
        HMI_EV_KEYDOWN,
    } HmiEventType;

public:
    HmiEvent();
    HmiEvent(HmiEventType ev);
    HmiEvent(HmiEventType ev, unsigned long lp);
    ~HmiEvent();

    HmiEventType        GetEvent() {return(type);}
    unsigned long       GetULArg() {return(arg.lp);}
    void                SetULArg(unsigned long ul) {arg.lp = ul;}
private:
    HmiEventType        type;
    _HmiEventParam      arg;
};

#endif /* __HMI_EVENT_H__ */