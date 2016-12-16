#include "spoof-utils.c"

struct usb_bus {
    const char *bus_name;
};

// Contents not used
struct usb_device_id;

struct usb_device_descriptor {
    __le16 idVendor;
    __le16 idProduct;
    /* Added because usb_to_input_id() */
    __le16 bcdDevice;
};
// Used to have:
//} __attribute__ ((packed));

/* Added because usb_fill_int_urb() */
enum usb_device_speed {
    USB_SPEED_UNKNOWN= 0,
    USB_SPEED_LOW, 
    USB_SPEED_FULL,
    USB_SPEED_HIGH,
    USB_SPEED_WIRELESS,
    USB_SPEED_SUPER,
    USB_SPEED_SUPER_PLUS,
};

struct usb_endpoint_descriptor {
    __u8 bEndpointAddress
    __u8 bInterval
    /* Added because usb_endpoint_is_int_in() */
    __u8 bmAttributes;
    /* Added because usb_maxpacket() */
    __le16 wMaxPacketSize;
};
// Used to have:
//} __attribute__ ((packed));

struct usb_host_endpoint {
    struct usb_endpoint_descriptor desc;
};

struct usb_device {
    char devpath[16];
    struct usb_bus *bus;
    struct device dev;
    struct usb_device_descriptor descriptor;
    char *manufacturer;
    char *product;
    struct usb_device *parent;
    /* Added because usb_fill_int_urb() */
    enum usb_device_speed speed;
    /* Added because usb_rcvintpipe() */
    int devnum;
    /* Added because usb_maxpacket() */
    struct usb_host_endpoint *ep_in[16];
};

/* Used in usb_alloc_urb() */
struct usb_iso_packet_descriptor;

/* Added because usb_fill_int_urb() */
typedef void (*usb_complete_t;)(struct urb *);

struct urb {
    void *context;
    int status;
    struct usb_device *dev;
    dma_addr_t transfer_dma;
    unsigned int transfer_flags;
    /* Added because usb_fill_int_urb() */
    unsigned int pipe;
    void *transfer_buffer;
    u32 transfer_buffer_length;
    usb_complete_t complete;
    int interval;
    int start_frame;
    /* Added because usb_alloc_urb() */
    struct kref kref;
    /* Added because usb_kill_urb() */
    atomic_t use_count;
    atomic_t reject;
    struct usb_host_endpoint *ep;
};

struct usb_interface_descriptor {
    __u8 bNumEndpoints;
};
// Used to have:
//} __attribute__ ((packed));

struct usb_host_interface {
    struct usb_interface_descriptor desc;
    struct usb_host_endpoint *endpoint;
};

struct usb_interface {
    struct device dev;
    struct usb_host_interface *cur_altsetting;
};

void usb_init_urb(struct urb *urb)
{
    if (urb) {
        memset(urb, 0, sizeof(*urb));
        kref_init(&urb->kref);
        INIT_LIST_HEAD(&urb->anchor_list);
    }
}
extern struct urb *usb_alloc_urb(int iso_packets, gfp_t mem_flags)
{
    struct urb *urb;

    urb = kmalloc(sizeof(struct urb) +
            iso_packets * sizeof(struct usb_iso_packet_descriptor),
            mem_flags);
    if (!urb) {
        //printk(KERN_ERR "alloc_urb: kmalloc failed\n");
        return NULL;
    }
    usb_init_urb(urb);
    return urb;
}

extern void usb_free_urb(struct urb *urb)
{
    if (urb)
        kref_put(&urb->kref, urb_destroy);
}

