#include "DrawCommand.h"

DrawCommand::DrawCommand(unsigned short cmd) 
{
    setCommand(cmd);
    cmd_raw = cmd;
}

void DrawCommand::setCommand(unsigned short cmd)
{
    cmd_conf = (cmd & 0x3FF);
    cmd_no = ((cmd >> 10) & 0x3F);
}

unsigned char DrawCommand::no() 
{
  return cmd_no;
}

unsigned short DrawCommand::conf() 
{
  return cmd_conf;
}

unsigned short DrawCommand::raw() 
{
    return cmd_raw;
}