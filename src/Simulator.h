/*
 * @Module Name：    Simulator.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-02-27
 */
#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <gtk/gtk.h>
#include <string>
#include "HMI_BMPDump.h"

using namespace std;

class Options
{
public:
    Options(const char *f_name);
    virtual ~Options(void);

    static Options      &refObject(void);

public:
    string              iniFileName;
    string              uart[5];
    string              logSwitch;
    string              logFolder;
    string              logFileName;
    string              progtamFlash;
    string              dataFlash;
    string              sensor_ch[2];
private:
    static Options      *obj;    
};

class Simulator
{
public:
    Simulator(void);
    Simulator(Simulator& obj);
    virtual ~Simulator(void);

    static Simulator&               refObject(void);
    int                             gtkmain(int argc, char* argv[]);
    void                            initBmp();
    SimulatorPlatform::BitMapDump*  getBmp(void) { return &disp; };
    GtkWidget*                      getDw(void) { return dw; };

private:
    static Simulator*               obj;
    bool                            fullScreenMode;
    GtkWidget*                      dw;
    SimulatorPlatform::HMI_BitMapDump disp;                       
};

#endif /* __SIMULATOR_H__ */
