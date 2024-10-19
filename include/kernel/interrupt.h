#ifndef INTERRUPT_H
#define INTERRUPT_H

void irq_handler(void) {
    printf("IRQ HANDLER\n");
    while(1);
}

void __attribute__ ((interrupt ("ABORT"))) reset_handler(void) {
    printf("RESER HANDLER\n");
    while(1);
}

void __attribute__ ((interrupt ("ABORT"))) prefetch_abort_handler(void) {
    printf("PREFETCH ABORT HANDLER\n");
    while(1);
}

void __attribute__ ((interrupt ("ABORT"))) data_abort_handler(void) {
    printf("DATA ABORT HANDLER\n");
    while(1);
}

void __attribute__ ((interrupt ("UNDEF"))) undefined_instruction_handler(void) {
    printf("UNDEFINED INSTRUCTION HANDLER\n");
    while(1);
}

void __attribute__ ((interrupt ("SWI"))) software_interrupt_handler(void) {
    printf("SWI HANDLER\n");
    while(1);
}

void __attribute__ ((interrupt ("FIQ"))) fast_irq_handler(void) {
    printf("FIQ HANDLER\n");
    while(1);
}
#endif // INTERRUPT_H