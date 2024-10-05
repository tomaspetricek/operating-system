#include <stdint.h>
#include <kernel/atag.h>
#include <kernel/list.h>

#ifndef MEM_H
#define MEM_H

#define PAGE_SIZE 4096

typedef struct {
	uint8_t allocated: 1;   // allocated to something
	uint8_t kernel_page: 1; // part of the kernel
	uint32_t reserved: 30;
} page_flags_t;

typedef struct page {
	uint32_t vaddr_mapped;	// virtual address that maps to this page	
	page_flags_t flags;
	DEFINE_LINK(page);
} page_t;

void mem_init(atag_t * atags);

void * alloc_page(void);

void free_page(void * ptr);

typedef struct heap_segment{
	struct heap_segment* next;
	struct heap_segment* prev;
	uint32_t is_allocated;
	uint32_t segment_size; // includes this header
} heap_segment_t;

void* kmalloc(uint32_t bytes);

void kfree(void *ptr);

#endif