/* Work on these: */
extern int usb_submit_urb(struct urb *urb, gfp_t mem_flags)
{
    static int pipetypes[4] = {
        PIPE_CONTROL, PIPE_ISOCHRONOUS, PIPE_BULK, PIPE_INTERRUPT
    };
    int                             xfertype, max;
    struct usb_device               *dev;
    struct usb_host_endpoint        *ep;
    int                             is_out;
    unsigned int                    allowed;

    if (!urb || !urb->complete)
        return -EINVAL;
    if (urb->hcpriv) {
        WARN_ONCE(1, "URB %p submitted while active\n", urb);
        return -EBUSY;
    }

    dev = urb->dev;
    if ((!dev) || (dev->state < USB_STATE_UNAUTHENTICATED))
        return -ENODEV;

    /* For now, get the endpoint from the pipe.  Eventually drivers
     * will be required to set urb->ep directly and we will eliminate
     * urb->pipe.
     */
    ep = usb_pipe_endpoint(dev, urb->pipe);
    if (!ep)
        return -ENOENT;

    urb->ep = ep;
    urb->status = -EINPROGRESS;
    urb->actual_length = 0;

    /* Lots of sanity checks, so HCDs can rely on clean data
     * and don't need to duplicate tests
     */
    xfertype = usb_endpoint_type(&ep->desc);
    if (xfertype == USB_ENDPOINT_XFER_CONTROL) {
        struct usb_ctrlrequest *setup =
                (struct usb_ctrlrequest *) urb->setup_packet;

        if (!setup)
            return -ENOEXEC;
        is_out = !(setup->bRequestType & USB_DIR_IN) ||
                 !setup->wLength;
    } else {
        is_out = usb_endpoint_dir_out(&ep->desc);
    }

    /* Clear the internal flags and cache the direction for later use */
    urb->transfer_flags &= ~(URB_DIR_MASK | URB_DMA_MAP_SINGLE |
                    URB_DMA_MAP_PAGE | URB_DMA_MAP_SG | URB_MAP_LOCAL |
                    URB_SETUP_MAP_SINGLE | URB_SETUP_MAP_LOCAL |
                    URB_DMA_SG_COMBINED);
    urb->transfer_flags |= (is_out ? URB_DIR_OUT : URB_DIR_IN);

    if (xfertype != USB_ENDPOINT_XFER_CONTROL &&
                    dev->state < USB_STATE_CONFIGURED)
        return -ENODEV;

    max = usb_endpoint_maxp(&ep->desc);
    if (max <= 0) {
        dev_dbg(&dev->dev,
                "bogus endpoint ep%d%s in %s (bad maxpacket %d)\n",
                usb_endpoint_num(&ep->desc), is_out ? "out" : "in",
                __func__, max);
        return -EMSGSIZE;
    }

    /* periodic transfers limit size per frame/uframe,
     * but drivers only control those sizes for ISO.
     * while we're checking, initialize return status.
     */
    if (xfertype == USB_ENDPOINT_XFER_ISOC) {
        int     n, len;

        /* SuperSpeed isoc endpoints have up to 16 bursts of up to
         * 3 packets each
         */
        if (dev->speed == USB_SPEED_SUPER) {
            int     burst = 1 + ep->ss_ep_comp.bMaxBurst;
            int     mult = USB_SS_MULT(ep->ss_ep_comp.bmAttributes);
            max *= burst;
            max *= mult;
        }

        /* "high bandwidth" mode, 1-3 packets/uframe? */
        if (dev->speed == USB_SPEED_HIGH) {
            int     mult = 1 + ((max >> 11) & 0x03);
            max &= 0x07ff;
            max *= mult;
        }

        if (urb->number_of_packets <= 0)
            return -EINVAL;
        for (n = 0; n < urb->number_of_packets; n++) {
            len = urb->iso_frame_desc[n].length;
            if (len < 0 || len > max)
                return -EMSGSIZE;
            urb->iso_frame_desc[n].status = -EXDEV;
            urb->iso_frame_desc[n].actual_length = 0;
        }
    } else if (urb->num_sgs && !urb->dev->bus->no_sg_constraint &&
                    dev->speed != USB_SPEED_WIRELESS) {
        struct scatterlist *sg;
        
        int i;
        for_each_sg(urb->sg, sg, urb->num_sgs - 1, i)
            if (sg->length % max)
                return -EINVAL;
    }

    /* the I/O buffer must be mapped/unmapped, except when length=0 */
    if (urb->transfer_buffer_length > INT_MAX)
            return -EMSGSIZE;

    /*
     * stuff that drivers shouldn't do, but which shouldn't
     * cause problems in HCDs if they get it wrong.
     */

    /* Check that the pipe's type matches the endpoint's type */
    if (usb_pipetype(urb->pipe) != pipetypes[xfertype])
        dev_WARN(&dev->dev, "BOGUS urb xfer, pipe %x != type %x\n",
                usb_pipetype(urb->pipe), pipetypes[xfertype]);

    /* Check against a simple/standard policy */
    allowed = (URB_NO_TRANSFER_DMA_MAP | URB_NO_INTERRUPT | URB_DIR_MASK |
                    URB_FREE_BUFFER);
    switch (xfertype) {
    case USB_ENDPOINT_XFER_BULK:
    case USB_ENDPOINT_XFER_INT:
        if (is_out)
            allowed |= URB_ZERO_PACKET;
        /* FALLTHROUGH */
    case USB_ENDPOINT_XFER_CONTROL:
        allowed |= URB_NO_FSBR; /* only affects UHCI */
        /* FALLTHROUGH */
    default:                        /* all non-iso endpoints */
        if (!is_out)
            allowed |= URB_SHORT_NOT_OK;
        break;
    case USB_ENDPOINT_XFER_ISOC:
        allowed |= URB_ISO_ASAP;
        break;
    }
    allowed &= urb->transfer_flags;

    /* warn if submitter gave bogus flags */
    if (allowed != urb->transfer_flags)
        dev_WARN(&dev->dev, "BOGUS urb flags, %x --> %x\n",
                urb->transfer_flags, allowed);

    /*
     * Force periodic transfer intervals to be legal values that are
     * a power of two (so HCDs don't need to).
     *
     * FIXME want bus->{intr,iso}_sched_horizon values here.  Each HC
     * supports different values... this uses EHCI/UHCI defaults (and
     * EHCI can use smaller non-default values).
     */
    switch (xfertype) {
    case USB_ENDPOINT_XFER_ISOC:
    case USB_ENDPOINT_XFER_INT:
        /* too small? */
        switch (dev->speed) {
        case USB_SPEED_WIRELESS:
            if ((urb->interval < 6) && (xfertype == USB_ENDPOINT_XFER_INT))
                return -EINVAL;
        default:
            if (urb->interval <= 0)
                return -EINVAL;
            break;
        }
        /* too big? */
        switch (dev->speed) {
        case USB_SPEED_SUPER:   /* units are 125us */
            /* Handle up to 2^(16-1) microframes */
            if (urb->interval > (1 << 15))
                return -EINVAL;
            max = 1 << 15;
            break;
        case USB_SPEED_WIRELESS:
            if (urb->interval > 16)
                return -EINVAL;
            break;
        case USB_SPEED_HIGH:    /* units are microframes */
            /* NOTE usb handles 2^15 */
            if (urb->interval > (1024 * 8))
                urb->interval = 1024 * 8;
            max = 1024 * 8;
            break;
        case USB_SPEED_FULL:    /* units are frames/msec */
        case USB_SPEED_LOW:
            if (xfertype == USB_ENDPOINT_XFER_INT) {
                if (urb->interval > 255)
                    return -EINVAL;
                /* NOTE ohci only handles up to 32 */
                max = 128;
            } else {
                if (urb->interval > 1024)
                    urb->interval = 1024;
                /* NOTE usb and ohci handle up to 2^15 */
                max = 1024;
            }
            break;
        default:
            return -EINVAL;
        }
        if (dev->speed != USB_SPEED_WIRELESS) {
            /* Round down to a power of 2, no more than max */
            urb->interval = min(max, 1 << ilog2(urb->interval));
        }
    }

    return usb_hcd_submit_urb(urb, mem_flags);
}

