#include "spoof-utils.c"

struct device {
    struct device *parent;
    void *driver_data;
};

// Contents not used
struct input_id {
    __u16 bustype;
    __u16 vendor;
    __u16 product;
    __u16 version;
};

struct input_dev {
    const char *name;
    const char *phys;
    struct input_id id;
    struct device dev;
    unsigned long evbit[((((0x1f +1)) + (8 * sizeof(long)) - 1) / (8 * sizeof(long)))];
    unsigned long keybit[((((0x2ff +1)) + (8 * sizeof(long)) - 1) / (8 * sizeof(long)))];
    unsigned long relbit[((((0x0f +1)) + (8 * sizeof(long)) - 1) / (8 * sizeof(long)))];
    int (*open)(struct input_dev *dev);
    void (*close)(struct input_dev *dev);
    /* Added because input_allocate_device */
    struct mutex mutex;
};

/* Work on these: */
void device_initialize(struct device);
struct input_dev *input_allocate_device(void)
{
    static atomic_t input_no = ATOMIC_INIT(-1);
    struct input_dev *dev;

    dev = kzalloc(sizeof(struct input_dev), GFP_KERNEL);
    if (dev) {
        // Ignoring these
        //dev->dev.type = &input_dev_type;
        //dev->dev.class = &input_class;
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

/* Work on these: */
void input_free_device(struct input_dev *dev)
{
    if (dev) {
        if (dev->devres_managed)
            WARN_ON(devres_destroy(dev->dev.parent,
                                    devm_input_device_release,
                                    devm_input_device_match,
                                    dev));
        input_put_device(dev);
    }
}

/* Work on these: */
int input_register_device(struct input_dev *dev)
{
    struct input_devres *devres = NULL;
    struct input_handler *handler;
    unsigned int packet_size;
    const char *path;
    int error;

    if (dev->devres_managed) {
        devres = devres_alloc(devm_input_device_unregister,
                              sizeof(struct input_devres), GFP_KERNEL);
        if (!devres)
            return -ENOMEM;

        devres->input = dev;
    }

    /* Every input device generates EV_SYN/SYN_REPORT events. */
    __set_bit(EV_SYN, dev->evbit);

    /* KEY_RESERVED is not supposed to be transmitted to userspace. */
    __clear_bit(KEY_RESERVED, dev->keybit);

    /* Make sure that bitmasks not mentioned in dev->evbit are clean. */
    input_cleanse_bitmasks(dev);

    packet_size = input_estimate_events_per_packet(dev);
    if (dev->hint_events_per_packet < packet_size)
        dev->hint_events_per_packet = packet_size;

    dev->max_vals = dev->hint_events_per_packet + 2;
    dev->vals = kcalloc(dev->max_vals, sizeof(*dev->vals), GFP_KERNEL);
    if (!dev->vals) {
        error = -ENOMEM;
        goto err_devres_free;
    }

    /*
     * If delay and period are pre-set by the driver, then autorepeating
     * is handled by the driver itself and we don't do it in input.c.
     */
    if (!dev->rep[REP_DELAY] && !dev->rep[REP_PERIOD])
        input_enable_softrepeat(dev, 250, 33);

    if (!dev->getkeycode)
        dev->getkeycode = input_default_getkeycode;

    if (!dev->setkeycode)
        dev->setkeycode = input_default_setkeycode;

    error = device_add(&dev->dev);
    if (error)
        goto err_free_vals;

    path = kobject_get_path(&dev->dev.kobj, GFP_KERNEL);
    pr_info("%s as %s\n",
            dev->name ? dev->name : "Unspecified device",
            path ? path : "N/A");
    kfree(path);

    error = mutex_lock_interruptible(&input_mutex);
    if (error)
        goto err_device_del;

    list_add_tail(&dev->node, &input_dev_list);

    list_for_each_entry(handler, &input_handler_list, node)
    input_attach_handler(dev, handler);

    input_wakeup_procfs_readers();

    mutex_unlock(&input_mutex);

    if (dev->devres_managed) {
        dev_dbg(dev->dev.parent, "%s: registering %s with devres.\n",
                __func__, dev_name(&dev->dev));
        devres_add(dev->dev.parent, devres);
    }
    return 0;

err_device_del:
    device_del(&dev->dev);
err_free_vals:
    kfree(dev->vals);
    dev->vals = NULL;
err_devres_free:
    devres_free(devres);
    return error;
}

/* Work on these: */
void input_unregister_device(struct input_dev *dev)
{
    if (dev->devres_managed) {
            WARN_ON(devres_destroy(dev->dev.parent,
                                    devm_input_device_unregister,
                                    devm_input_device_match,
                                    dev));
            __input_unregister_device(dev);
            /*
             * We do not do input_put_device() here because it will be done
             * when 2nd devres fires up.
             */
    } else {
            __input_unregister_device(dev);
            input_put_device(dev);
    }
}

static inline void *dev_get_drvdata(const struct device *dev)
{
    return dev->driver_data;
}
static inline void dev_set_drvdata(struct device *dev, void *data);
{
    dev->driver_data = data;
}
static inline void *input_get_drvdata(struct input_dev *dev)
{
    return dev_get_drvdata(&dev->dev);
}
static inline void input_set_drvdata(struct input_dev *dev, void *data);
{
    dev_set_drvdata(&dev->dev, data);
}

/* Work on these: */
void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
    unsigned long flags;

    if (is_event_supported(type, dev->evbit, EV_MAX)) {
        spin_lock_irqsave(&dev->event_lock, flags);
        input_handle_event(dev, type, code, value);
        spin_unlock_irqrestore(&dev->event_lock, flags);
    }
}

static inline void input_sync(struct input_dev *dev);
{
    input_event(dev, 0x00, 0, 0);
}
static inline void input_report_key(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, 0x01, code, !!value);
}
static inline void input_report_rel(struct input_dev *dev, unsigned int code, int value);
{
    input_event(dev, 0x02, code, !!value);
}
