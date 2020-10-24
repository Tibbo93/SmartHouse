#include "packet_handler.h"
#include "../../common/TinyFrame.h"
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

int serialPort = 0;

void dumpFrame(const uint8_t *buff, size_t len) {

    size_t i;
    int data_len = 0;

    for (i = 0; i < len; i++) {
        printf("%3u \033[94m%02X\033[0m", buff[i], buff[i]);
        if (buff[i] >= 0x20 && buff[i] < 127) {
            printf(" %c", buff[i]);
        } else {
            printf(" \033[31m.\033[0m");
        }
        printf("\n");
    }
    printf("--- end of frame ---\n\n");
}

void dumpFrameInfo(TF_Msg *msg) {
    printf("\033[33mFrame info\n"
           "  type: %02Xh\n"
           "  data: \"%.*s\"\n"
           "   len: %u\n"
           "    id: %Xh\033[0m\n\n",
           msg->type, msg->len, msg->data, msg->len, msg->frame_id);
}

void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len) {

    printf("DENTRO WRITE\n");
    /*while(*buff) {
        printf("%c", *buff);
        buff++;
    }*/
    
    if (serialPort != -1) {
        printf("write: %zu", write(serialPort, buff, len));
    } else {
        printf("\nNo peer!\n");
    }
}

void receive_message(char *buff, int fd, TinyFrame *tf) {

    printf("DENTRO RECEIVE\n");

    if (serialPort == 0)
        serialPort = fd;

    int recvBytes = read(serialPort, buff, TF_MAX_PAYLOAD_RX);
    if (recvBytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return;
    }
    printf("PRIMA ACCEPT bytes ricevuti: %d\n", recvBytes);
    TF_Accept(tf, buff, recvBytes);
    printf("ACCEPT FINITA\n");
}
