#include "sclUtil.h"
#include "HMI_Resource.h"
#include "HmiMain.h"

#define HEAP_SIZE       INIT_MEM_SIZE

static unsigned long    heap[ HEAP_SIZE / sizeof( unsigned long ) ];  // 3MB
static unsigned long    heap_tmp[ HEAP_SIZE / sizeof( unsigned long ) ];  // 3MB

// Heap, Trend以外は2MB上限とする
static unsigned int     sc_binary_head[SC_BIN_HEAD_SIZE/sizeof(unsigned int)];  // 28B
static SCHEAD           sc_head;  // 
static SCPICTINFO       sc_pict_info[SC_MAX_PICTURE_NUM];   // 1KB

static SCFONTINFO       sc_font_info[SC_MAX_FONT_NUM];      // 64KB
static int              sc_font_num;

static unsigned short   sc_string_tab[SC_MAX_STR_NUM][SC_MAX_STR_LENGTH];    // 380KB

static short            sc_pre_screen_id[HMI_SCREEN_END - HMI_SCREEN_START] = {-1};
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
 *      得到临时的模拟的heap的大小，该内存块处理大文件（比如字体文件等使用）
 *
 * @params
 *          size ，heap大小
 * @return
 *          返回heap的指针
 */
unsigned long* sc_getHmiHeapTemp(unsigned long* size)
{
    if(size != 0)
    {
        *size = HEAP_SIZE;
    }

    return (heap_tmp);
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

/*
 *      获取字体信息
 *
 * @params
 *          void
 * @return
 *          SCHEAD： 字体信息
 */
SCHEAD* sc_getHead(void)
{
    return(&sc_head);
}

/*
 *      获取已经注册的字体数量
 *
 * @params
 *          void
 * @return
 *          int： 已经注册的字体数量
 */
int sc_getFontNum(void)
{
    return(sc_font_num);
}

/*
 *      记录已经注册的字体数量
 *
 * @params
 *          font_num： 字体数量
 * @return
 *          void
 */
void sc_setFontNum(int font_num)
{
    sc_font_num = font_num;
}

/*
 *      返回字体数据
 *
 * @params
 *          void
 * @return
 *          指向字体数据的指针
 */
SCFONTINFO* sc_getFontInfo(void)
{
    return(sc_font_info);
}

/*
 *      返回字体类型tab表
 *
 * @params
 *          void
 * @return
 *          指向字体Tab表的指针
 */
SCFONTTAB sc_getFontTab(void)
{
    return(sc_font_tab);
}

unsigned short  sc_getPictWidth(int pict_id)
{
    return(sc_pict_info[pict_id].width);
}

void sc_getFixedString(int target_ID, unsigned short* str)
{
    SCUTF16_StrCpy(str, sc_string_tab[target_ID]);
}

/****************************************************************************/
/**
 * @brief           sc_getPreScreenID()
 *                  返回当前画面的前一个画面
 *
 * @param[in]       sID:            当前画面
 *
 * @return          void
 */
/****************************************************************************/
short sc_getPreScreenID(short sID)
{
    short preScreenID = -1;
    if((sID >= HMI_SCREEN_START) && (sID < HMI_SCREEN_END)) 
    {
        short screenNo = sID - HMI_SCREEN_START;
        preScreenID = sc_pre_screen_id[screenNo];
    }
    return (preScreenID);
}

/****************************************************************************/
/**
 * @brief           sc_setPreScreenID()
 *                  当前画面的前一个画面
 *
 * @param[in]       sID:            当前画面
 *                  preScreenID:    当前画面的前一个画面
 *
 * @return          void
 */
/****************************************************************************/
void sc_setPreScreenID(short sID, short preScreenID)
{
    if( (preScreenID >= HMI_SCREEN_START) &&
        (preScreenID < HMI_SCREEN_END) &&
        (sID >= HMI_SCREEN_START) && 
        (sID < HMI_SCREEN_END) ) 
    {
        short screenNo = sID - HMI_SCREEN_START;
        sc_pre_screen_id[screenNo] = preScreenID;
    }
}