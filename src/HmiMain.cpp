///
/// file        HmiMain.cpp
/// brief       HMI main process file
///
/// author      Wang.Yu
/// version     00.01.00
/// date        2018/11/26
///
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include "Queue-pthread_s.h"
#include "HmiMain.h"
#include "HmiEvent.h"
#include "HMIControl.h"
#include "HMI_Resource.h"
#include "sclColor.h"
#include "sclDrawCom.h"

#include "test_Simulator.h"

// using namespace std;
// using namespace boost;

#define VRAM_REGIST_SIZE                (4*1024)
#define VRAM_EXPORT_SIZE                ((1024+640)*1024)       // 1.6MB（将字体和图像数据展开到VRAM时，一次写入的上限值）

extern "C" void SendEvent2HMI(
    pf_touch_event_t            ev,
    pf_point_t                  pnt,
    unsigned int                level,
    pf_drag_flick_direction_t   direc
)
{
    HmiEvent ev_info(HmiEvent::HMI_EV_MOUSE_PRESSED, (unsigned long)pnt.x);
    HMIMAIN->SetReady(true);
    HMIMAIN->AddEventQueue(ev_info);
}
/// function    HmiMain
/// brief       constructor
HmiMain::HmiMain() : is_ready(false)
{
}

/// function    ~HmiMain
/// brief       deconstructor
HmiMain::~HmiMain()
{
}
#include "sclBoard.h"
#include "sclParts.h"
#include "sclLabel.h"

void HmiMain::start(void)
{
    int sts;
    unsigned long*  buf;
    unsigned char*  regBuf;
    unsigned long   size;

    buf = sc_getHmiHeap(&size);

    // VRAM处理
    regBuf = (unsigned char *)buf;      // heap用作扩展处理的缓冲区

    sts = registFont(0, regBuf);

    // 注册调色板1、2
    sts = registPalette();

#if 0   /* for testing simulator */
    test_main();
#endif
#if 0
    test_draw();
#endif

    const SCRect m_rect = {100, 100, 100, 100};
    unsigned short* com_buf;
    SCBoard* board = new SCBoard(m_rect, 1);

    const SCRect lbl_rect = {110, 110, 60, 30};
    SCLabel* label = new SCLabel(0, lbl_rect);
    label->arrangement(SC_ARRANGE_LEFT);
    label->setStr("abc");
    board->RegistParts(label);

    int com_num = board->Draw(com_buf);

    if(com_num > 0)
    {
        board->Display(com_num, com_buf);
    }

    main();
}

/****************************************************************************/
/**
 * @brief           registFont()
 *                  注册字体信息及字体数据
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   
 *
 * @return  int     填充的描画命令数
 */
