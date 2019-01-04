/**
* @file     sclCallback.h
*           回调函数 头文件
*
* @author   Tupelo
* @version  V1.01.01    2018/12/29
*/

#include "sclBase.h"
#include "sclCallback.h"


SCCallback::SCCallback(
    int     type,
    void    (*func)(int),
    int     clientData
) : m_func(func),
    m_type(type),
    m_clientData(clientData),
    m_next(0)
{
}


SCCallback::~SCCallback(
)
{
}


/****************************************************************************/
/**
 * @brief       DoCallback 函数
 *              执行回调函数
 *
 * @author      Tupelo
 * @date        2018/12/29
 *
 * @param       なし
 *
 * @return      なし
 */
/****************************************************************************/
void    SCCallback::DoCallback(void)
{
    if(m_func) 
    {
        m_func(m_clientData);
    }
}