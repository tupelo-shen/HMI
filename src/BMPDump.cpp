#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "GDC_Sim.h"
#include "BMPDump.h"

using namespace SimulatorPlatform;
using namespace std;
using namespace boost;

typedef struct
{
    char *  dst;
    unsigned long   size;
} TableList;

BitMapDump::BitMapDump(void): fsize(0), fdata(0), raw(0), raw24bit(0), raw24bit2(0)
{
}

BitMapDump::~BitMapDump(void)
{
    if(0 != fdata)
    {
        delete [] fdata;
        fdata = 0;
    }
    if(0 != raw)
    {
        delete [] raw;
        raw = 0;
    }
    if(0 != raw24bit)
    {
        delete [] raw24bit;
        raw24bit = 0;
    }
    if(0 != raw24bit2)
    {
        delete [] raw24bit2;
        raw24bit2 = 0;
    }
}

BitMapDump& BitMapDump::operator=(const BitMapDump & org)
{
    if(0 != fdata)
    {
        delete [] fdata;
        fdata = 0;
    }
    if(0 != raw)
    {
        delete [] raw;
        raw = 0;
    }
    if(0 != raw24bit)
    {
        delete [] raw24bit;
        raw24bit = 0;
    }
    if(0 != raw24bit2)
    {
        delete [] raw24bit2;
        raw24bit2 = 0;
    }
    fsize = org.fsize;
    fdata = new unsigned char [fsize];
    memcpy(fdata, org.fdata, fsize);
    memcpy(&(head), &(org.head), sizeof(head));
    memcpy(&(info), &(org.info), sizeof(info));
    raw = new unsigned short [width() * height()];
    memcpy(raw,  org.raw, (sizeof(unsigned short) * width() * height()));
    return *this;
}

void BitMapDump::loadFile(const char * fname)
{
    const filesystem::path path(fname);
    try
    {
        fsize = file_size(path);
        fdata = new unsigned char [fsize];
        ifstream fin(fname, ios::in | ios::binary);
        if(fin)
        {
            fin.read((char *)fdata, fsize);
            makeTable();
            createRawData();
        }
    }
    catch (filesystem::filesystem_error & exp)
    {
        fsize   = 0;
        fdata   = 0;
        throw;
    }
}

void BitMapDump::loadDat() 
{
  static unsigned short z = 0;
  for(unsigned int i=0; i<height(); i++) {
    for(unsigned int j=0; j<width(); j++) {
      if((i / 10) % 2 > 0) {
        raw[i*width()+j] = 0xF000+z;
      } else {
        raw[i*width()+j] = 0X000F+z;
      }
    }
  }
  z++;
}

void BitMapDump::makeTable()
{
  TableList headerTable[] = 
  {
    { reinterpret_cast<char*>(&(head.type)),        sizeof(head.type)       },
    { reinterpret_cast<char*>(&(head.size)),        sizeof(head.size)       },
    { reinterpret_cast<char*>(&(head.reserved1)),   sizeof(head.reserved1)  },
    { reinterpret_cast<char*>(&(head.reserved2)),   sizeof(head.reserved2)  },
    { reinterpret_cast<char*>(&(head.offset)),      sizeof(head.offset)     }
  };

  TableList infoTable[] = 
  {
    { reinterpret_cast<char*>(&(info.size)),            sizeof(info.size)           },
    { reinterpret_cast<char*>(&(info.width)),           sizeof(info.width)          },
    { reinterpret_cast<char*>(&(info.height)),          sizeof(info.height)         },
    { reinterpret_cast<char*>(&(info.planes)),          sizeof(info.planes)         },
    { reinterpret_cast<char*>(&(info.bitCount)),        sizeof(info.bitCount)       },
    { reinterpret_cast<char*>(&(info.compression)),     sizeof(info.compression)    },
    { reinterpret_cast<char*>(&(info.sizeImage)),       sizeof(info.sizeImage)      },
    { reinterpret_cast<char*>(&(info.biXPixPerMeter)),  sizeof(info.biXPixPerMeter) },
    { reinterpret_cast<char*>(&(info.biYPixPerMeter)),  sizeof(info.biYPixPerMeter) },
    { reinterpret_cast<char*>(&(info.biClrUsed)),       sizeof(info.biClrUsed)      },
    { reinterpret_cast<char*>(&(info.biCirImportant)),  sizeof(info.biCirImportant) }
  };

  unsigned long offset = 0;
  for(unsigned long idx = 0; idx < (sizeof(headerTable) / sizeof(TableList)); idx ++)
  {
    memcpy(headerTable[idx].dst, &(fdata[offset]), headerTable[idx].size);
    offset += headerTable[idx].size;
  }

  for(unsigned long idx = 0; idx < (sizeof(infoTable) / sizeof(TableList)); idx ++)
  {
    memcpy(infoTable[idx].dst, &(fdata[offset]), infoTable[idx].size);
    offset += infoTable[idx].size;
  }
}


