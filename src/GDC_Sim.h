#ifndef __GDC_SIM_H__
#define __GDC_SIM_H__

#include "Simulator.h"

unsigned int    VramAddr(unsigned int x, unsigned y);
void            setSim(Simulator *sim);
Simulator       *getSim();

#endif /* __GDC_SIM_H__ */
