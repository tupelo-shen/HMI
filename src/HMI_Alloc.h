#ifndef __HMIAlloc_h_
#define __HMIAlloc_h_

#include <new>

class HMIAlloc {
public:
    HMIAlloc() {}

    static void*    operator new(std::size_t size);
    static void     operator delete(void* ptr);
};


#endif