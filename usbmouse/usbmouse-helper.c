/* FILE: usbmouse-helper.c
 * DESC: all functions and types required to compile
 *       usbmouse.c without linux headers.
 *
 * NOTE: Minimal implementation as copied from linux 
 *       headers
 * NOTE: Some simplications made, see NOTES below
 * NOTE: Some modifications made for correct simulation
 */

/* FROM: /lib/modules/`uname -r`/build/include/linux
 * FILE: types.h 
 *
 * NOTE: based on 32 or 64 bit archicture,
 *       so substituting unsigned int type here
 */
typedef unsigned int dma_addr_t;
/* END: types.h */

/* FROM: /lib/modules/`uname -r`/build/include/linux
 * FILE: spinlock_types_up.h */
typedef struct {
    volatile unsigned int slock;
} arch_spinlock_t;
/* END: spinlock_types_up.h */

/* FROM: /lib/modules/`uname -r`/build/include/linux
 * FILE: spinlock_types.h */
typedef struct raw_spinlock {
    arch_spinlock_t raw_lock;
} raw_spinlock_t;

typedef struct spinlock {
    union {
        struct raw_spinlock rlock;
    };
} spinlock_t;
/* END: spinlock_types.h */

/* FROM: /lib/modules/`uname -r`/build/arch/x86/include/asm
 * FILE: bitops.h
 *
 * TODO: Verify correct operation of this code or modify it
 */
static __always_inline int constant_test_bit(long nr, 
                                             const volatile unsigned long *addr)
{
    return ((1UL << (nr & (BITS_PER_LONG-1))) &
            (addr[nr >> _BITOPS_LONG_SHIFT])) != 0;
}

static inline int variable_test_bit(long nr, 
                                    volatile const unsigned long *addr)
{
    int oldbit;

    asm volatile("bt %2,%1\n\t"
            "sbb %0,%0"
            : "=r" (oldbit)
            : "m" (*(unsigned long *)addr), "Ir" (nr));

    return oldbit;
}

#define test_bit(nr, addr)          \
    (__builtin_constant_p((nr))     \
    ? constant_test_bit((nr), (addr)) \
    : variable_test_bit((nr), (addr)))
/* END: bitops.h */

static __always_inline void spin_unlock_irqrestore(spinlock_t *lock, 
                                                   unsigned long flags)
{
    raw_spin_unlock_irqrestore(&lock->rlock, flags);
}

/* FROM: /lib/modules/`uname -r`/build/include/linux/
 * FILE: device.h */
struct device {
};
/* END: device.h */

/* FROM: /lib/modules/`uname -r`/build/drivers/input
 * FILE: input.c/.h 
 *
 * TODO: simplify spinlock handler here 
 * NOTE: DO NOT NEED to handle event, don't care 
 */
static inline int is_event_supported(unsigned int code,
                                     unsigned long *bm,
                                     unsigned int max)
{
    return code <= max && test_bit(code, bm);
}

void input_event(struct input_dev *dev, 
                 unsigned int type, 
                 unsigned int code, 
                 int value)
{
    unsigned long flags;
    
    if (is_event_supported(type, dev->evbit, EV_MAX)) {
        spin_lock_irqsave(&dev->event_lock, flags);
        input_handle_event(dev, type, code, value);
        spin_unlock_irqrestore(&dev->event_lock, flags);
    }
}

struct input_dev {
    const char *name;
    const char *phys;
    struct device dev;
    unsigned long  evbit[BITS_TO_LONGS(EV_CNT) ];
    unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];
    unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
     int  (*open)(struct input_dev *dev);
    void (*close)(struct input_dev *dev);
    spinlock_t event_lock;
};

static inline void input_report_key(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, EV_KEY, code, !!value);
}

static inline void input_report_rel(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, EV_REL, code, value);
}

static inline void input_sync(struct input_dev *dev)
{
    input_event(dev, EV_SYN, SYN_REPORT, 0);
}

struct input_dev *input_allocate_device(void)
{
    static atomic_t input_no = ATOMIC_INIT(-1);
    struct input_dev *dev;

