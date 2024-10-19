#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct
{
    uint32_t width;   // measured in pixels
    uint32_t height;  // measured in pixels
    uint32_t vwith;   // virtual width
    uint32_t vheight; // virtual height
    uint32_t bytes;   // will be filled by the GPU
    uint32_t depth;
    uint32_t ignorex;
    uint32_t ignorey;
    void *pointer; // will be filled by the GPU
    uint32_t size; // will be filled by the GPU // is the framebuffer
} fb_init_t;

fb_init_t fbinit __attribute__((aligned(16)));

int framebuffer_init(void)
{
    mail_message_t msg;

    fbinit.width = 640;
    fbinit.height = 480;
    fbinit.vwith = fbinit.width;
    fbinit.vheight = fbinit.height;
    fbinit.depth = COLORDEPTH;

    msg.data = ((uint32_t)&fbinit + 0x40000000) >> 4;

    mailbox_send(msg, FRAMEBUFFER_CHANNEL);
    msg = mailbox_read(FRAMEBUFFER_CHANNEL);

    if (!msg.data)
    {
        return -1;
    }
    fbinfo.width = fbinit.width;
    fbinfo.height = fbinit.height;
    fbinfo.chars_width = fbinfo.width / CHAR_WIDTH;
    fbinfo.chars_height = fbinfo.height / CHAR_HEIGHT;
    fbinfo.chars_x = 0;
    fbinfo.chars_y = 0;
    fbinfo.pitch = fbinit.bytes;
    fbinfo.buf = fbinit.pointer;
    fbinfo.buf_size = fbinit.size;

    return 0;
}

typedef struct
{
    uint32_t channel : 4;
    uint32_t data : 28;
} mail_message_t;

typedef struct
{
    uint32_t reserved : 30;
    uint8_t empty : 1;
    uint8_t full : 1;
} mail_status_t;

mail_message_t mailbox_read(int channel)
{
    mail_status_t stat;
    mail_message_t res;

    // make sure that the message is from the right channel
    do
    {
        // make sure there is mail to receive
        do
        {
            stat = *MAIL0_STATUS;
        } while (stat.empty)

            // get the message
            res = *MAIL0_READ;
    } while (res.channel != channel);

    return res;
}

void mailbox_send(mail_message_t msg, int channel)
{
    mail_status_t stat;
    msg.channel = channel;

    // Make sure you can send mail
    do
    {
        stat = *MAIL0_STATUS;
    } while (stat.full);

    // send message
    *MAIL0_WRITE = msg;
}

#endif // FRAMEBUFFER_H