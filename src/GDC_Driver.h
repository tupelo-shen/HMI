
#ifndef __GDC_DRIVER_H__
#define __GDC_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif


enum
{
    PF_GDC_OK = 0,                   /**< ����I�� */
    PF_GDC_PARAMETER_ERROR = -1,     /**< �p�����[�^�G���[ */
    PF_GDC_UNINITIALIZED_ERROR = -2, /**< ���������G���[ */
    PF_GDC_VRAMADDRESS_ERROR = -3,   /**< VRAM�T�C�Y�𒴂����G���A���w�� */
    PF_GDC_TOUCHLOG_NONE = -4,       /**< �^�b�`���O�f�[�^�Ȃ� */
};

/*
 * 表示坐标
 */
typedef struct
{
  unsigned short x;
  unsigned short y;
} pf_point_t;

/*
 * 绘图command
 */
#define  COM_MENU_EXECUTE_NO            0x01    /**< Com No.01: 执行绘图 */
#define  COM_SCR_FLIP_NO                0x02    /**< Com No.02: screen plate */
#define  COM_MENU_COMMON_SETUP_NO       0x03    /**< Com No.03: 菜单绘制常用设置(自动转发目的地/传输源数据格式设置) */
#define  COM_MENU_COLOR_BACK_NO         0x04    /**< Com No.04: 指定颜色(BACK) */
#define  COM_MENU_COLOR_FORE_NO         0x05    /**< Com No.05: 指定颜色(FORE) */
#define  COM_MENU_SOURCE_LOC_OFFSET2_NO 0x06    /**< Com No.06: 菜单绘图BitBlt传输源坐标设置偏移地址2 */
#define  COM_MENU_SOURCE_LOC_OFFSET1_NO 0x07    /**< Com No.07: 菜单绘图BitBlt传输源坐标设置偏移地址1 */
#define  COM_MENU_SOURCE_LOC_OFFSET0_NO 0x08    /**< Com No.08: 菜单绘图BitBlt传输源坐标设置偏移地址0 */
#define  COM_MENU_SOURCE_LOC_Y_NO       0x09    /**< Com No.09: 菜单绘图BitBlt传输源坐标设置传输源Y坐标 */
#define  COM_MENU_SOURCE_LOC_X_NO       0x0A    /**< Com No.0A: 菜单绘图BitBlt传输源坐标设置传输源X坐标 */
#define  COM_MENU_DEST_LOC_OFFSET2_NO   0x0B    /**< Com No.0B: 菜单绘图BitBlt传输目标坐标设置偏移地址2 */
#define  COM_MENU_DEST_LOC_OFFSET1_NO   0x0C    /**< Com No.0C: 菜单绘图BitBlt传输目标坐标设置偏移地址1 */
#define  COM_MENU_DEST_LOC_OFFSET0_NO   0x0D    /**< Com No.0D: 菜单绘图BitBlt传输目标坐标设置偏移地址0 */
#define  COM_MENU_DEST_LOC_Y_NO         0x0E    /**< Com No.0E: 菜单绘图BitBlt传输目标坐标设置-Y坐标 */
#define  COM_MENU_DEST_LOC_X_NO         0x0F    /**< Com No.0F: 菜单绘图BitBlt传输目标坐标设置-X坐标 */
#define  COM_MENU_TRNSF_AREA_WIDTH_NO   0x10    /**< Com No.10: 菜单绘图BitBlt传输数据区设置传输数据区（宽度）(width) */
#define  COM_MENU_TRNSF_AREA_HEIGHT_NO  0x11    /**< Com No.11: 菜单绘图BitBlt传输数据区设置传输数据区（高度）(height) */
#define  COM_GDC_INT_NO                 0x12    /**< Com No.12: 菜单绘图中断标志 */
#define  COM_FONT_TYPE_NO               0x13    /**< Com No.13: 指定字符类型 */
#define  COM_FONT_CHARACTER_CODE_M_NO   0x14    /**< Com No.14: 指定文字CODE(高bit位) */
#define  COM_FONT_CHARACTER_CODE_L_NO   0x15    /**< Com No.15: 指定文字CODE(低bit位) */
#define  COM_MENU_COLOR_PAL_ADDR        0x16    /**< Com No.16: 8位调色板地址 */
#define  COM_MENU_COLOR_PAL_R           0x17    /**< Com No.17: 8位调色板数据(Red) */
#define  COM_MENU_COLOR_PAL_G           0x18    /**< Com No.18: 8位调色板数据(Green) */
#define  COM_MENU_COLOR_PAL_B           0x19    /**< Com No.19: 8位调色板数据(Blue) */
/*
 * plane table
 */
typedef struct {
  unsigned int start; /**< start adress */
  unsigned int size;  /**< table size */
} pf_plane_table_t;

/*
 * 字体大小
 */
typedef struct
{
  unsigned short width;   /**> 宽度 */
  unsigned short height;  /**> 高度 */
} pf_font_size_t;

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

int             PF_GDC_DRV_FlipPaletteAndScreen(unsigned int, unsigned int);
int             PF_GDC_DRV_FlipPalette(unsigned int);
int             PF_GDC_DRV_FlipScreen(unsigned int);
void            PF_GDC_DRV_GetPaletteAndScreen(unsigned int *, unsigned int *);
unsigned int    PF_GDC_DRV_GetPalette(void);
unsigned int    PF_GDC_DRV_GetScreen(void);

// 触摸板配置相关
int             PF_GDC_DRV_SetTouchPanelConfigData(pf_touch_event_func_t func, const pf_touch_config_t* config);

// 与描述命令相关
int             PF_GDC_DRV_SetMenuCommand(const unsigned short *, unsigned int);
int             PF_GDC_DRV_FlushMenuCommand(void);
int             PF_GDC_DRV_ClearMenuCommand(void);
int             PF_GDC_DRV_RegistPlaneTable(const pf_plane_table_t *, unsigned int);
void            PF_GDC_DRV_SetTransparentColor(unsigned char);
int             PF_GDC_DRV_RegistFont(
                    unsigned int font_id, 
                    unsigned int vram_offset, 
                    pf_font_size_t font_size, 
                    unsigned int width_info, 
                    unsigned int width_address);
int             PF_GDC_DRV_VramAccess(unsigned int offset, const void *dat, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* __GDC_DRIVER_H__ */
