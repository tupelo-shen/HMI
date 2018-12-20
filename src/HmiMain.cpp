///
/// file        HmiMain.cpp
/// brief       HMI main process file
///
/// author      Wang.Yu
/// version     00.01.00
/// date        2018/11/26
///
#include <thread>
#include "Queue-pthread_s.h"
#include "HmiMain.h"
#include "HmiEvent.h"
#include "HMIControl.h"
#include "HMI_Resource.h"
#include "test_Simulator.h"

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

void HmiMain::start(void)
{
    int sts;
    unsigned long*  buf;
    unsigned char*  regBuf;
    unsigned long   size;

    buf = sc_getHmiHeap(&size);

    // VRAM处理
    regBuf = (unsigned char *)buf;      // heap用作扩展处理的缓冲区

    sts = registPalette(regBuf);

#if 1   /* for testing simulator */
    test_main();
#endif
    main();
}

/*
 *      得到bin文件的偏移量
 *
 * @params
 *          void
 * @return
 *          偏移量大小
 */
int HmiMain::registPalette(unsigned char* buf)
{
    int sts;
    int palette_type, color_no, i;
    unsigned int* offset_array;
    unsigned int offset;
    unsigned short command_buf[2048];   // 256色*4(makeRegistColorコマンド数)*2パレット
    int command_num = 0;
    unsigned char rgb[4];
    unsigned char* ptr;

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
    //         command_num += SCDrawCom::makeCom16171819(palette_type, color_no, rgb[0], rgb[1], rgb[2], &command_buf[command_num]);
    //     }
    // }

    // 把描画命令拷贝到缓冲区中
    PF_GDC_DRV_SetMenuCommand(command_buf, command_num);

    // Flush描画命令的缓冲区
    PF_GDC_DRV_FlushMenuCommand();

    return(sts);
}
/// function    main
/// brief       HMI main process function
///
/// param       none
/// return      none
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

/// function    eventHandler
/// brief       HMI main event process function
///
/// param       ev  : event popped from queue
/// return      none
void HmiMain::eventHandler(HmiEvent& ev)
{
    HmiEvent::HmiEventType type = HmiEvent::HMI_EV_NONE;
    unsigned long param;

    // get event type & param
    type = ev.GetEvent();
    switch(type) {
    case HmiEvent::HMI_EV_KEYDOWN:
        param = ev.GetULArg();
        std::cout << std::endl << "\"" << (char)param << "\" key is pushed down!" << std::endl;
        // std::cout << std::endl << "\"" << param << "\" key is pushed down!" << std::endl;
        break;
    case HmiEvent::HMI_EV_MOUSE_PRESSED:
        param = ev.GetULArg();
        std::cout << std::endl << "\"" << (unsigned long)param << "\" key is pushed down!" << std::endl;
    default: // HMI_EV_NONE or not defined
        break;
    }
}
