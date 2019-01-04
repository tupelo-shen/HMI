/**
* @file     sclCallback.h
*           回调函数 头文件
*
* @author   Tupelo
* @version  V1.01.01    2018/12/29
*/
#ifndef __SCL_CALLBACK_H__
#define __SCL_CALLBACK_H__

#include "sclBase.h"

/*
 * Callback 类定义使用
 */
enum SCCallbackType {
    SCCallbackTypeTAP = 0,      // Tap操作
    SCCallbackTypeFLICK,        // Flick操作
    SCCallbackTypeLONGTAP,      // 长按操作
    SCCallbackTypeCHANGEVALUE   // 值发生变化时的操作(不使用的话删除即可)
};


/*
 * Callback 类定义
 */
class SCCallback
{
public :
    SCCallback(int type, void (*func)(int), int clientData);
    virtual ~SCCallback();

    void            DoCallback(void);

    SCCallback*     Next(void) const {return(m_next);}
    void            Next(SCCallback* next) {m_next = next;}

    int             GetType(void) const {return(m_type);}
    
private :
    void            (*m_func)(int clientData);
    int             m_type;         // 回调函数的类型
    int             m_clientData;   // 毁掉函数执行时的参数
    SCCallback*     m_next;         // 指向下一个回调函数
};

#endif  /* __SCL_CALLBACK_H__ */