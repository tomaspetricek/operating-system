#include <kernel/mem.h>
#include <kernel/atag.h>
#include <common/stdlib.h>
#include <stdint.h>
#include <stddef.h>

extern uint8_t __end;

static uint32_t num_pages;

DEFINE_LIST(page);
IMPLEMENT_LIST(page);

static page_t *all_pages_array;
page_list_t free_pages;

void mem_init(atag_t *atags)
{
    uint32_t mem_size, page_array_len, kernel_pages, i;

    // get the total number of pages
    mem_size = get_mem_size(atags);
    num_pages = mem_size / PAGE_SIZE;

    // allocate space for pages metadata.
    // start this block just after the kernel image is finished
    page_array_len = sizeof(page_t) * num_pages;
    all_pages_array = (page_t *)&__end;
    bzero(all_pages_array, page_array_len);
    INITIALIZE_LIST(free_pages);

    // iterate over all pages and mark them with the appropriate flags
    // start with kernel pages
    kernel_pages = ((uint32_t)&__end) / PAGE_SIZE;
    for (i = 0; i < kernel_pages; i++)
    {
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE; // identity map the kernel pages
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_page = 1;
    }
    // map the rest of the pages as unallocated, and add them to the free list
    for (; i < num_pages; i++)
    {
        all_pages_array[i].flags.allocated = 0;
        append_page_list(&free_pages, &all_pages_array[i]);
    }
}

void *alloc_page(void)
{
    page_t *page;
    void *page_mem;

    if (size_page_list(&free_pages) == 0)
        return 0;

    // get a free page
    page = pop_page_list(&free_pages);
    page->flags.kernel_page = 1;
    page->flags.allocated = 1;

    // get the address the physical page metadata refers to
    page_mem = (void *)((page - all_pages_array) * PAGE_SIZE);

    // zero out the page
    bzero(page_mem, PAGE_SIZE);

    return page_mem;
}

void free_page(void *ptr)
{
    page_t *page;

    // get page metadata from the physical address
    page = all_pages_array + ((uint32_t)ptr / PAGE_SIZE);

    // mark the page as free
    page->flags.allocated = 0;
    append_page_list(&free_pages, page);
}

void *kmalloc(uint32_t bytes)
{
    heap_segment_t *curr, *best = NULL;
    int diff, best_diff = 0x7fffffff; // max signed int

    bytes += sizeof(heap_segment_t);
    bytes += bytes % 16 ? 16 - (bytes % 16) : 0; // what does it do?

    // find allocation that is the closests in size to this request
    for (curr = heap_segment_list_head; curr != NULL; curr = curr->next)
    {
        diff = curr->segment_size - bytes;

        if (!curr->is_allocated && diff < best_diff && diff >= 0)
        {
            best = curr;
            best_diff = diff;
        }
    }

    // there is no free memery now
    if (best == NULL)
    {
        return NULL;
    }

    // if the best difference we could come up was large, split up the segement into two
    // since our segment headers are rather large, the criterion for splitting the segment is that
    // when split, the segment not being requested should be twice a header size
    if (best_diff > (int)(2 * sizeof(heap_segment_t)))
    {
        bzero(((void *)(best)) + bytes, sizeof(heap_segment_t));
        curr = best->next;
        best->next = ((void *)(best)) + bytes;
        best->next->next = curr;
        best->next->prev = best;
        best->next->segment_size = best->segment_size - bytes;
        best->segment_size = bytes;
    }
    best->is_allocated = 1;
    return best + 1; // why is there the plus one?
}