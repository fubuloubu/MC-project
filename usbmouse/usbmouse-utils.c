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
