#ifndef __DRAW_COMMAND_H__
#define __DRAW_COMMAND_H__

class DrawCommand 
{
private:
    unsigned short  cmd_raw;
    unsigned char   cmd_no;
    unsigned short  cmd_conf;

public:
    DrawCommand(){}
    DrawCommand(unsigned short cmd);
    void setCommand(unsigned short cmd);
    unsigned char no();
    unsigned short conf();
    unsigned short raw();
};

#endif