/*
 * @Module Name：    BMPDump.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-12-06
 */
#ifndef __BMPDUMP_h__
#define __BMPDUMP_h__

namespace SimulatorPlatform
{
    class BitMapDump
    {
    private:
        typedef struct FormatHeaser
        {
            unsigned short type;
            unsigned long  size;
            unsigned short reserved1;
            unsigned short reserved2;
            unsigned long  offset;
        } Heaser;
        typedef struct BitmapInfomation
        {
            unsigned long  size;
            unsigned long  width;
            unsigned long  height;
            unsigned short planes;
            unsigned short bitCount;
            unsigned long  compression;
            unsigned long  sizeImage;
            unsigned long  biXPixPerMeter;
            unsigned long  biYPixPerMeter;
            unsigned long  biClrUsed;
            unsigned long  biCirImportant;
        } BitmapInfo;
        
    protected:
        /**
        * Windows bitmap file size
        */
        unsigned long       fsize;                  /* file size    */

        /**
        * Windows bitmap file image data
        */
        unsigned char *     fdata;                  /* file data    */

        /**
        * Windows bitmap file format header
        */
        Heaser              head;                   /* header   */

        /**
        * Windows birmap file infomation
        */
        BitmapInfo          info;        
    }
}
#endif /* __BMPDUMP_h__ */
