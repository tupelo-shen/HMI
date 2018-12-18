#include <stdlib.h>
#include "GDC_Driver.h"

#define RTE_FPGA_GDC_TPIF_LONGTAP_DETECTION_TIME 1000

pf_touch_event_func_t touchpanel_callback;
pf_touch_config_t touchpanel_config;

/*
 *      设置触摸板配置数据
 *
 * @params
 *          func，   event发生时，调用的回调函数
 *          config， 指向event配置数据结构体的指针
 * @return
 *          如果回调函数没有被指定，则返回-1；
 *          成功，返回0；
 */
int GDC_DRV_SetTouchPanelConfigData(pf_touch_event_func_t func, const pf_touch_config_t* config)
{
    pf_touch_config_t temp_config = *config;

    if (temp_config.longtap_detection_time == 0) 
    {
        temp_config.longtap_detection_time = RTE_FPGA_GDC_TPIF_LONGTAP_DETECTION_TIME;
    }

    if (temp_config.width_resolution == 0 || temp_config.height_resolution == 0) 
    {
        temp_config.width_resolution = 320;
        temp_config.height_resolution = 240;
    }

    if (func == NULL) 
    {
        return -1;
    }

    touchpanel_callback = func;
    touchpanel_config = temp_config;

    return 0;
}