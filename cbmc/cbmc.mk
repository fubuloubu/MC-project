../usbmouse/usbmouse.i:
	@echo "    GCC usbmouse"
	@cd ../usbmouse && \
		$(MAKE) all >/dev/null 2>/dev/null

DRIVER_METHODS =usb_mouse_irq 
DRIVER_METHODS+=usb_mouse_open
DRIVER_METHODS+=usb_mouse_close
DRIVER_METHODS+=usb_mouse_probe
DRIVER_METHODS+=usb_mouse_disconnect

all: $(addsuffix .results,$(DRIVER_METHODS))

CBMC_FLAGS=
#CBMC_FLAGS+=--show-claims
#CBMC_FLAGS+=--show-vcc
CBMC_FLAGS+=--unwind 100
CBMC_FLAGS+=--unwinding-assertions

%.results: ../usbmouse/usbmouse.i
	@echo "   CBMC $*"
	@# Ignoring failures below because 
	@# we may want to look at them
	@-cbmc $< $(CBMC_FLAGS) --function $* >$@ 2>/dev/null

.PHONY: clean
clean:
	@echo "  CLEAN cbmc-results"
	@cd ../usbmouse && \
		$(MAKE) clean >/dev/null
	@rm -f *.results
