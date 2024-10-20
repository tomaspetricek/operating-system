#include <kernel/spinlock.h>

void spin_init(spin_lock_t *lock) { *lock = 1; }

void spin_lock(spin_lock_t *lock)
{
    // needs to be atomic
    while (!try_lock(lock))
        ;
}

void spin_unlock(spin_lock_t *lock) { *lock = 1; }