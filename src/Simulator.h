/*
 * @Module Name：    Simulator.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-02-27
 */
#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <gtk/gtk.h>
#include <gtk/gtk.h>

class Simulator
{
public:
    Simulator(void);
    Simulator(Simulator& obj);
    virtual ~Simulator(void);

    static Simulator&               refObject(void);
    int                             gtkmain(int argc, char* argv[]);
    // void                            initBmp();
    // SimulatorPlatform::BitMapDump*  getBmp();
    GtkWidget*                      getDw(void) { return dw; };

private:
    static Simulator*   obj;
    bool                fullScreenMode;
    GtkWidget*          dw;
    // SimulatorPlatform::Liq4WC_BitMapDump disp;                       
};

#endif /* __SIMULATOR_H__ */