/****************************************************************************/
int HmiMain::registFont(unsigned short lang_id, unsigned char* buf)
{
    int                 sts;
    int                 i, j;
    SCVRAM              vram_adr;
    SCHEAD*             head;
    pf_font_size_t      size;               // 注册到驱动中的字体大小信息
    UNTRANSFER          transfer;
    SCFONTTAB           font_tab;
    SCFONTINFO*         font_info;
    unsigned int        fontIndex;
    unsigned int        headSize;
    unsigned int        bitmapSize;
    unsigned int        dataSize;
    unsigned int        vramSize;
    int                 registFontNum = 0;
    int                 vramCnt;

    // 获取HMI类使用的字体信息保存位置
    font_tab    = sc_getFontTab();
    font_info   = sc_getFontInfo();
    head        = sc_getHead(); 

    // 清空注册的字体数量
     sc_setFontNum(registFontNum);
    
    // 在VRAM的位置
    vram_adr.ADRS.DWORD = 0;
    vram_adr.ADRS.BIT.Y = SC_VRAM_FONT;

    // 最多有6种字体
    for (i = 0; i < SC_MAX_FONT_NUM; i++)
    {
        // Font的有效性检查
        fontIndex = font_tab[lang_id][i];
        if (fontIndex == IDF_FILE_NONE)
            continue;

        unsigned long fsize;
        loadFile("fonts/DXFont8x16.bdf", buf, &fsize);

        // 解析Head结构体信息
        head->FONT.WIDTH    = *(unsigned char *)buf;
        head->FONT.HEIGHT   = *(unsigned char *)(buf+1);
        head->FONT.COUNT    = *(unsigned short*)(buf+2);
        memcpy((char *)(head->FONT.CHAR), (char *)(buf+4), 4*(head->FONT.COUNT));

        printf("head->FONT.WIDTH = %d; head->FONT.HEIGHT = %d, head->FONT.COUNT = %d\n", 
                head->FONT.WIDTH, head->FONT.HEIGHT, head->FONT.COUNT);
        // 算出字体大小
        headSize = 4 + 4*head->FONT.COUNT;
        bitmapSize = ((head->FONT.WIDTH%8) ? (head->FONT.WIDTH/8 + 1) :
                (head->FONT.WIDTH/8)) * head->FONT.HEIGHT;
        printf("headSize = %d;bitmapSize = %d\n", headSize, bitmapSize);

        // 
        dataSize = bitmapSize * head->FONT.COUNT;
        printf("dataSize = %d\n", dataSize);
        memcpy((char*)buf, (char*)(buf+headSize), dataSize);
        memset((buf+dataSize), 0x00, INIT_MEM_SIZE-dataSize);

        // Bitmap展开到缓冲区中
        for (j = head->FONT.COUNT-1; j>=0; j--)
        {
            // head->FONT.CHAR[j].CODE 存储字体中单个字符的编码（unsigned short型）
            // 将buf中存储的字符数据，按照编码大小，存到编码对应的位置上
            printf("head->FONT.CHAR[%d].WIDTH = %d; head->FONT.CHAR[%d].CODE = %d\n", j, head->FONT.CHAR[j].WIDTH,  j, head->FONT.CHAR[j].CODE);
            font_info[i].width[head->FONT.CHAR[j].CODE] = (unsigned char)(head->FONT.CHAR[j].WIDTH);
            memcpy((char*)(buf + bitmapSize * (head->FONT.CHAR[j].CODE)), 
                    (char*)(buf + bitmapSize*j), bitmapSize);
            memset((buf + bitmapSize*j), 0x00, bitmapSize);
        }

        // 将一种字体文件展开到VRAM中
        vramSize = bitmapSize * (head->FONT.CHAR[head->FONT.COUNT-1].CODE+1);
        if (vramSize%2 != 0) vramSize++;
        vramCnt = vramSize / VRAM_EXPORT_SIZE;
        vramCnt = (vramSize % VRAM_EXPORT_SIZE) ? vramCnt+1 : vramCnt;
        for (j=0; j<vramCnt; j++)
        {
            int cpy_size = (vramCnt-1 == j) ? (vramSize-j*VRAM_EXPORT_SIZE) :
                    VRAM_EXPORT_SIZE;
            sts = VramAccessBlock(vram_adr.ADRS.DWORD + j*VRAM_EXPORT_SIZE,
                    buf + j*VRAM_EXPORT_SIZE, cpy_size);
        }
        if(sts != 0) return(sts);

        // VRAM写入成功时
        font_info[i].f_width  = head->FONT.WIDTH;
        font_info[i].f_height = head->FONT.HEIGHT;
        printf("font_info[i].f_width = %d; font_info[i].f_height = %d\n", 
                font_info[i].f_width, font_info[i].f_height);
        size.width  = ((head->FONT.WIDTH%8)?(head->FONT.WIDTH/8+1):
                (head->FONT.WIDTH/8))*8;
        size.height = head->FONT.HEIGHT;

        // 往驱动中注册字体信息
        transfer.pointer = font_info[i].width;
        sts = PF_GDC_DRV_RegistFont(i, vram_adr.ADRS.DWORD, size,
                FONT_PROPORTIONAL, transfer.value);
        if(sts != 0) return(sts);
        printf("transfer.value = %d\n", transfer.value);
        // 记录注册字体数量
        sc_setFontNum(++registFontNum);

        // 下一种字体的VRAM地址
        vram_adr.ADRS.DWORD += (SC_MAX_CHAR_NUM*bitmapSize);
    }

    return sts;
}

