#include "LiqHMIMain.h"
#include "HMIAlloc.h"


void* HMIAlloc::operator new(std::size_t size)
{
    return(LiqHMIMain::getHMIHeap()->alloc((unsigned long)size));
}

void HMIAlloc::operator delete(void* ptr)
{
    LiqHMIMain::getHMIHeap()->free(ptr);
}
