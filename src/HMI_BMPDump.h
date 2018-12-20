#ifndef __HMI_BMPDUMP_H__
#define __HMI_BMPDUMP_H__

#include "BMPDump.h"

/**
 *
 * @file BMPDump.h
 *
 * @brief Window bitmap format and raw data process class
 *
 * @author Shouji, Igarashi
 *
 */

namespace SimulatorPlatform
{
    class HMI_BitMapDump : public BitMapDump
    {
    public:
        virtual void init() 
        {
            head.type = 0x424d;     // 'BM',Windows支持的位图格式
            head.size = sizeof(head) + sizeof(info) + 204800 * 2;//76800 * 8;
            head.reserved1 = 0;
            head.reserved2 = 0;
            head.offset = sizeof(head) + sizeof(info);

            info.size = 40;
            info.width = 320 * 2;
            info.height = 240 * 2;
            info.planes = 1;
            info.bitCount = 16;
            info.compression = 0; // 無圧縮
            info.sizeImage = 204800 * 2;//76800 * 8;
            info.biXPixPerMeter = 0;
            info.biYPixPerMeter = 0;
            info.biClrUsed = 0;
            info.biCirImportant = 0;
        }
    };
}

#endif /* __HMI_BMPDUMP_H__ */