    dev = kzalloc(sizeof(struct input_dev), GFP_KERNEL);
    if (dev) {
        dev->dev.type = &input_dev_type;
        dev->dev.class = &input_class;
        device_initialize(&dev->dev);
        mutex_init(&dev->mutex);
        spin_lock_init(&dev->event_lock);
        init_timer(&dev->timer);
        INIT_LIST_HEAD(&dev->h_list);
        INIT_LIST_HEAD(&dev->node);

        dev_set_name(&dev->dev, "input%lu",
                     (unsigned long)atomic_inc_return(&input_no));

        __module_get(THIS_MODULE);
    }

    return dev;
}
/* END: input.h */

/* TODO: find
 * maybe in uapi? 
 * might not be relevant */
struct usb_device_descriptor {
};

/* FROM: /lib/modules/`uname -r`/build/include/linux/
 * FILE: usb.h */
struct urb {
   void *context;
   int  status; 
};

struct usb_bus {
};

struct usb_device {
    struct  device dev;
    struct  usb_bus *bus;
    char    devpath[16];
    char    *manufacturer;
    char    *product;
    struct  usb_device_descriptor descriptor;
};

struct usb_interface_descriptor {
};

struct usb_endpoint_descriptor {
};

struct usb_host_endpoint {
};

struct usb_host_interface {
    struct usb_interface_descriptor desc;
    struct usb_host_endpoint *endpoint;
};

struct usb_interface {
    struct usb_host_interface *cur_altsetting;
    struct device dev;
};
/* END: usb.h */

signed char usb_submit_urb(struct urb *urb, 
                           const signed char STATE)
{
    return 0;
}

void dev_err(struct input_dev dev, 
             const char[] ERRORMSG, 
             const char[] BUSNAME,
             const char[] DEVPATH,
             const signed char STATUS)
{
    return;
}

struct usb_device input_get_drvdata(struct input_dev *dev)
{
    struct usb_device drvdata;
    return drvdata;
}

void usb_kill_urb(struct urb *urb)
{
    return;
}

struct usb_device interface_to_usbdev(struct usb_interface)
{
    struct usb_device device;
    return device;
}

int usb_rcvintpipe(struct input_dev *dev, int endpoint_address)
{
    return 0;
}

// not sure of return type
int usb_pipeout(int pipe)
{
    return 0;
}

int usb_maxpacket(struct input_dev *dev, int pipe, int pipeout)
{
    return 0;
}

signed char* usb_alloc_coherant(struct input_dev *dev, 
                                int something, 
                                int gfp, 
                                dma_addr_t data_dma)
{
    signed char *data;
    return data;
}

struct urb* usb_alloc_urb(int something, int gfp)
{
    struct urb *urb;
    return urb;
}

void usb_make_path(struct input_dev *dev, char[64] phys, int size)
{
    return;
}

void usb_to_input_id(struct input_dev *dev, int& id)
{
    id = 0;
    return;
}

void usb_fill_int_urb(struct urb *irq,
                      struct input_dev *dev,
                      int pipe,
                      char[] data,
                      int p,
                      void (*func)(struct urb urb),
                      struct input_dev *input,
                      interval)
{
    return;
}

int input_register_device(struct input_dev *dev)
{
    return 0;
}

input_unregister_device(struct input_dev *dev)
{
    return;
}

void usb_set_intfdata(struct usb_interface *intf, struct input_dev *dev)
{
    return;
}

void usb_free_urb(struct urb *urb)
{
    return;
}

void usb_free_coherant(struct input_dev dev, int flag, char[] data, data_dma_t data_dma)
{
    return;
}

struct usb_interface usb_get_intfdata(struct usb_interface)
{
    struct usb_interface intf;
    return intf;
}

void input_set_drvdata(struct input_dev *dev, struct input_dev *input)
{
    return;
}

void input_free_device(struct input_dev dev)
{
    return;
}

// memory allocation return type?
int kzalloc(int alloc_size, int gfp)
{
   return 0; 
}

void kfree(char[] mem)
{
    return;
}
