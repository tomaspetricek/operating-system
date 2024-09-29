#include <common/stdio.h>
#include <common/stdlib.h>
#include <kernel/uart.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    NONE = 0x00000000,
    CORE = 0x54410001,
    MEM = 0x54410002,
} atag_tag_t;

typedef struct
{
    uint32_t size;
    uint32_t start;
} mem_t;

typedef struct atag
{
    uint32_t tag_size;
    atag_tag_t tag;
    union
    {
        mem_t mem;
    };
} atag_t;

uint32_t get_mem_size(atag_t *tag)
{
    while (tag->tag != NONE)
    {
        if (tag->tag == MEM)
        {
            return tag->mem.size;
        }
        tag = ((uint32_t *)tag) + tag->tag_size;
    }
    return 0;
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void)r0;
    (void)r1;
    (void)atags;

    uart_init();
    uart_puts("Hello, kernel World!\r\n");

    while (1)
    {
        uart_putc(uart_getc());
        uart_putc('\n');
    }
}