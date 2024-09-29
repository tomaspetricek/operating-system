#include <common/stdio.h>
#include <common/stdlib.h>
#include <common/list.h>
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

#define PAGE_SIZE 4096

typedef struct
{
    uint8_t allocated : 1;   // is allocated to something
    uint8_t kernel_page : 1; // is part of the kernel
    uint32_t reserved : 30;
} page_flags_t;

typedef struct page
{
    uint32_t vaddr_mapped; // virtual address that maps to this page
    page_flags_t flags;
    DEFINE_LINK(page);
} page_t;

extern uint8_t __end; // from the linker script

static uint32_t num_pages;

DEFINE_LIST(page)
IMPLEMENT_LIST(page)

static page_t *all_pages_array;
page_list_t free_pages;

void mem_init(atag_t *atags)
{
    uint32_t mem_size, page_array_len, kernel_pages, i;

    // get total number of pages
    mem_size = 1024 * 1024 * 1024; // get_mem_size(atags);
    num_pages = mem_size / PAGE_SIZE;

    // allocate space for all pages metadata.
    // start block just right after the kernel image is finished
    page_array_len = sizeof(page_t) * num_pages;
    all_pages_array = (page_t *)&__end;
    bzero(all_pages_array, page_array_len);
    INITIALIZE_LIST(free_pages);

    // iterate over all pages and mark them with appropriate flags
    // start with kernel pages
    kernel_pages = ((uint32_t)&__end) / PAGE_SIZE;
    for (i = 0; i < kernel_pages; i++)
    {
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE; // identity map the kernel pages
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_page = 1;
    }

    // map the rest of the pages as unallocated,
    // and add them to the free list
    for (; i < num_pages; i++)
    {
        all_pages_array[i].flags.allocated = 0;
        append_page_list(&free_pages, &all_pages_array[i]);
    }
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