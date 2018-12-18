#ifndef __HMI_CONTROL_H__
#define __HMI_CONTROL_H__

#include "HmiMain.h"

#define HMIMAIN          HMIControl::getHMI()

class HMIControl
{
public:
    HMIControl();
    virtual ~HMIControl();

    static  HmiMain*        getHMI(){return(hmi_main);}
    static  void            setHMI(HmiMain* real) {hmi_main = real;}
    virtual void            initialize();

private:
    static  HmiMain*        hmi_main;
};
#endif
