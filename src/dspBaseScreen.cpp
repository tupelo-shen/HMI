#include "dspBaseScreen.h"
#include "sclLabel.h"
#include "sclButton.h"
#include "HmiMain.h"

// 部品位置信息
const SCRect BaseScreen::title_lbl_pict_pos(6+5, 6+5, 24, 24);         // ICON位置
const SCRect BaseScreen::title_lbl_pos     (3+5, 3+5, 60, 24);         // 按下响应的区域

const SCRect BaseScreen::prev_btn_pict_pos (36, 203, 24, 24);         // ICON位置
const SCRect BaseScreen::prev_btn_pos      (33, 200, 60, 24);         // 按下响应的区域

const SCRect BaseScreen::next_btn_pict_pos (230, 203, 24, 24);     // ICON位置
const SCRect BaseScreen::next_btn_pos      (227, 200,  60, 24);     // 按下响应的区域

/* 回调函数*/
static void cBaseChangeScreen(int parts_id);

M_IMPLEMENT_RUNTIME(BaseScreen, SCUpdateTypeNONE, SCBoard)
// M_IMPLEMENT_RUNTIME(BaseScreen, SCUpdateTypeCYCLIC, SCBoard)
/****************************************************************************/
/**
 * @brief           BaseScreen()
 *                  构造函数
 *
 * @param[in]       SCRect: 显示位置
 *                  sID:    画面ID
 *
 * @return          void
 */
/****************************************************************************/
BaseScreen::BaseScreen(
    SCRect& area,
    const short sID
):  SCBoard(area, sID)
{
    initBaseScreen();
}

/****************************************************************************/
/**
 * @brief           ~BaseScreen()
 *                  析构函数
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
BaseScreen::~BaseScreen(
)
{
}

/****************************************************************************/
/**
 * @brief           initBaseScreen()
 *                  画面的初始化
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
void BaseScreen::initBaseScreen(void)
{
    // Title
    createTitleLabel();

    // 创建Prev按钮
    createPrevBtn();

    // 创建Next按钮
    createNextBtn();
}

/****************************************************************************/
/**
 * @brief           createTitleLabel()
 *                  创建标题
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
void BaseScreen::createTitleLabel(void)
{
    SCRect      parts_area;
    SCLabel*    label;

    parts_area = BaseScreen::title_lbl_pos;
    label = new SCLabel(BASE_PARTS_ID_TITLE_LBL, parts_area);
    label->arrangement(SC_ARRANGE_LEFT);
    this->RegistParts(label);  
}
/****************************************************************************/
/**
 * @brief           createPrevBtn()
 *                  创建Prev按钮
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
void BaseScreen::createPrevBtn(void)
{
    SCRect      parts_area;
    SCParts*    back_btn_pict;  // Prev button icon
    SCParts*    back_btn;       // Prev button
    SCCallback* callback;

    // Prev button icon
    // parts_area = BaseScreen::prev_btn_pict_pos;
    // back_btn_pict = new SCPict(BASE_PARTS_ID_BACK_BTN_PICT, parts_area, IDB_ICON_PREV, COLOR_TRANSPARENT, COLOR_GRAY_EE, SC_UPDATE_WORK);
    // this->RegistParts(back_btn_pict);

    // Prev button
    parts_area = BaseScreen::prev_btn_pos;
    back_btn = new SCButton(BASE_PARTS_ID_PREV_BTN, 
                            parts_area, 
                            COLOR_TRANSPARENT/* COLOR_TRANSPARENT */, 
                            COLOR_TRANSPARENT, 
                            SC_UPDATE_WORK, 
                            SC_PARTS_FONT_TYPE_MFONT, 
                            false);
    // back_btn->setStr("P");
    this->RegistParts(back_btn);

    // 注册回调函数
    // Prev按钮
    callback = new SCCallback(SCCallbackTypeTAP, cBaseChangeScreen, BASE_PARTS_ID_PREV_BTN);
    back_btn->AddCallback(callback);   
}

/****************************************************************************/
/**
 * @brief           createNextBtn()
 *                  创建 Next 按钮
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
void BaseScreen::createNextBtn(void)
{
    SCRect      parts_area;
    SCParts*    back_btn_pict;  // Prev button icon
    SCParts*    back_btn;       // Prev button
    SCCallback* callback;

    // Prev button icon
    // parts_area = BaseScreen::prev_btn_pict_pos;
    // back_btn_pict = new SCPict(BASE_PARTS_ID_BACK_BTN_PICT, parts_area, IDB_ICON_PREV, COLOR_TRANSPARENT, COLOR_GRAY_EE, SC_UPDATE_WORK);
    // this->RegistParts(back_btn_pict);

    // Prev button
    parts_area = BaseScreen::next_btn_pos;
    back_btn = new SCButton(BASE_PARTS_ID_NEXT_BTN, 
                            parts_area, 
                            COLOR_TRANSPARENT, 
                            COLOR_TRANSPARENT, 
                            SC_UPDATE_WORK, 
                            SC_PARTS_FONT_TYPE_MFONT, 
                            false);
    // back_btn->setStr("P");
    this->RegistParts(back_btn);

    // 注册回调函数
    // Prev按钮
    callback = new SCCallback(SCCallbackTypeTAP, cBaseChangeScreen, BASE_PARTS_ID_NEXT_BTN);
    back_btn->AddCallback(callback);   
}


/****************************************************************************/
/**
 * @brief           createNextBtn()
 *                  创建 Next 按钮
 *
 * @param[in]       void
 *
 * @return          void
 */
/****************************************************************************/
static void cBaseChangeScreen(int parts_id)
{
    switch(parts_id) 
    {
    case BaseScreen::BASE_PARTS_ID_PREV_BTN:
        // BaseScreen* base_screen;
        // base_screen = static_cast<BaseScreen*>(LIQHMIMAIN->getActiveBoard());
        // LiqHMIEvChgScreenForNotUpdatingID(sc_getPreScreenID(base_screen->GetScreenID()));
        HMIEventChangeScreen(HMI_SCREEN_MAIN, 0, 0, 0, 0, 0);
        break;

    case BaseScreen::BASE_PARTS_ID_NEXT_BTN:
        HMIEventChangeScreen(HMI_SCREEN_HOME, 0, 0, 0, 0, 0);
        break;
    default:
        break;
    }
}