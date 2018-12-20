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
    /*
     *      | 数据段名称 | Windows结构体定义  | 大小（Byte）      |
     *      | bmp文件头  | BITMAPFILEHEADER  |     14          |
     *      | 位图信息头 | BITMAPINFOHEADER  |     40          |
     *      | 调色板     | ----------------  | 由颜色索引数决定 |
     *      | 位图数据   | ----------------  | 由图像尺寸决定   |
     *
     *    说明：
     *          1. bmp文件数据存储方式是以小端模式（little endian）
     */
    class BitMapDump
    {
    private:
        // bmp文件头结构体定义，共14个字节大小
        typedef struct FormatHeader
        {
            unsigned short type;            /* 文件类型 */
            unsigned long  size;            /* 文件大小，单位是字节 */
            unsigned short reserved1;       /* 保留1 */
            unsigned short reserved2;       /* 保留2 */
            unsigned long  offset;          /* 从文件头到实际的图像数据之间的偏移量，单位是字节 */
        } Header;

        // bmp文件信息头结构体定义
        typedef struct BitmapInfomation
        {
            unsigned long  size;            /* 结构体自身需要的字节数 */
            unsigned long  width;           /* 说明图像的宽度，用像素为单位 */
            unsigned long  height;          /* 说明图像的高度，用像素为单位 */
            unsigned short planes;          /* 说明颜色平面数，其值总是设为1 */
            unsigned short bitCount;        /* 说明比特数/像素，其值为1、4、8、16、24、32 */
            unsigned long  compression;     /* 说明图像数据压缩的类型 */
            unsigned long  sizeImage;       /* 说明图像大小，BI_RGB格式时，可设置为0 */
            unsigned long  biXPixPerMeter;  /* 水平分辨率 */
            unsigned long  biYPixPerMeter;  /* 垂直分辨率 */
            unsigned long  biClrUsed;       /* 颜色索引数 */
            unsigned long  biCirImportant;  /* 颜色索引数的重要性。0，表示都重要 */
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
        Header              head;                   /* header   */

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

    inline unsigned long BitMapDump::size( void )
    {
        return fsize;
    }

    inline unsigned char * BitMapDump::ptr( void )
    {
        return fdata;
    }

    inline unsigned long BitMapDump::imageSize( void )
    {
        return info.sizeImage;
    }

    inline unsigned char * BitMapDump::image( void ) const
    {
        return &( fdata[head.offset] );
    }

    inline unsigned long BitMapDump::width( void ) const
    {
        return info.width;
    }

    inline unsigned long BitMapDump::height( void ) const
    {
        return info.height;
    }

    inline void BitMapDump::fill( unsigned short data )
    {
        unsigned long size = width() * height();
        for( unsigned long idx = 0; idx < size; idx ++ )
        {
            raw[idx] = data;
        }
    }
}
#endif /* __BMPDUMP_h__ */
