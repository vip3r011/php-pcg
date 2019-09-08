/* PCG stands for Permutable Congruence Generator and was written
 *  by Melissa O'Neill: https://www.cs.hmc.edu/~oneill/index.html
 *
 * The original sources for PCG can be found at: http://www.pcg-random.org/
 */

#ifndef PHP_PCG_H
#define PHP_PCG_H

// Return the number of ticks from the CPU since the last restart.
#define seed(a) asm volatile ("rdtsc\n\t" "shl $32, %%rdx\n\t" "or %%rdx, %0" : "=a" (a) :: "rdx")

// Define the static multiplier for 64-bit PCG
#define multiplier 6364136223846793005ull

// Thread-unsafe spinlock functions
#define spinlock_declare(mutex) volatile int mutex = 0
#define spinlock_lock(mutex)    do { while(mutex == 1){} mutex = 1; } while(0)
#define spinlock_unlock(mutex)  mutex = 0

// Type aliases
typedef unsigned long long  qword;
typedef unsigned long       dword;

// Simple boolean data type
typedef enum { false, true } bool;

// Create our PCG structure
struct pcg_t { qword state, increment; };

// Random step function
void step(struct pcg_t *pcg) { pcg->state = pcg->state * multiplier + pcg->increment; }

// Random rotate function
qword rot(qword value, qword rotation) { return (value >> rotation) | (value << ((- rotation) & 63u)); }

// Return a rotated output
qword out(qword state) { return rot((state >> 32u) ^ state, state >> 59u); }

// Seeded random function
void pcg_srandom(struct pcg_t *pcg, qword init, qword sequence) {
    pcg->state = 0u;
    pcg->increment = (sequence << 1u) | 1u;
    step(pcg);

    pcg->state += init;
    step(pcg);
}

// Random function
qword pcg_rand(struct pcg_t *pcg) {
    step(pcg);
    return out(pcg->state);
}

// Seed our PRNG
void entropy(void *dest, dword size) {
    static spinlock_declare(mutex);
    spinlock_lock(mutex);

    static bool init = false;
    static struct pcg_t pcg;

    qword seeder;
    seed(seeder);

    if (!init) {
        dword temp;

        pcg_srandom(&pcg, (seeder ^ (qword) &entropy), (qword) &temp);
        init = true;
    }

    char *dest_ptr = (char *) dest;
    for (dword i = 0; i < size; ++i)
        dest_ptr[i] = (char) pcg_rand(&pcg);

    spinlock_unlock(mutex);
}

// Return a bounded random value
qword pcg_bind(struct pcg_t *pcg, qword max) {
    qword threshold = -max % max;
    while (1) {
        qword random = pcg_rand(pcg);
        if (random >= threshold) return random % max;
    }
}

// Grab from an external header function.
extern zend_module_entry pcg_module_entry;
#define phpext_pcg_ptr &pcg_module_entry;

// Keep this at the current PHP version to make things easier.
#define PHP_PCG_VERSION "7.3.9"

// Determine if PHP is compiled for thread-safe operations.
#if defined(ZTS) && defined(COMPILE_DL_PCG)
ZEND_TSRMLS_CACHE_EXTERN()
#endif
#endif