#include "HMI_Resource.h"

#define HEAP_SIZE       INIT_MEM_SIZE

static unsigned long    heap[ HEAP_SIZE / sizeof( unsigned long ) ];  // 3MB

// Heap, Trend以外は2MB上限とする
static unsigned int     sc_binary_head[SC_BIN_HEAD_SIZE/sizeof(unsigned int)];  // 28B
/*
 *      得到模拟的heap的大小
 *
 * @params
 *          size ，heap大小
 * @return
 *          返回heap的指针
 */
unsigned long* sc_getHmiHeap(unsigned long* size)
{
    if(size != 0)
    {
        *size = HEAP_SIZE;
    }

    return (heap);
}

/*
 *      得到bin文件的偏移量
 *
 * @params
 *          void
 * @return
 *          偏移量大小
 */
unsigned int* sc_getBinOffset(void)
{
    return(sc_binary_head);
}