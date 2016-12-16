#ifndef UTILS_C
#define UTILS_C

#include <stdlib.h>
#include <string.h>

typedef unsigned short __u16;

/* From list.h */
// Doubly linked list
struct list_head {
    struct list_head *next, *prev;
};
static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

/* Modified from atomic.h */
typedef struct {
    int counter;
} atomic_t;
static inline int atomic_get(atomic_t *v)
{
    return v->counter;
}
static inline void atomic_set(atomic_t *v, int i)
{
    v->counter = i;
}
#define ATOMIC_INIT(i) { i }
static inline void atomic_inc(atomic_t *v)
{
    v->counter++;
}
/* Macro should produce:
 * asm volatile(LOCK_PREFIX "incl %0" : "+m" (v->counter));
 * LOCK_PREFIX "\n\tlock; "
 * lock; incl 
 * Same with:
 */
static inline void atomic_dec(atomic_t *v)
{
    v->counter--;
}

/* From spinlock_types(_up).h */
typedef struct {
    volatile unsigned int slock;
} arch_spinlock_t;

typedef struct raw_spinlock {
    arch_spinlock_t raw_lock;
    /* Some other debug stuff taken out... */
} raw_spinlock_t;

typedef struct spinlock {
    union {
        struct raw_spinlock rlock;
        /* Some other debug stuff taken out... */
    };
} spinlock_t;
#define __RAW_SPIN_LOCK_INITIALIZER(lockname)   \
{                                               \
    .raw_lock = __ARCH_SPIN_LOCK_UNLOCKED,      \
    SPIN_DEBUG_INIT(lockname)                   \
    SPIN_DEP_MAP_INIT(lockname)                 \
}

# define raw_spin_lock_init(lock)               \
do { *(lock) = __RAW_SPIN_LOCK_UNLOCKED(lock); } while (0)

static __always_inline raw_spinlock_t *spinlock_check(spinlock_t *lock)
{
    return &lock->rlock;
}

#define spin_lock_init(_lock)               \
do {                                        \
    spinlock_check(_lock);                  \
    raw_spin_lock_init(&(_lock)->rlock);    \
} while (0)

/* From mutex.h */
struct mutex {
    /* 1: unlocked, 0: locked, negative: locked, possible waiters */
    atomic_t count;
    spinlock_t wait_lock;
    struct list_head wait_list;
};
void mutex_init(struct mutex);

/* from kref.h */
struct kref {
    atomic_t refcount;
};
static inline void kref_init(struct kref *kref)
{
    atomic_set(&kref->refcount, 1);
}
static inline int kref_put(struct kref *kref, void (*release)(struct kref *kref))
{
    // Old
    //return kref_sub(kref, 1, release);
    
    // Give this a shot...
    if (atomic_get(&kref->refcount) > 1) {
        atomic_dec(&kref->refcount);
        release(kref);
        return 1;
    } else {
        atomic_dec(&kref->refcount);
        return 0;
    }
}

/* Copied from kernel.h */
typedef unsigned long long dma_addr_t;

//#define GFP_ATOMIC 0x1001
// Used to be ((( gfp_t)0x20u)|(( gfp_t)0x80000u)|(( gfp_t)0x2000000u))
//#define GFP_KERNEL 0x1002
// Used to be ((( gfp_t)(0x400000u|0x2000000u)) | (( gfp_t)0x40u) | (( gfp_t)0x80u))

typedef enum {
    GFP_KERNEL,
    GFP_ATOMIC,
} gfp_t;

//extern void *__kmalloc_fake;
static inline void *kmalloc(size_t s, gfp_t gfp)
{
    // if (__kmalloc_fake)
    //    return __kmalloc_fake;
    return malloc(s);
}
static inline void *kzalloc(size_t s, gfp_t gfp)
{
    /* Ignoring gfp implementation details */
    void *p = malloc(s);
    memset(p, 0, s);
    return p;
}

static inline void kfree(void *p)
{
    /* Ignoring gfp implementation details */
    free(p);
}

/* Assuming big endian */
#define le16_to_cpu(x) __swab16((__force __u16)(__le16)(x))

/* Should be in stdlib:
 * strlcpy()
 * strlcat()
 * strlen()
 */

/* Bypass functionality (NULL MACROS) */
#define dev_err(dev, format, ...)
#define snprintf(string, size, format, ...)

/* Macros and Parameters */
#define ECONNRESET  104
#define ESHUTDOWN   108
#define ENOENT      2
#define EIO         5
#define ENOMEM      12
#define ENODEV      19

#define BTN_MOUSE   0x110
#define BTN_LEFT    0x110
#define BTN_RIGHT   0x111
#define BTN_MIDDLE  0x112
#define BTN_SIDE    0x113
#define BTN_EXTRA   0x114

#define REL_X       0x00
#define REL_Y       0x01
#define REL_WHEEL   0x08

#define BITMASK(VAL) (1UL << ((VAL) % 64))
#define BITWORD(VAL) ((VAL) / 64)
#define EV_KEY      0x01
#define EV_REL      0x02

#define URB_NO_TRANSFER_DMA_MAP 0x0004

//Not sure if necessary
//#define NULL ((void *)0)

//UTILS_C
#endif
