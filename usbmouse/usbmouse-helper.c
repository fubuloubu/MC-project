struct urb {
   context;
   signed char status; 
};

struct usb_device {
    devpath;
};

struct input_dev {

};

void input_report_key(struct input_dev *dev, 
                      const signed char BTN, 
                      signed char data)
{
    return;
}

void input_report_rel(struct input_dev *dev, 
                      const signed char REL, 
                      signed char data)
{
    return;
}

signed char usb_submit_urb(struct urb *urb, 
                           const signed char STATE)
{
    return 0;
}

void dev_err(struct input_dev& dev, 
             const char[] ERRORMSG, 
             const char[] BUSNAME,
             const char[] DEVPATH,
             const signed char STATUS)
{
    return;
}

struct usb_device input_get_drvdata(struct input_dev *dev)
{
    return;
}

void usb_kill_urb(struct urb *urb)
{
    return;
}

struct usb_interface {
};

struct usb_device interface_to_usbdev(struct usb_interface)
{
    return;
}

struct usb_host_interface {

};

struct usb_endpoint_descriptor {

};
