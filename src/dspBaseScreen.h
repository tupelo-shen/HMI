#ifndef __DSP_BASE_SCREEN_H__
#define __DSP_BASE_SCREEN_H__


#include "sclBoard.h"
#include "sclParts.h"

class BaseScreen : public SCBoard
{
    M_DECLARE_RUNTIME(BaseScreen)

public:
    BaseScreen(SCRect& area, const short sID);
    virtual ~BaseScreen();

    virtual void    initBaseScreen(void);
    virtual void    createPrevBtn(void);
    virtual void    createNextBtn(void);
    virtual void    createTitleLabel(void);

    /* 部品ID */
    enum {
        BASE_PARTS_ID_TITLE_LBL_PICT = 0,
        BASE_PARTS_ID_TITLE_LBL,
        BASE_PARTS_ID_PREV_BTN_PICT,
        BASE_PARTS_ID_PREV_BTN,
        BASE_PARTS_ID_NEXT_BTN_PICT,
        BASE_PARTS_ID_NEXT_BTN,
        BASE_PARTS_ID_ALARMREP,
        BASE_PARTS_ID_ALARMREP_BTN,
        BASE_PARTS_ID_HOME_PICT,
        BASE_PARTS_ID_HOME_BTN,
        BASE_PARTS_ID_NUM
    };

    /**
     * @brief   Dialog type
     */
    enum
    {
        BASE_DLG_TYPE_CONFIG_SAVE_FAILED,   //!< Failed.
        BASE_DLG_TYPE_DATABASE_BUSY,        //!< Database busy.
        BASE_DLG_TYPE_COMMUNICATION_ERROR,  //!< Communication error.
        BASE_DLG_TYPE_RUNNING_WAIT,         //!< It is running. Please wait.
        // -----

        BASE_DLG_TYPE_MAX,
    };

    // baseScreen title
    static const SCRect title_lbl_pict_pos;
    static const SCRect title_lbl_pos;

    // prev button display area
    static const SCRect prev_btn_pict_pos;
    static const SCRect prev_btn_pos;

    // next button display area
    static const SCRect next_btn_pict_pos;
    static const SCRect next_btn_pos; 
};

#endif /* __DSP_BASE_SCREEN_H__ */
