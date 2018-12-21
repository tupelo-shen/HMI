/**
* @file sclBase.h
*           绘图基础库
*
* @author   
* @version  
*/
#ifndef __SCL_BASE_H__
#define __SCL_BASE_H__

// --------------------------< 画面使用的基本定义 >--------------------------------
/*
 * 更新类型
 */
enum SCUpdateType {
    SCUpdateTypeNONE        = 0x00,     // 不更新
    SCUpdateTypeCYCLIC      = 0x01,     // 定周期更新
    SCUpdateTypeTREND       = 0x02,     // trend周期更新
    SCUpdateTypeCALTREND    = 0x04,     // 画面内的trend周期更新
    SCUpdateTypeTIMER       = 0x08,     // Timer更新
    SCUpdateTypeALL         = 0xff      // 全更新
};

/*
 * 错误定义
 */
#define SC_ERR_NONE     (0)
#define SC_ERR_ERROR    (1)

/*
 * 类SCPoint定义。设置，获取点坐标(x,y)
 */
struct SCPoint {

    SCPoint(): x(0), y(0){}
    SCPoint(unsigned short inX, unsigned short inY): x(inX), y(inY){}

    void Set(unsigned short inX, unsigned short inY) 
    {
        x = inX;
        y = inY;
    }

    bool operator != (const SCPoint &inPoint) const 
    {
        if( (x != inPoint.x) || (y != inPoint.y) ) 
        {
            return(true);
        }
        return(false);
    }

    bool operator == (const SCPoint &inPoint) const 
    {
        if( (x == inPoint.x) && (y == inPoint.y) ) 
        {
            return(true);
        }
        return(false);
    }

    SCPoint operator +(const SCPoint &point) const 
    {
        SCPoint newPt;

        newPt.Set(x + point.x, y + point.y);
        return(newPt);
    }

    unsigned short      x;
    unsigned short      y;
};

/*
 * 类SCRect定义。设置，获取矩形坐标(x,y,width,height)
 */
struct SCRect {

    SCRect():
        x(0),
        y(0),
        width(0),
        height(0){}

    SCRect(unsigned short inX, unsigned short inY, short inWidth,unsigned short inHeight):
        x(inX),
        y(inY),
        width(inWidth),
        height(inHeight){}

    void Set(unsigned short inX, unsigned short inY, unsigned short inWidth,
            unsigned short inHeight) {
        x = inX;
        y = inY;
        width = inWidth;
        height = inHeight;
    }

    bool            Contains(const SCPoint& point) const;
    bool            Contains(unsigned short tx, unsigned short ty) const;
    bool            Contains(const SCRect& rect) const;
    bool            Overlap(const SCRect& rect) const;
    bool            operator != (const SCRect& rect) const;
    bool            operator == (const SCRect& rect) const;

    unsigned short  Right(void) const {return(x + width - 1);}
    unsigned short  Bottom(void) const {return(y + height - 1);}

    unsigned short  x;
    unsigned short  y;
    unsigned short  width;
    unsigned short  height;

};
#endif /* __SCL_BASE_H__ */
