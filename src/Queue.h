#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdexcept>
#include <cstddef>

namespace ana_stl
{
    template<class ELEMENT, unsigned int m_size>
    class Queue
    {
    private:
        bool full;
        unsigned int front;
        unsigned int back;
        ELEMENT m_e[m_size];

        unsigned int next(unsigned int num)
        {
            return (num + 1) % m_size;
        }
    public:
        Queue(){front = 0; back = 0; full = false;}
        
        // 添加元素到队尾
        bool add(ELEMENT e)
        {
            if(full)
            {
                return false;
            }
            else
            {
                m_e[back] = e;
                back = next(back);
                if(front == back)
                {
                    full = true;
                }
                return true;
            }
        }

        // 从队列中取出一个元素
        ELEMENT remove()
        {
            if(front != back || full)
            {
                if(full)
                {
                    full = false;
                }
                ELEMENT e = m_e[front];
                front = next(front);
                return e;
            }
            else
            {
                throw std::out_of_range("Empty queue");
            }
        }

        // 获取队列当前最前头的元素
        ELEMENT element()
        {
            if (front != back || full)
            {
                return m_e[front];
            }else
            {
                throw std::out_of_range("Empty Queue");
            }
        }

        // 获取队列当前最前头的元素
        ELEMENT element_back()
        {
            if (front != back || full)
            {
                return m_e[back-1];
            }else
            {
                return 0;
            }
        }
        // 当前队列中的元素个数
        std::size_t count()
        {
            if(full)
            {
                return m_size;
            }
            else
            {
                return front <= back ? back - front : back + m_size - front;
            }
        }

        // 清空队列
        void clear()
        {
            front = 0;
            back = 0;
            full = false;
        }

        // 判断队列是否为空
        bool empty()
        {
            return front == back && !full;
        }

        // 返回队列的大小
        unsigned int size()
        {
            return m_size;
        }
    };
};
#endif /* __QUEUE_H__ */
