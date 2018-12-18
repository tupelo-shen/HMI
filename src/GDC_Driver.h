
#ifndef __GDC_DRIVER_H__
#define __GDC_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
enum
{
    PF_GDC_OK = 0,                   /**< ����I�� */
    PF_GDC_PARAMETER_ERROR = -1,     /**< �p�����[�^�G���[ */
    PF_GDC_UNINITIALIZED_ERROR = -2, /**< ���������G���[ */
    PF_GDC_VRAMADDRESS_ERROR = -3,   /**< VRAM�T�C�Y�𒴂����G���A���w�� */
    PF_GDC_TOUCHLOG_NONE = -4,       /**< �^�b�`���O�f�[�^�Ȃ� */
};
#endif 

typedef struct
{
  unsigned short x;
  unsigned short y;
} pf_point_t;

typedef enum
{
    pf_touch_event_touch = 0,
    pf_touch_event_release, 
    pf_touch_event_longtap,
    pf_touch_event_drag,       
    pf_touch_event_flick,       
    pf_touch_event_calibration_init,

    pf_touch_event_reserved = 0x7FFFFFFF
} pf_touch_event_t;

typedef enum
{
  pf_drag_flick_none = 0,  /**< 没有滑动事件 */
  pf_drag_flick_up = 1,    /**< 向上滑动 */
  pf_drag_flick_down,      /**< 向下滑动 */
  pf_drag_flick_right,     /**< 向右滑动 */
  pf_drag_flick_left,      /**< 向左滑动 */

  pf_drag_flick_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_drag_flick_direction_t;

typedef struct
{
  unsigned short fpga_sampling;               /**< FPGA�T���v�����O���� (ms) */
  unsigned short flick_sampling;              /**< �t���b�N���o���� (ms) */
  unsigned short longtap_detection_time;      /**< �����O�^�b�v���o���� (ms) */
  unsigned short touch_threshold;             /**< �^�b�`����p���W�������l */
  unsigned short drag_flick_threshold_level1; /**< �h���b�O/�t���b�N����p�̍��W�������l(level1) */
  unsigned short drag_flick_threshold_level2; /**< �h���b�O/�t���b�N����p�̍��W�������l(level2) */
  unsigned short drag_flick_threshold_level3; /**< �h���b�O/�t���b�N����p�̍��W�������l(level3) */
  unsigned short width_resolution;            /**< ������\ */
  unsigned short height_resolution;           /**< ��������\ */

} pf_touch_config_t;

typedef void (*pf_touch_event_func_t)(pf_touch_event_t, pf_point_t, unsigned int, pf_drag_flick_direction_t);


int GDC_DRV_SetTouchPanelConfigData(pf_touch_event_func_t func, const pf_touch_config_t* config);

#ifdef __cplusplus
}
#endif

#endif /* __GDC_DRIVER_H__ */