/****************************************************************************/
/**
 * @brief           registPalette()
 *                  注册调色板
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   void
 *
 * @return  int     0: 成功
 */
/****************************************************************************/
int HmiMain::registPalette(void)
{
    // int sts;
    int palette_type, color_no, i;
    // unsigned int* offset_array;
    // unsigned int offset;
    unsigned short command_buf[2048];   // 256色*4(makeRegistColorコマンド数)*2パレット
    int command_num = 0;
    unsigned char rgb[4];
    // unsigned char* ptr;

    // 调色板数据偏移量的获取
    // offset_array = sc_getBinOffset();
    // offset = offset_array[OFFSET_PALETTE];

    // // 从Flash中读取调色板数据
    // sts = readFlashBlock(offset, buf, SC_PALETTE_DATA_SIZE);
    // if(sts != 0)
    //     return(sts);

    // ptr = buf;
    // for(palette_type = 0; palette_type < 2; palette_type++) {
    //     for(color_no = 0; color_no < SC_MAX_COLOR_NO; color_no++) {
    //         for(i = 0; i < SC_PALETTE_PRIMARY_COLOR_NUM; i++) {
    //             rgb[i] = *(ptr++);
    //         }
    //         command_num += SCDrawCom::makeCom16171819(palette_type, color_no, 
    //              rgb[0], rgb[1], rgb[2], &command_buf[command_num]);
    //     }
    // }

    // 注册调色板
    for(palette_type = 0; palette_type < 2; palette_type++) 
    {
        if (palette_type == 0)
        {
            for(color_no = 0; color_no < SC_MAX_COLOR_NO; color_no++) 
            {
                for(i = 0; i < SC_PALETTE_PRIMARY_COLOR_NUM; i++) 
                {
                    rgb[i] = g_palette_buf_1[color_no][i];
                }
                command_num += SCDrawCom::makeCom16171819(palette_type, color_no, 
                        rgb[0], rgb[1], rgb[2], &command_buf[command_num]);
            }            
        }
        else
        {
            for(color_no = 0; color_no < SC_MAX_COLOR_NO; color_no++) 
            {
                for(i = 0; i < SC_PALETTE_PRIMARY_COLOR_NUM; i++) 
                {
                    rgb[i] = g_palette_buf_2[color_no][i];
                }
                command_num += SCDrawCom::makeCom16171819(palette_type, color_no, 
                        rgb[0], rgb[1], rgb[2], &command_buf[command_num]);
            }            
        }
    }
    // 把描画命令拷贝到缓冲区中
    PF_GDC_DRV_SetMenuCommand(command_buf, command_num);

    // Flush描画命令的缓冲区
    PF_GDC_DRV_FlushMenuCommand();

    return 0;
}

/****************************************************************************/
/**
 * @brief           VramAccessBlock()
 *                  注册调色板
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   void
 *
 * @return  int     0: 成功
 */
/****************************************************************************/

int HmiMain::VramAccessBlock(
    unsigned int addr,
    const void* data,
    unsigned int size  // 总是偶数、奇数时VramAccess驱动不动作
)
{
    int sts;
    unsigned int offset = addr;
    unsigned int dSize  = size;
    unsigned char* buf  = (unsigned char*)data;

    while (dSize > VRAM_REGIST_SIZE)
    {
        sts = PF_GDC_DRV_VramAccess(offset, (const void*)buf, VRAM_REGIST_SIZE);
        if(sts != 0) return(sts);
        dSize -= VRAM_REGIST_SIZE;
        offset += VRAM_REGIST_SIZE;
        buf += VRAM_REGIST_SIZE;
    }
    sts = PF_GDC_DRV_VramAccess(offset, (const void*)buf, dSize);
    if(sts != 0) return(sts);

    return(sts);
}

/****************************************************************************/
/**
 * @brief           main()
 *                  HMI的主程序，完成HMI相关主要逻辑的处理
 *
 * @param[in/out]   void
 *
 * @return          void
 */
