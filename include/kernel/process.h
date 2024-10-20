#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <kernel/list.h>

typedef void (*kthread_function_f)(void);

typedef struct
{
    uint32_t r1;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t cpsr; // current program state register
    uint32_t sp; // stack pointer probably
    uint32_t lr;
} proc_saved_state_t;

typedef struct pcb
{
    proc_saved_state_t *saved_state; // pointer to where on stack is the state saved, becomes invalid when the process is running
    void *stack_page;                // the stack for this process, the stack starts at the end of this page
    uint32_t pid;                    // process id number
    DEFINE_LINK(pcb);
    char proc_name[20];
} process_control_block_t;

void process_init(void);

void schedule(void);

void create_kernel_thread(kthread_function_f thread_func, char * name, int name_len);

#endif