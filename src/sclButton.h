/**
* @file sclButton.h
*           sclButton
*
* @author   
* @version
*/

#ifndef _SCL_BUTTON_H_
#define _SCL_BUTTON_H_

#include "sclLabel.h"

/*
 * Button class definition
 */
class SCButton : public SCLabel
{
    M_DECLARE_RUNTIME(SCButton)
public :
    SCButton(const short id, const SCRect& area,
            unsigned char fore_color = COLOR_BLACK,
            unsigned char back_color = COLOR_TRANSPARENT,
            unsigned int work_scr = SC_UPDATE_WORK,
            unsigned short font_type = SC_PARTS_FONT_TYPE_MFONT, bool trans=false);
    virtual ~SCButton();

    virtual int             Draw(unsigned short* com);
    virtual void            TDown(const SCPoint& point);
    virtual void            TMove(const SCPoint& point,
                                const unsigned int level, const int direction);
    virtual void            TUp(const SCPoint& point);
    virtual void            TFlick(const SCPoint& point,
                                const unsigned int level, const int direction);
    virtual void            TLongTap(const SCPoint& point);

protected:
    virtual unsigned char   getButtonColor();
    int                     drawRect(unsigned short** com_pos);
    int                     drawUnderLine(unsigned short** com_pos);
    int                     drawSideLine(unsigned short** com_pos);
private:
    void                    redrawButton(const SCPoint& point);
    
private:
    bool                    m_transparent;

};

#endif  // _SCL_BUTTON_H_    end