/****************************************************************************/
void HmiMain::main(void)
{
    // 鼠标等输入设备的配置
    pf_touch_config_t           t_conf;

    t_conf.fpga_sampling = 0;
    t_conf.flick_sampling = 0;
    t_conf.longtap_detection_time = 0;
    t_conf.touch_threshold = 0;
    t_conf.drag_flick_threshold_level1 = 0;
    t_conf.drag_flick_threshold_level2 = 0;
    t_conf.drag_flick_threshold_level3 = 0;
    t_conf.width_resolution = 0;
    t_conf.height_resolution = 0;

    PF_GDC_DRV_SetTouchPanelConfigData(SendEvent2HMI, &t_conf);

    HmiEvent ev(HmiEvent::HMI_EV_NONE);
    
    while(true) 
    {
        // 事件处理
        while(true) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // 事件处理
            if(!IsEmpty() && GetEventQueue((HmiEvent&) ev))
            {
                eventHandler(ev);
            }
        }
    }
}

/****************************************************************************/
/**
 * @brief           eventHandler()
 *                  根据指定的事件，做出相应的处理
 *
 * @param[in]       ev:     待处理的事件
 *
 * @return          void
 */
/****************************************************************************/
void HmiMain::eventHandler(HmiEvent& ev)
{
    HmiEvent::HmiEventType type = HmiEvent::HMI_EV_NONE;
    unsigned long param;

    // get event type & param
    type = ev.GetEvent();
    switch(type) 
    {
    case HmiEvent::HMI_EV_KEYDOWN:
        param = ev.GetULArg();
        // std::cout << std::endl << "\"" << (char)param << "\" key is pushed down!" << std::endl;
        break;
    case HmiEvent::HMI_EV_MOUSE_PRESSED:
        param = ev.GetULArg();
        // std::cout << std::endl << "\"" << (unsigned long)param << "\" key is pushed down!" << std::endl;
    default: // HMI_EV_NONE or not defined
        break;
    }
}

/****************************************************************************/
/**
 * @brief           loadFile()
 *                  载入文件，并将其保存在缓冲区fdata中，数据大小为fsize
 *
 * @param[in/out]   fname： 文件名称
 *
 * @return          void
 */
/****************************************************************************/

void HmiMain::loadFile(const char * fname, unsigned char* buf, unsigned long *fsize)
{
    unsigned char*      fdata;
    // unsigned long       size;

    // buf_tmp = (unsigned char *)sc_getHmiHeapTemp(&size);
    
    const boost::filesystem::path path(fname);
    try
    {
        *fsize = file_size(path);
        fdata = new unsigned char [*fsize];
        std::ifstream fin(fname, std::ios::in | std::ios::binary);
        if(fin)
        {
            fin.read((char *)fdata, *fsize);
            *fsize = convertToBin((const char* )fdata, buf, *fsize);
            printf("fsize = %ld\n", *fsize);
        }
    }
    catch (boost::filesystem::filesystem_error & exp)
    {
        fsize   = 0;
        fdata   = 0;
        throw;
    }
}

/****************************************************************************/
/**
 * @brief           readLine()
 *                  从缓存区中读取一行数据，行结束符为“0x0D,0x0A”
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   buf 缓冲区
 *
 * @return          void
 */
/****************************************************************************/
// unsigned int        readLine(const char* src, char* dst);
unsigned int HmiMain::readLine(const char* src, char* dst) 
{
    unsigned int    line_char_num = 0;

    while(1)
    {
        if ((src != NULL) && (*src != 0x0D))
        {
            *dst++ = *src++;
            line_char_num++;
        }
        else
        {
            if (*(++src) == 0x0A)       // find line end
            {
                *dst = '\0';
                return line_char_num;
            }
            else
            {
                *dst++ = *src++;
                line_char_num++;
            }
        }
    }
}
#include <cstring>
/****************************************************************************/
/**
 * @brief           convertToBin()
 *                  
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   buf 缓冲区
 *
 * @return          void
 */
/****************************************************************************/
 // char *strtok(char *s, const char *delim);
#include <cstdlib>
#define SINGLE_CHAR_DATA_BYTES      (32 * 32)               // 目前只能支持最大32x32的字体