/* Work on these: */
extern void usb_kill_urb(struct urb *urb)
{
    //might_sleep(); // Skipping this debug statement
    if (!(urb && urb->dev && urb->ep))
        return;
    atomic_inc(&urb->reject);

    usb_hcd_unlink_urb(urb, -ENOENT);
    wait_event(usb_kill_urb_queue, atomic_read(&urb->use_count) == 0);

    atomic_dec(&urb->reject);
}

static inline void usb_fill_int_urb(struct urb *
        struct usb_device *dev,
        unsigned int pipe,
        void *transfer_buffer,
        int buffer_length,
        usb_complete_t complete_fn,
        void *context,
        int interval);
{
    urb->dev = dev;
    urb->pipe = pipe;
    urb->transfer_buffer = transfer_buffer;
    urb->transfer_buffer_length = buffer_length;
    urb->complete = complete_fn;
    urb->context->context;

    if (dev->speed == USB_SPEED_HIGH || dev->speed == USB_SPEED_SUPER) {
        interval = ({
            typeof((typeof(interval))({
                typeof(interval) _max1 = (interval);
                typeof(1) _max2= (1);      
                (void) (&_max1 == &_max2);
                _max1 > _max2 ? _max1 : _max2;
            })) _min1 = ((typeof(interval))({
                typeof(interval _max1 = (interval);
                typeof(1) _max2= (1);      
                (void) (&_max1 == &_max2);
                _max1 > _max2 ? _max1 : _max2;
            }));
            typeof(16) _min2 = (16);
            (void) (&_min1 == &_min2);
            _min1 < _min2 ? _min1 : _min2;
        });

        urb->interval = 1 << (interval -1);
    } else {
        urb->interval = interval;
    }

    urb->start_frame = -1;
}

static inline void *usb_get_intfdata(struct usb_interface *intf)
{
    return dev_get_drvdata(&intf->dev);
}
static inline void usb_set_intfdata(struct usb_interface *intf, void *data)
{
    dev_set_drvdata(&intf->dev, data);
}

static inline struct usb_device *interface_to_usbdev(struct usb_interface *intf)
{
    return ({
        const typeof( ((struct usb_device *)0)->dev ) *__mptr = (intf->dev.parent);
        (struct usb_device *)( (char *)__mptr - __builtin_offsetof(struct usb_device, dev) );
    });
}

static inline int usb_endpoint_xfer_int(const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bmAttributes & 0x03) == 3);
}
static inline int usb_endpoint_dir_out(const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bEndpointAddress & 0x80) == 0);
}
static inline int usb_endpoint_is_int_in(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_xfer_int(epd) && usb_endpoint_dir_out(epd);
}