static unsigned short * createtRawDataFrom16bit(unsigned long width, unsigned long height, unsigned char * src)
{
    unsigned short * raw = new unsigned short [width * height];
    for(unsigned long y = 0; y < height; y ++)
    {
        for(unsigned long x = 0; x < width; x ++)
        {
            unsigned long src_idx = (height - 1 - y) * width * 2 + (x * 2);
            unsigned long data = ((static_cast<unsigned short>(src[src_idx+1])<<8) | static_cast<unsigned short>(src[src_idx]));
            raw[(y*width)+x] = data;
        }
    }
    return raw;
}

static unsigned short * createtRawDataFrom24bit(unsigned long width, unsigned long height, unsigned char * src)
{
    unsigned short * raw = new unsigned short [width * height];
    for(unsigned long y = 0; y < height; y ++)
    {
        for(unsigned long x = 0; x < width; x ++)
        {
            unsigned short data = 0;
            unsigned long idx = ((height - 1 - y) * width * 3) + (x * 3);
            unsigned long r = src[idx + 2];
            unsigned long g = src[idx + 1];
            unsigned long b = src[idx   ];
            r = (r * 31) / 255;
            g = (g * 63) / 255;
            b = (b * 31) / 255;
            data |= (r & 0x001F)<<11;
            data |= (g & 0x003F)<<5;
            data |= (b & 0x001F);
            raw[(y*width)+x] = data;
        }
    }
    return raw;
}

void BitMapDump::createRawData(void)
{
    switch(info.bitCount)
    {
        case 16:
            raw = createtRawDataFrom16bit(width(), height(), image());
            break;
        case 24:
            raw = createtRawDataFrom24bit(width(), height(), image());
            break;
        default:
            break;
    }
}
/*
 *      将raw原始数据转换为24位原始数据或者24位-2原始数据
 *
 * @params
 *          y， 原始数据的y坐标
 *          height, 图像的高度
 * @return
 *          None。
 */
void BitMapDump::convFromRaw(unsigned long y, unsigned long height)
{
    if(0 != raw)
    {
        unsigned long width = this->width();    // 得到像素宽度
        
        // 如果没有指向24位原始数据区域，则创建一个这样的区域
        if(0 == raw24bit)   
        {
            unsigned long height= this->height();
            raw24bit = new unsigned char [width * height * 3];
        }
        if(0 == raw24bit2)
        {
            unsigned long height= this->height();
            raw24bit2 = new unsigned char [width * height * 3 * 4];
        }

        // 
        for(; y < height; y ++)
        {
            for(unsigned long x = 0; x < width; x ++)
            {
                unsigned short data = raw[(y * width) + x];
                unsigned long r = ((data >> 11) & 0x001F);
                unsigned long g = ((data >>  5) & 0x003F);
                unsigned long b = ((data     ) & 0x001F);
                r = (r * 255) / 31;
                g = (g * 255) / 63;
                b = (b * 255) / 31;

                unsigned long idx = (((y    )    ) * ((width * 3)    )) + ((x * 3)    );
                raw24bit[idx ] = r;
                raw24bit[idx+1] = g;
                raw24bit[idx+2] = b;

                idx = (((y * 2)    ) * ((width * 3) * 2)) + ((x * 3) * 2);
                raw24bit2[idx     ] = r;
                raw24bit2[idx   + 1] = g;
                raw24bit2[idx   + 2] = b;
                raw24bit2[idx+3   ] = r;
                raw24bit2[idx+3 + 1] = g;
                raw24bit2[idx+3 + 2] = b;
                idx = (((y * 2) + 1) * ((width * 3) * 2)) + ((x * 3) * 2);
                raw24bit2[idx     ] = r;
                raw24bit2[idx   + 1] = g;
                raw24bit2[idx   + 2] = b;
                raw24bit2[idx+3   ] = r;
                raw24bit2[idx+3 + 1] = g;
                raw24bit2[idx+3 + 2] = b;
            }
    }
    }
}

void BitMapDump::copyRaw(BitMapDump & org)
{
    memcpy(raw,  org.raw, (sizeof(unsigned short) * width() * height()));
}

void BitMapDump::mkFileImage(void)
{
    unsigned char * dst = image();
    unsigned long height = this->height();
    unsigned long width  = this->width();
    for(unsigned long y = 0; y < height; y ++)
    {
        for(unsigned long x = 0; x < width; x ++)
        {
            unsigned long data = raw[(y*width)+x];
            unsigned long idx = (((height - 1) - y) * (width * 2)) + (x * 2);
            dst[idx+1] = static_cast<unsigned char>(data >> 8);
            dst[idx ] = static_cast<unsigned char>(data & 0x00ff);
        }
    }
}

