/* Copied from kernel.h */
typedef unsigned long long dma_addr_t;

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
#define le16_to_cpu(x) (( __u16)(__le16)(x))

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

#define GFP_ATOMIC  ((( gfp_t)0x20u)|(( gfp_t)0x80000u)|(( gfp_t)0x2000000u))
#define GFP_KERNEL  ((( gfp_t)(0x400000u|0x2000000u)) | (( gfp_t)0x40u) | (( gfp_t)0x80u))

#define BITMASK(VAL) (1UL << ((VAL) % 64))
#define BITWORD(VAL) ((VAL) / 64)
#define EV_KEY      0x01
#define EV_REL      0x02

#define URB_NO_TRANSFER_DMA_MAP 0x0004

//Not sure if necessary
//#define NULL ((void *)0)
