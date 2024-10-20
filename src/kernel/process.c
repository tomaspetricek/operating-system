#include <kernel/process.h>
#include <kernel/mem.h>
#include <kernel/interrupt.h>
#include <kernel/timer.h>
#include <common/stdlib.h>

static uint32_t next_proc_num = 1;
#define NEW_PID next_proc_num++;

extern uint8_t __end;

IMPLEMENT_LIST(pcb);

pcb_list_t run_queue;
pcb_list_t all_proc_list;

process_control_block_t *current_process;

void process_init(void)
{
    process_control_block_t *main_pcb;
    INITIALIZE_LIST(run_queue);
    INITIALIZE_LIST(all_proc_list);

    // allocate and intialize the block
    main_pcb = kmalloc(sizeof(process_control_block_t));
    main_pcb->stack_page = (void *)&__end;
    main_pcb->pid = NEW_PID;
    memcpy(main_pcb->proc_name, "Init", 5);

    // add self to all process list. it is already running, so don't add it to the run queue
    append_pcb_list(&all_proc_list, main_pcb);

    current_process = main_pcb;

    // set timer to go of after 10 ms. triggers the whole system
    timer_set(10000);
}

// round robbin scheduling
void schedule(void)
{
    DISABLE_INTERRUPTS();
    process_control_block_t *new_thread, *old_thread;

    // if nothing in the run queue, the current process should just continue
    if (size_pcb_list(&run_queue) == 0)
    {
        ENABLE_INTERRUPTS();
        return;
    }

    // get the next thread to run. for now we are using round-robin
    new_thread = pop_pcb_list(&run_queue);
    old_thread = current_process;
    current_process = new_thread;

    // put the current thread back in the run queue
    append_pcb_list(&run_queue, old_thread);

    // context switch
    switch_to_thread(old_thread, new_thread);
    ENABLE_INTERRUPTS();
}

static void reap(void)
{
    DISABLE_INTERRUPTS();
    process_control_block_t *new_thread, *old_thread;

    // if nothing on the run queue, there is nothing to do now. just loop
    while (size_pcb_list(&run_queue) == 0)
        ;

    new_thread = pop_pcb_list(&run_queue);
    old_thread = current_process;
    current_process = new_thread;

    // free the resources used by the old process.
    // technically, we are using dangling pointers here, but since interrupts are disabled and we only have one core, it
    // should still be fine
    free_page(old_thread->stack_page);
    kfree(old_thread);

    // context switch
    switch_to_thread(old_thread, new_thread);
}

void create_kernel_thread(kthread_function_f thread_func, char *name, int name_len)
{
    process_control_block_t *pcb;
    proc_saved_state_t *new_proc_state;

    // allocate and initialize the pcb
    pcb = kmalloc(sizeof(process_control_block_t));
    pcb->stack_page = alloc_page();
    pcb->pid = NEW_PID;
    memcpy(pcb->proc_name, name, MIN(name_len, 19));
    pcb->proc_name[MIN(name_len, 19) + 1] = 0;

    // get the locaion the stack pointer should be in when this is run
    // stack grows from the lower address to the higher ones
    // stack starts at: pcb->stack_page + PAGE_SIZE
    new_proc_state = pcb->stack_page + PAGE_SIZE - sizeof(proc_saved_state_t);
    pcb->saved_state = new_proc_state;

    // set up the stack that will be restored during a context switch
    bzero(new_proc_state, sizeof(proc_saved_state_t));
    new_proc_state->lr = (uint32_t)thread_func; // lr is used as return address in switch_to_thread
    new_proc_state->sp = (uint32_t)reap;        // when the thread function returns this reaper routine will clean it up
    new_proc_state->cpsr = 0x13 | (8 << 1);     // sets the thread up to run in supervisor mode with irqs only
    // 0x13 - suppervisor mode

    // add thread to the lists
    append_pcb_list(&all_proc_list, pcb);
    append_pcb_list(&run_queue, pcb);
}
