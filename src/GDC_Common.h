#ifndef __GDC_COMMON_H__
#define __GDC_COMMON_H__

#include <stdint.h>
#include "GDC_Driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VRAM_SIZE                   (64 * 1024 * 1024)          /**> VRAM size */
#define MAX_PLANE_ID_COUNT          65535                       /**> 最大plane数 */
#define MAX_FONT_NO                 6                           /**> Font个数 */
#define MAX_BACKLIGHT_DUTY          100                         /**> backlight PWM DUTY max value*/
#define MAX_VRAMOFFSET_ADDRESS      0x03FFFFFF                  /**> 最大VRAM offset adress */
#define DRIVER_BUFFER_COUNT         2048                        /**> driver buffer */
#define FPGA_FIFO_COUNT             1024                        /**> FPGA FIFO数 */
#define MAX_TOUCHPANEL_SAMPLING     0x31                        /**> 最大采样周期 0x31 = 49 (从0开始，500ms)*/
#define MAX_SAMPLING_VALUE          0x3FF                       /**> 最大采样値 */

void                    PF_GDC_SetPlaneTable(const pf_plane_table_t* plane_table, uint16_t count);
const pf_plane_table_t* PF_GDC_GetPlaneTable(void);
uint16_t                PF_GDC_GetPlaneTableCount(void);

#ifdef __cplusplus
}
#endif

#endif