static inline unsigned int __create_pipe(struct usb_device *dev, unsigned int endpoint)
{
    return (dev->devnum << 8) | (endpoint << 15);
}
#define usb_rcvintpipe(dev, ep_addr) ((1 << 30) | __create_pipe(dev, ep_addr) | 0x80)

static inline int usb_endpoint_maxp(const struct usb_endpoint_descriptor *epd)
{
    return le16_to_cpu(epd->wMaxPacketSize);
}
static inline __u16 usb_maxpacket(struct usb_device *udev, int pipe, int is_out)
{
    struct usb_host_endpoint *ep;
    unsigned epnum = (((pipe) >> 15) & 0xf);
    
    if (is_out) {
        /* There was some weird stuff here, but it 
         * looked like just error reporting, so I 
         * removed it */
        ep = udev->ep_in[epnum];
    }
    if (!ep)
        return 0;

    return usb_endpoint_maxp(&ep->desc);
}

#define usb_pipeout(pipe) (!((pipe) & 0x80))

/* Work on these: */
void *usb_alloc_coherent(struct usb_device *dev, size_t size, gfp_t mem_flags, dma_addr_t *dma)
{
    if (!dev || !dev->bus)
        return NULL;
    return hcd_buffer_alloc(dev->bus, size, mem_flags, dma);
}

/* Work on these: */
void usb_free_coherent(struct usb_device *dev, size_t size, void *addr, dma_addr_t dma)
{
    if (!dev || !dev->bus)
        return;
    if (!addr)
        return;
    hcd_buffer_free(dev->bus, size, addr, dma);
}

static inline int usb_make_path(struct usb_device *dev, char *buf, size_t size)
{
    int actual;
    actual = snprintf(buf, size, "usb-%s-%s", dev->bus->bus_name, dev->devpath);
    return (actual >= (int)size) ? -1 : actual;
}

static inline void usb_to_input_id(const struct usb_device *dev, struct input_id *id)
{
    id->bustype = 0x03;
    id->vendor = le16_to_cpu(dev->descriptor.idVendor);
    id->product = le16_to_cpu(dev->descriptor.idProduct);
    id->version = le16_to_cpu(dev->descriptor.bcdDevice);
}
