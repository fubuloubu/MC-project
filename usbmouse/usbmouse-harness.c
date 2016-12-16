#include "usbmouse-full.c"
//#include "../klee_src/include/klee/klee.h"

int main() {
    int status;
    struct input_dev *dev;
    //klee_make_symbolic(dev, sizeof(dev), "dev");
    status = usb_mouse_open(dev);
    return status;
}
