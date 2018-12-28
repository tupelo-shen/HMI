#include "sclUtil.h"
#include "HMI_Resource.h"

#define HEAP_SIZE       INIT_MEM_SIZE

static unsigned long    heap[ HEAP_SIZE / sizeof( unsigned long ) ];  // 3MB

// Heap, Trend以外は2MB上限とする
static unsigned int     sc_binary_head[SC_BIN_HEAD_SIZE/sizeof(unsigned int)];  // 28B
static SCPICTINFO       sc_pict_info[SC_MAX_PICTURE_NUM];   // 1KB

static SCFONTINFO       sc_font_info[SC_MAX_FONT_NUM];      // 64KB
static int              sc_font_num;

static unsigned short   sc_string_tab[SC_MAX_STR_NUM][SC_MAX_STR_LENGTH];    // 380KB

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

int sc_getFontNum()
{
    return(sc_font_num);
}

SCFONTINFO* sc_getFontInfo()
{
    return(sc_font_info);
}

unsigned short  sc_getPictWidth(int pict_id)
{
    return(sc_pict_info[pict_id].width);
}

void sc_getFixedString(int target_ID, unsigned short* str)
{
    SCUTF16_StrCpy(str, sc_string_tab[target_ID]);
}