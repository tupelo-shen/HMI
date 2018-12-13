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
    
    private:
        /**
         * 16 bit color raw data
         */
        unsigned short *    raw;

        /**
         * 24 bit color raw data
         */
        unsigned char *     raw24bit;

        /**
         * 24 bit color raw data(duble size)
         */
        unsigned char *     raw24bit2;

    public:
        BitMapDump(void);
        virtual ~BitMapDump(void);

        virtual BitMapDump&         operator=(const BitMapDump& org);
        virtual void                loadFile(const char* f_name);
        virtual void                makeTable();
        virtual void                loadDat();
    private:
        virtual void                createRawData(void);
    public:
        virtual void                convFromRaw(unsigned long y, unsigned long height);
        virtual void                copyRaw(BitMapDump & org);
        virtual void                mkFileImage(void);
        virtual unsigned char*      get24BitRawData(void);
        virtual unsigned char*      get24BitRawData2(void);
        virtual void                print(void) const;
        unsigned long               size(void);
        unsigned char*              ptr(void);
        unsigned long               imageSize(void);
        unsigned char*              image(void) const;
        unsigned long               width(void) const;
        unsigned long               height(void) const;
        void                        fill(unsigned short data);
        void                        convertColor8to16(unsigned char *color8, 
                                                        unsigned short *color_pal);
        unsigned short*             getRaw();
        void                        clearRaw();        
    };
}
#endif /* __BMPDUMP_h__ */
