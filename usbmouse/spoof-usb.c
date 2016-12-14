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
} __attribute__ ((packed));

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
} __attribute__ ((packed));

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
};

struct usb_interface_descriptor {
    __u8 bNumEndpoints;
} __attribute__ ((packed));

struct usb_host_interface {
    struct usb_interface_descriptor desc;
    struct usb_host_endpoint *endpoint;
};

struct usb_interface {
    struct device dev;
    struct usb_host_interface *cur_altsetting;
};

/* Work on these: */
extern struct urb *usb_alloc_urb(int iso_packets, gfp_t mem_flags);
extern void usb_free_urb(struct urb *urb);
extern int usb_submit_urb(struct urb *urb, gfp_t mem_flags);
extern void usb_kill_urb(struct urb *urb);

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
void *usb_alloc_coherent(struct usb_device *dev, size_t size, gfp_t mem_falgs, dma_addr_t *dma);
void usb_free_coherent(struct usb_device *dev, size_t size, void *addr, dma_addr_t dma);

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
