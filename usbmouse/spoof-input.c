struct device {
    struct device *parent;
    void *driver_data;
};

// Contents not used
struct input_id;

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
}

/* Work on these: */
struct input_dev *input_allocate_device(void);
void input_free_device(struct input_dev *dev);

/* Work on these: */
int input_register_device(struct input_dev *);
void input_unregister_device(struct input_dev *);

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
void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value);
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