unsigned char * BitMapDump::get24BitRawData(void)
{
    if(0 == raw24bit)
    {
        convFromRaw(0, this->height());
    }
    return raw24bit;
}

/*
 *      得到24位原始数据
 *
 * @params
 *          void
 * @return
 *          返回数据的地址
 */
unsigned char * BitMapDump::get24BitRawData2(void)
{
    if(raw24bit2 != NULL) {
        delete [] raw24bit2;
        raw24bit2 = 0;
    }
    if(0 == raw24bit2)
    {
        convFromRaw(0, this->height());
    }
    return raw24bit2;
}

void BitMapDump::convertColor8to16(unsigned char *color8, unsigned short *color_pal)
{
    if(raw == NULL) {
        raw = new unsigned short [this->width() * this->height()];
    }

    for(unsigned long j=0; j<height(); j++) {
        for(unsigned long i =0; i<width(); i++) {
            raw[i+j*width()] = color_pal[color8[VramAddr(i, j)]];
            //std::cout << i+j*width() << " " << raw[i+j*width()] << std::endl;
        }
    }
}

unsigned long rgb16_to_rgb32(unsigned short a)
{
    /* 1. Extract the red, green and blue values */
    /* from rrrr rggg gggb bbbb */
    unsigned long r = (a & 0xF800) >11;
    unsigned long g = (a & 0x07E0) >5;
    unsigned long b = (a & 0x001F);

    /* 2. Convert them to 0-255 range:
    There is more than one way. You can just shift them left:
    to 00000000 rrrrr000 gggggg00 bbbbb000
    r <<= 3;
    g <<= 2;
    b <<= 3;
    But that means your image will be slightly dark and
    off-colour as white 0xFFFF will convert to F8,FC,F8
    So instead you can scale by multiply and divide: */
    r = r * 255 / 31;
    g = g * 255 / 63;
    b = b * 255 / 31;
    /* This ensures 31/31 converts to 255/255 */

    /* 3. Construct your 32-bit format (this is 0RGB): */
    return (r << 16) | (g << 8) | b;
}


void BitMapDump::print(void) const
{
    std::ios::fmtflags flagsSaved = std::cout.flags();

    cout << "File Size : "<< fsize << endl;

    cout << std::hex << std::setfill('0');
    cout << "head.type      = 0x" << std::setw(4) << head.type << "(" << static_cast<char>(head.type & 0x00ff) << static_cast<char>(head.type >> 8) << ")" << endl;
    cout << "head.size      = 0x" << std::setw(8) << head.size << endl;
    cout << "head.reserved1 = 0x" << std::setw(4) << head.reserved1 << endl;
    cout << "head.reserved2 = 0x" << std::setw(4) << head.reserved2 << endl;
    cout << "head.offset    = 0x" << std::setw(8) << head.offset << endl;

    cout << std::dec << std::setfill(' ') << std::setw(1);
    cout << "info.offset         = " << info.size << endl;
    cout << "info.size           = " << info.size << endl;
    cout << "info.width          = " << info.width << endl;
    cout << "info.height         = " << info.height << endl;
    cout << "info.planes         = " << info.planes << endl;
    cout << "info.bitCount       = " << info.bitCount << endl;
    cout << "info.compression    = " << info.compression << endl;
    cout << "info.sizeImage      = " << info.sizeImage << endl;
    cout << "info.biXPixPerMeter = " << info.biXPixPerMeter << endl;
    cout << "info.biYPixPerMeter = " << info.biYPixPerMeter << endl;
    cout << "info.biClrUsed      = " << info.biClrUsed << endl;
    cout << "info.biCirImportant = " << info.biCirImportant << endl;

    cout << std::hex << std::setfill('0') << std::setw(8) ;
    cout << "image()   = 0x" << reinterpret_cast< unsigned long >(image()) << endl;

    unsigned char * bmp = this->image();
    cout << std::setw(2);
    for(unsigned long idx = 0; idx < info.sizeImage; idx ++)
    {
        if(0 == (idx % 16))
        {
            cout << std::setw(8)  << idx << ":";
        }
        cout << std::setw(2) << static_cast<int>(bmp[idx]) << " ";
        if(7 == (idx % 16))
        {
            cout << " ";
        }
        if(15 == (idx % 16))
        {
            cout << endl;
        }
    }
    std::cout.flags(flagsSaved);
}

unsigned short *BitMapDump::getRaw() {
  return raw;
}

void BitMapDump::clearRaw() {
  delete [] raw;
  raw = 0;
}
