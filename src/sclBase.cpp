/**
* @file sclBase.h
*           绘图基础库
*
* @author   
* @version  
*/
#include "sclBase.h"

// SCRect class
bool SCRect::Contains(const SCPoint&  point) const
{
    if((point.x >= x) && (point.x < x + width) &&
        (point.y >= y) && (point.y < y + height)) 
    {
        return(true);
    }

    return(false);
}

bool SCRect::Contains(unsigned short tx, unsigned short ty) const
{
    if( (tx >= x) && (tx < x + width) &&
        (ty >= y) && (ty < y + height)) 
    {
        return(true);
    }

    return(false);
}

bool SCRect::Contains(const SCRect& rect) const
{
    if((rect.x >= x) && (rect.x + rect.width <= x + width) &&
        (rect.y >= y) && (rect.y + rect.height <= y + height)) 
    {
        return(true);
    }

    return(false);
}

bool SCRect::Overlap(const SCRect& rect) const
{
    if( (rect.x < x + width) && (rect.y < y + height) &&
        (rect.x + width > x) && (rect.y + rect.height > y) ) 
    {
        return(true);
    }

    return(false);
}

bool SCRect::operator != ( const SCRect& rect) const
{
    if( (rect.x != x) || (rect.y != y) ||
        (rect.width != width) || (rect.height != height) ) 
    {
        return(true);
    }

    return(false);
}

bool SCRect::operator == (const SCRect& rect) const
{
    if( (rect.x == x) && (rect.y == y) &&
        (rect.width == width) && (rect.height == height) ) 
    {
        return(true);
    }

    return(false);
}