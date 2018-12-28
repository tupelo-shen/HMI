/**
* @file sclCore.cpp
*           SCCore类定义
* 
* @author   
* @version  
*/
#include <cstddef>
#include "sclBase.h"

// 设定类信息
const ClassInfo SCCore::m_classInfo = {"SCCore", SCUpdateTypeNone, 0};

SCCore::SCCore(
    const unsigned short x,
    const unsigned short y,
    const unsigned short width,
    const unsigned short height
)
{
    m_area.Set(x, y, width, height);
}


SCCore::~SCCore(
)
{
}

/****************************************************************************/
/**
 * @brief   IsClass函数
 *          确定对象类是否是指定的类
 *
 * @author  Tupelo
 * @date    2018/12/27
 *
 * @param[in]   const ClassInfo* classInfo
 *
 * @return  bool
 * @retval  true
 * @retval  false
 */
/****************************************************************************/
bool    SCCore::IsClass(const ClassInfo* classInfo) const
{
    if(this != 0) 
    {
        if(GetClassInfo() == classInfo) 
        {
            return(true);
        }
    }

    return(false);
}

/****************************************************************************/
/**
 * @brief   IsFamily函数
 *          确定对象的类是否是指定的类直接/间接派生类
 *
 * @author  Tupelo
 * @date    2018/12/27
 *
 * @param[in]   const ClassInfo* classInfo
 *
 * @return  bool
 * @retval  true
 * @retval  false
 */
/****************************************************************************/
bool SCCore::IsFamily(const ClassInfo* classInfo) const
{
    if(this != NULL) 
    {
        const ClassInfo* pInfo = GetClassInfo();

        while(pInfo != NULL) 
        {
            if(pInfo == classInfo) 
            {
                return(true);
            }

            pInfo = pInfo->pSuperClassInfo;
        }
    }

    return(false);
}

/****************************************************************************/
/**
 * @brief   GetArea函数
 *          将部品的区域拷贝到通过参数传递进来的SCRect中
 *
 * @author  Tupelo
 * @date    2018/12/27
 *
 * @param[in/out]   SCRect* area
 *
 * @return  なし
 */
/****************************************************************************/
void SCCore::GetArea(SCRect* area) const
{
    area->Set(m_area.x, m_area.y, m_area.width, m_area.height);
}