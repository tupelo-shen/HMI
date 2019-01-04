#include "HmiMain.h"
#include "HMI_Alloc.h"


void* HMIAlloc::operator new(std::size_t size)
{
    // return(HmiMain::getHMIHeap()->alloc((unsigned long)size));
}

void HMIAlloc::operator delete(void* ptr)
{
    // HmiMain::getHMIHeap()->free(ptr);
}
