KERNEL_SOURCE=/lib/modules/$(shell uname -r)/build
CURR_DIR=$(shell pwd)
TARGET=virtual_spi_controller.ko
obj-m += virtual_spi_controller.o


all:
	$(MAKE) -C $(KERNEL_SOURCE) M=$(CURR_DIR) modules
clean:
	$(MAKE) -C $(KERNEL_SOURCE) M=$(CURR_DIR) clean
install:
	$(HOST_INSTALL) $(TARGET) $(IMAGES_DIR)

.PHONY:all clean install

