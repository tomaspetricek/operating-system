#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef int spin_lock_t;

void spin_init(spin_lock_t *lock);
void spin_lock(spin_lock_t *lock);
void spin_unlock(spin_lock_t *lock);

#endif