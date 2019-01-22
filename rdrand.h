/*
 * Copyright © 2019 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

/*
 * Access x86-64 CPU (AMD or Intel) hardware-assisted RNG.
 * https://hackage.haskell.org/package/crypto-random-0.0.9/src/cbits/rdrand.c
 */

#include <assert.h>
#include <stdint.h>

#ifdef RDRAND

static int
has_rdrand(void) {
    uint32_t eax, ecx;
    eax = 0x01;
    asm("cpuid" : "+a" (eax), "=c" (ecx) : : "ebx", "edx");
    return (ecx >> 30) & 1;
}

static inline uint64_t
rdrand64_checked(void) {
    uint8_t err;
    uint64_t r;
    asm volatile ("rdrand %0; setc %1" : "=r" (r), "=qm" (err) : : "cc");
    assert(err == 1);
    return r;
}

static inline uint64_t
rdrand64(void) {
    uint64_t r;
    asm volatile ("%=: rdrand %0; jnc %=b" : "=r" (r) : : "cc");
    return r;
}

static inline uint32_t
rdrand32(void) {
    uint32_t r;
    asm volatile ("%=: rdrand %0; jnc %=b" : "=r" (r) : : "cc");
    return r;
}

#else

#include <stdlib.h>
#include <unistd.h>

static int
has_rdrand(void) {
    srandom(getpid());
    return 1;
}

static inline uint64_t
rdrand64(void) {
    uint64_t r1 = lrand48();
    uint64_t r2 = lrand48();
    uint64_t r3 = lrand48();
    return (r1 << 33) ^ (r2 << 24) ^ r3;
}

static inline uint64_t
rdrand64_checked(void) {
    return rdrand64();
}

static inline uint32_t
rdrand32(void) {
    return rdrand64();
}

#endif
