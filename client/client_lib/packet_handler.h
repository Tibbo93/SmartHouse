#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include "../common/TinyFrame.h"

#define REQUEST_MSG 1
#define REPLY_MSG 2
#define ERROR_MSG 3

void packetHandler_init(int serialPort);

void receive_message(char *buff, int fd, TinyFrame *tf);

void dumpFrame(const uint8_t *buff, size_t len);

void dumpFrameInfo(TF_Msg *msg);

#endif