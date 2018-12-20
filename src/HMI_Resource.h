#ifndef __HMI_RESOURCE_H__
#define __HMI_RESOURCE_H__

/* CPU之外RAM的使用方法
0x80700000+-------------------------------------------------+
          | Heap                             3072KB         |
          |                                                 |
          | Trend (Log + Disp) Data      MAX 2048KB         |
          |                                                 |
          | 其它                          MAX 4096KB        |
0x80FFFFFF| (Font信息，图像信息，文字信息)                     |
0x81000000+-------------------------------------------------+
          | Stack                                           |
0x810FFFFF| 1024KB                                          |
          +-------------------------------------------------+
*/

#define INIT_MEM_SIZE           (3*1024*1024)   // heap size
#define SC_BIN_HEAD_SIZE        (7*4)
#define SC_PALETTE_DATA_SIZE    (2048)      // 256色*4(makeRegistColor command数)*2Palette

enum {
    OFFSET_PICTURE,
    OFFSET_PICTURE_CMD,
    OFFSET_STRING_CMD,
    OFFSET_STRING,
    OFFSET_IMAGE,
    OFFSET_FONT,
    OFFSET_PALETTE,
};

unsigned long*          sc_getHmiHeap(unsigned long* size);
unsigned int*           sc_getBinOffset(void);

#endif /* __HMI_RESOURCE_H__ */