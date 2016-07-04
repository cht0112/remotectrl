#ifndef REMOTECTRL_H
#define REMOTECTRL_H
#include <stdio.h>
#define CMD_PING                0

#define CMD_ROUTER_BEGIN        100
#define CMD_OPEN                101
#define CMD_OPEN_ACK            102
#define CMD_ROUTER_END          199

#define CMD_TOHOST_BEGIN        300
#define CMD_SHUTDOWN            301
#define CMD_SHUTDOWN_ACK        302
#define CMD_START_STREAM        311
#define CMD_START_STREAM_ACK    312
#define CMD_STOP_STREAM         313
#define CMD_STOP_STREAM_ACK     314
#define CMD_TOHOST_END          399

#define CMD_FROMHOST_BEGIN      500
#define CMD_FROMHOST_END        599

#pragma pack(1)
struct st_cmd_head{
        unsigned int  cmd;
        unsigned int len;
};
#pragma pack()

struct st_remote
{
        int udpsock;
        int tcpsock;
        unsigned short udpsport;
        unsigned short tcpsport;
        unsigned char recvbuf[1024];
	
};

#endif

