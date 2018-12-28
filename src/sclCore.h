#ifndef __SCL_CORE_H__
#define __SCL_CORE_H__

#include "HMI_Alloc.h"
#include "sclBase.h"


// 类信息的宏定义
#define M_DECLARE_RUNTIME(class_name)   /* pgr0767 */ \
protected: \
    virtual const ClassInfo* GetClassInfo() const {return(&m_classInfo);} \
public: \
    virtual int GetUpdateType() const {return(GetClassInfo()->updateType);} \
    static const ClassInfo m_classInfo;

#define M_IMPLEMENT_RUNTIME(class_name, update_type, super_class_name) /* pgr0766 */ /* pgr0767 */ \
    const ClassInfo class_name::m_classInfo = { #class_name, update_type, &super_class_name::m_classInfo};

#define M_RUNTIME_TYPE(class_name)  (&class_name::m_classInfo)

/*
 * 类信息结构体
 */
struct  ClassInfo {
    const char*             pClassName;
    const int               updateType;
    const struct ClassInfo* pSuperClassInfo;
};

/*
 * 类Core定义
 */
class SCCore : public HMIAlloc {
    M_DECLARE_RUNTIME(SCCore)
public :
    SCCore(const unsigned short x, const unsigned short y,
            const unsigned short width, const unsigned short height);
    virtual ~SCCore() = 0;

    bool    IsClass(const ClassInfo* classInfo) const;
    bool    IsFamily(const ClassInfo* classInfo) const;
    void    GetArea(SCRect* area) const;

protected :
    SCRect  m_area;
};

#endif  /* __SCL_CORE_H__ */