// char            line[1024]      = {0};
// unsigned char   char_data[SINGLE_CHAR_DATA_BYTES]       = {0}; // 目前只能支持最大32x32的字体
// unsigned char   char_data_1[SINGLE_CHAR_DATA_BYTES]     = {0}; // 目前只能支持最大32x32的字体

unsigned long HmiMain::convertToBin(const char* src, unsigned char* dst,  unsigned long size)
{
    int             len, i, j, k, m;
    unsigned int    l_cnt           = 0;
    unsigned int    char_height_pix = 0;
    unsigned long   cur_pos         = 0;
    unsigned long   count           = 0;                    // 记录填充到dst中的字节数
    unsigned short  font_chars_num  = 0;
    unsigned short  char_width      = 0;
    unsigned short  char_height     = 0;
    int             char_x_offset   = 0;
    int             char_y_offset   = 0;
    unsigned short  char_data_bytes = 0;
    unsigned char   char_width_max  = 0;
    unsigned char   char_height_max = 0;
    int             char_x_offset_max   = 0;
    int             char_y_offset_max   = 0;
    unsigned char   char_data_start = 0;
    unsigned char   fill_data_start = 0;
    char            line[1024]      = {0};
    unsigned char   char_data[SINGLE_CHAR_DATA_BYTES]       = {0}; // 目前只能支持最大32x32的字体
    unsigned char   char_data_1[SINGLE_CHAR_DATA_BYTES]     = {0}; // 目前只能支持最大32x32的字体
    char            start_font[]    = "STARTFONT";
    char            font_max_area[] = "FONTBOUNDINGBOX";
    char            font_chars[]    = "CHARS";
    char            font_coding[]   = "ENCODING";
    char            font_bbx[]      = "BBX";
    char            font_bitmap[]   = "BITMAP";
    char            font_endchar[]  = "ENDCHAR";
    const char      char_space      = ' ';
    char*           ptr             = NULL;
    unsigned char*  font_head_ptr   = dst;
    unsigned char*  font_data_ptr   = dst;
    unsigned short* tmp_ptr         = NULL;

    for( i = 0; *src != '\0'; i++)
    {
        if (cur_pos >= size)
            break;
        l_cnt = readLine(src+cur_pos, line);    // readLine(const char* src, unsigned char* dst)
        cur_pos += l_cnt + 2;                   // 2 是行结束符所占字节数

        // 取每一行的首个字符串
        ptr = strtok(line, &char_space);
        // printf("ptr = %s\n", ptr);

        // 查找 "FONTBOUNDINGBOX"
        if (strcmp(ptr, font_max_area) == 0)
        {            
            ptr = strtok(NULL, &char_space);     // get width
            len = strlen(ptr);
            printf("width = %s\n", ptr);
            if (len)
            {
                char_width_max = static_cast<unsigned char>(atoi(ptr++));        // str converts to integer
                *font_head_ptr++ = char_width_max;                 
            }
            ptr = strtok(NULL, &char_space);     // get height
            len = strlen(ptr);
            printf("height = %s\n", ptr);
            if (len)
            {
                char_height_max = static_cast<unsigned char>(atoi(ptr++));        // str converts to integer
                *font_head_ptr++ = char_height_max;
                printf("height = %d\n", (unsigned int)*(dst+1));
            }
            ptr = strtok(NULL, &char_space);     // get x_offset
            len = strlen(ptr);
            printf("x_offset = %s\n", ptr);
            if (len)
            {
                char_x_offset_max = atoi(ptr++);        // str converts to integer
                // *font_head_ptr++ = char_x_offset_max;
            }
            ptr = strtok(NULL, &char_space);     // get y_offset
            len = strlen(ptr);
            printf("y_offset = %s\n", ptr);
            if (len)
            {
                char_y_offset_max = atoi(ptr++);        // str converts to integer
                // *font_head_ptr++ = char_y_offset_max;
            }
        }

        // 查找 "CHARS"
        if (strcmp(ptr, font_chars) == 0)
        {
            ptr = strtok(NULL, &char_space);     // delete "CHARS"
            printf("chars = %s\n", ptr);
            font_chars_num = static_cast<unsigned short>(atoi(ptr++));
            tmp_ptr = reinterpret_cast<unsigned short*>(font_head_ptr);
            *tmp_ptr = font_chars_num;          // record chars
            font_head_ptr += 2;                 // a short number is devided into two bytes
            font_data_ptr += 4 + 4 * font_chars_num; // CODE: short; WIDTH: short
            count = 4 + 4 * font_chars_num;

        }

        // 查找 "ENCODING"
        if (strcmp(ptr, font_coding) == 0)
        {
            ptr = strtok(NULL, &char_space);     // delete "ENCODING"
            printf("ENCODING = %s\n", ptr);
            unsigned short font_encoding = static_cast<unsigned short>(atoi(ptr++));
            tmp_ptr = reinterpret_cast<unsigned short*>(font_head_ptr);
            *tmp_ptr = font_encoding;         
            font_head_ptr += 2;
        }

        // 查找 "BBX"
        if (strcmp(ptr, font_bbx) == 0) 
        {
            ptr = strtok(NULL, &char_space);     // get "char_width"
            printf("char_width = %s\n", ptr);
            char_width = static_cast<unsigned short>(atoi(ptr++));
            ptr = strtok(NULL, &char_space);     // get "char_height"
            printf("char_height = %s\n", ptr);
            char_height = static_cast<unsigned short>(atoi(ptr++));
            tmp_ptr = reinterpret_cast<unsigned short*>(font_head_ptr);
            *tmp_ptr = char_width;              
            font_head_ptr += 2;
            ptr = strtok(NULL, &char_space);     // get "char_x_offset"
            printf("char_x_offset = %s\n", ptr);
            char_x_offset = atoi(ptr++);
            ptr = strtok(NULL, &char_space);     // get "char_y_offset"
            printf("char_y_offset = %s\n", ptr);
            char_y_offset = atoi(ptr++);
        }

        // 查找 "BITMAP"
        if (strcmp(ptr, font_bitmap) == 0) 
        {
            char_data_start = 1;                // 开始读取单个字符数据
            memset(char_data, 0x00, SINGLE_CHAR_DATA_BYTES);
            k = 0;
            j = 0;
            // 计算文件中该字符的数据个数
            char_data_bytes = (char_width % 8) ? (char_width/8 + 1) : (char_width/8);
            printf("char_data_bytes = %d\n", char_data_bytes);
            continue;                           // 重新读取一行
        }

        // 查找 "ENDCHAR"
        if (strcmp(ptr, font_endchar) == 0) 
        {
            char_data_start = 0;                // 结束读取单个字符数据
            fill_data_start = 1;
            printf("end *****\n");
        }

        // 读取bitmap数据
        if (char_data_start)
        {
            for (m = 0; m < char_data_bytes; m++)
            {
                char str[3] = {0};
                printf("ptr = %s\n", ptr);
                if (get2CharsFromStr(ptr, str))
                {
                    // printf("str = %s\n", str);
                    char_data_1[j++] = static_cast<unsigned char>(atoi(str));
                }
            }
        }

        // 将bitmap数据填充到指定缓冲区
        if (fill_data_start)
        {
            for(j=j-1; j!=0; j--)
            {
                unsigned int index = (k++)+char_data_bytes*(char_y_offset - char_y_offset_max);
                char_data[index] = char_data_1[j];
            }
            memcpy(font_data_ptr, char_data, char_data_bytes * char_height);
            count += char_data_bytes * char_height_max;
            fill_data_start = 0; 
        }
    }
    
    printf("count = %d\n", count);
    return count;
}
/****************************************************************************/
/**
 * @brief           get2CharsFromStr
 *                  从一个指定字符串中取前两个字符，奇数时，只取一个字符
 *
 * @author          Tupelo
 * @date            2018-12-27
 *
 * @param[in/out]   src: 指定字符串
 *
 * @return          dst: 目标字符串
 */
/****************************************************************************/
unsigned int HmiMain::get2CharsFromStr(const char* src, char* dst)
{
    unsigned int    i = 0;

    for (i = 0; (*src != '\0') && (i < 2); i++)
    {
        *dst++ = *src++;
    }

    *dst = '\0';
    return i;
}
