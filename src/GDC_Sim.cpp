
#include "GDC_Sim.h"

static Simulator *_sim;

void setSim(Simulator *sim)
{
    _sim = sim;
}

Simulator *getSim() 
{
    return _sim;
}

unsigned int VramAddr(unsigned int x, unsigned y) 
{
    return ( x | (y << 9) );
}
